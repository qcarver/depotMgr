#ifndef QC_RACK_H
#define QC_RACK_H
#include <aruco.h>
#include <concepts>
#include <bin.h>

class Rack{
 public:
   //Rack(const std::vector<Bin> & bins);

   // Constructor that accepts any input range of Bin
   template <std::ranges::input_range Range>
     requires std::convertible_to<std::ranges::range_value_t<Range>, Bin>
        Rack(Range&& r);

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
