#ifndef QC_RACK_H
#define QC_RACK_H

#include "bin.h"
#include <vector>
#include <ranges>
#include <iostream>
#include <iomanip>

using Row = std::vector<Slot>;
using Rows = std::vector<Row>;
// Crosscuts are derivate, actual objects are in rows, hence reference_wrapper
using ColumnCrosscut = std::vector<std::reference_wrapper<const Slot>>;

class Rack{
 public:
   template <std::ranges::input_range Range>
   requires std::convertible_to<std::ranges::range_value_t<Range>, Bin>
   Rack(Range&& r)
    : slots(std::forward<Range>(r).begin(), std::forward<Range>(r).end()) {
        
        // build stats from the slots before organizing them
        stats(slots);
        
        // organize slots into the member `slotRows`
        buildGridFromExistingBins(slots);
        
        // order the vectors of 'rows' from top to bottom
        sortRowOrderByFirstBinInRow();

        // if bins are pulled out, then back fill empty slots in the grid
        backFillEmptySlots();
    }

   // return the row number that the bin is in
   size_t findBinRow(uint32_t bin_id) const;

   // return the column number that the bin is in
   size_t findBinColumn(uint32_t bin_id) const;

   ~Rack();

 private:
    // Returns the number of columns (slots) in the row at the given index
   inline size_t numColumnsInRow(size_t rowIndex) const {
      return slotRows.at(rowIndex).size();
   }

    // Make the Rack stream operator a friend for access to private members
    friend std::ostream& operator<<(std::ostream& os, const Rack& rack);

   // Splits vector of slots from construction into the member `slotRows`
   bool buildGridFromExistingBins(const Row&);

   // Assumes vector already row grouped. Sorts them hi to low
   bool sortRowOrderByFirstBinInRow();

   // Looks where bins aren't and fills them with empty slots. returns true.
   bool backFillEmptySlots();

   // Returns Slot references at the same index (aka Column) per row of slotRows
   ColumnCrosscut getColumnCrossCut(size_t columnIndex) const;

   // Returns the minimum CenterX value in a cross-cut of slots
   uint16_t getMinXInCrossCut(const ColumnCrosscut& crossCut, size_t column) const;

   // Returns the number of bins in the longest row (tie okay!)
   size_t getMaxColumns() const;
   
   Row slots;

    struct Stats {
        int16_t avgBinHeight = 0;
        int16_t avgBinWidth = 0;
        const Bin* highestBin = nullptr;   // lowest center().y
        const Bin* lowestBin = nullptr;    // highest center().y
        const Bin* leftmostBin = nullptr;  // lowest center().x
        const Bin* rightmostBin = nullptr; // highest center().x
        int16_t numColumns = 0; // Number of columns in the rack

        template <typename Range>
        Stats& operator()(const Range& r) {
            size_t count = 0;
            avgBinHeight = 0;
            avgBinWidth = 0;
            
            // First pass: calculate averages
            for (const Slot& slot : r) {
                if (!slot.isFilled()) continue;
                const Bin& bin = slot.bin.value();
                ++count;
                avgBinHeight += bin.height();
                avgBinWidth += bin.width();
                
                // Update extremes...
                if (!highestBin || bin.center().y < highestBin->center().y)
                    highestBin = &bin;
                if (!lowestBin || bin.center().y > lowestBin->center().y)
                    lowestBin = &bin;
                if (!leftmostBin || bin.center().x < leftmostBin->center().x)
                    leftmostBin = &bin;
                if (!rightmostBin || bin.center().x > rightmostBin->center().x)
                    rightmostBin = &bin;
            }
            
            avgBinHeight = count ? avgBinHeight / count : 0;
            avgBinWidth = count ? avgBinWidth / count : 0;
            
            // Use consistent tolerance for all bins
            int tolerance = avgBinWidth / 2;
            
            
            std::vector<int> uniqueXs;

            // Second pass: find unique X coordinates using consistent tolerance
            for (const Slot& slot : r) {
                if (!slot.isFilled()) continue;
                const int binX = slot.bin.value().center().x;
                
                bool isUnique = true;
                
                // Check if this X is close to any existing unique X
                for (int uniqueX : uniqueXs) {
                    if (std::abs(binX - uniqueX) <= tolerance) {
                        isUnique = false;
                        break; // Found a close match, not unique
                    }
                }
                
                // Only add if it's truly unique (not close to existing ones)
                if (isUnique) {
                    uniqueXs.push_back(binX);
                }
            }
            
            numColumns = uniqueXs.size();
            return *this;
        }
    } stats;

    //Essentially a vector of vectors of rows contaning slots 
    Rows slotRows;
};

inline std::ostream& operator<<(std::ostream& os, const Rack& rack) {
    // Assume slotRows is a Rows and each Slot may have a Bin
    // Find the max number of columns in any row
    size_t maxCols = 0;
    for (const auto& row : rack.slotRows) {
        if (row.size() > maxCols) maxCols = row.size();
    }

    // Print top border
    os << "+";
    for (size_t col = 0; col < maxCols; ++col)
        os << "-----+";
    os << "\n";

    // Print each row
    for (const auto& row : rack.slotRows) {
        os << "|";
        for (size_t col = 0; col < maxCols; ++col) {
            if (col < row.size() && row[col].isFilled()) {
                // Print up to 3-digit bin number, right-aligned
                int bin_id = row[col].bin->marker.id;
                os << std::setw(3) << bin_id << " |";
            } else {
                os << "     |";
            }
        }
        os << "\n";
        // Print row separator
        os << "+";
        for (size_t col = 0; col < maxCols; ++col)
            os << "-----+";
        os << "\n";
    }
    return os;
}

// Add this operator<< for Rows
inline std::ostream& operator<<(std::ostream& os, const Row& row) {
    bool first = true;
    for (const auto& slot : row) {
        if (!first) {
            os << ", ";
        }
        first = false;
        
        if (slot.isFilled()) {
            os << slot.bin.value().marker.id;
        } else {
            os << "null";
        }
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ColumnCrosscut& crossCut) {
    for (size_t i = 0; i < crossCut.size(); ++i) {
        const Slot& slot = crossCut[i].get();
        if (slot.bin.has_value()) {
            std::cout << slot.bin.value().marker.id;
        } else {
            std::cout << "empty";
        }
        if (i != crossCut.size() - 1) std::cout << ",";
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Rows& rows) {
    for (const auto& row : rows) {
        os << row << std::endl; 
    }
    return os;
}

#endif
