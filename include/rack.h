#ifndef QC_RACK_H
#define QC_RACK_H
#include <aruco.h>
#include <concepts>
#include <bin.h>

class Rack{
 public:
   // B/c templates - this c'tor has to be defined in the header file. :/
   // Constructor that accepts any input std::range of Bins, organizes from there.
   template <std::ranges::input_range Range>
   requires std::convertible_to<std::ranges::range_value_t<Range>, Bin>
   Rack(Range&& r)
      : slots(std::forward<Range>(r).begin(), std::forward<Range>(r).end()) {
      std::cout << "Rack constructed from Slot range\n";

      // Calculate stats from the slots.. Eg: num bins, average bin width, height, etc.
      stats(slots); 
      
      // turns the range of slots into the member vector of vectors: slotRows
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
   
   // Splits vector of slots from construction into the member `slotRows`
   bool buildGridFromExistingBins(const std::vector<Slot>&);

   // Assumes vector already row grouped. Sorts them hi to low
   bool sortRowOrderByFirstBinInRow();

   // Looks where bins aren't and fills them with empty slots. returns true.
   bool backFillEmptySlots();

   // Returns Slot pointers at the same index (aka Column) per row of slotRows
   std::vector<const Slot*> getColumnCrossCut(size_t columnIndex) const;

   // Returns the minimum CenterX value in a cross-cut of slots
   uint16_t getMinXInCrossCut(std::vector<const Slot*> crossCut, size_t column) const;

   // Returns the number of bins in the longest row (tie okay!)
   size_t getMaxColumns() const;

   // vector of slots where bins can go
   std::vector<Slot> slots;

   struct Stats {
      uint16_t avgBinHeight = 0.0;
      uint16_t avgBinWidth = 0.0;
      size_t numBins = 0;

      template <typename Range>
      auto operator()(const Range& r) {
         for (const Slot& slot : r) {
            if (!slot.isFilled()) continue;
            ++numBins;
            const Bin& bin = slot.bin.value();
            // Running average update
            avgBinHeight += (bin.height() - avgBinHeight) / numBins;
            avgBinWidth += (bin.width() - avgBinWidth) / numBins;
         }
         return *this;
      }
   }stats;

   friend std::ostream& operator<<(std::ostream& os, const Stats& stats) {
      os << "Average Bin Height: " << stats.avgBinHeight << "\n"
         << "Average Bin Width: " << stats.avgBinWidth << "\n";
      return os;
   }
    
   //Essentially a vector of vectors of rows contaning slots 
   std::vector<std::vector<Slot>> slotRows;
};

#endif
