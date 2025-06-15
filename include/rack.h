#ifndef QC_RACK_H
#define QC_RACK_H
#include <aruco.h>
#include "group.h"
#include <concepts>

template <typename T>
concept RowOrColumn = std::is_base_of<Group, T>::value && (std::is_same_v<T, Column> || std::is_same_v<T, Row>);

class Rack{
 public:
    //Rack(const std::vector<Bin> & bins);

    // Constructor that accepts any input range of Bin
    template <std::ranges::input_range Range>
      requires std::convertible_to<std::ranges::range_value_t<Range>, Bin>
         Rack(Range&& r);

    int findBinRow(int id) const;
    int findBinColumn(int id) const;
    ~Rack();

 private:
   // vector of slots where bins can go
   std::vector<Slot> slots;

   struct Stats {
      uint16_t avgBinHeight = 0.0;
      uint16_t avgBinWidth = 0.0;
      const Bin* highestBin = nullptr;   // lowest center().y
      const Bin* lowestBin = nullptr;    // highest center().y
      const Bin* leftmostBin = nullptr;  // lowest center().x
      const Bin* rightmostBin = nullptr; // highest center().x

      template <typename Range>
      auto operator()(const Range& r) {
         size_t count = 0;

         for (const Slot& slot : r) {
            if (!slot.isFilled()) continue;
            ++count;
            const Bin& bin = slot.bin.value();
            // Running average update
            avgBinHeight += (bin.height() - avgBinHeight) / count;
            avgBinWidth += (bin.width() - avgBinWidth) / count;
            // Update min/max's
            if (!highestBin || bin.center().y < highestBin->center().y)
               highestBin = &bin;
            if (!lowestBin || bin.center().y > lowestBin->center().y)
               lowestBin = &bin;
            if (!leftmostBin || bin.center().x < leftmostBin->center().x)
               leftmostBin = &bin;
            if (!rightmostBin || bin.center().x > rightmostBin->center().x)
               rightmostBin = &bin;
         }

         return *this;
      }
   }stats;

   friend std::ostream& operator<<(std::ostream& os, const Stats& stats) {
      os << "Average Bin Height: " << stats.avgBinHeight << "\n"
         << "Average Bin Width: " << stats.avgBinWidth << "\n"
         << "Highest Bin Center Y: " << (stats.highestBin ? stats.highestBin->center().y : 0) << "\n"
         << "Lowest Bin Center Y: " << (stats.lowestBin ? stats.lowestBin->center().y : 0) << "\n"
         << "Leftmost Bin Center X: " << (stats.leftmostBin ? stats.leftmostBin->center().x : 0) << "\n"
         << "Rightmost Bin Center X: " << (stats.rightmostBin ? stats.rightmostBin->center().x : 0) << "\n";
      return os;
   }
   //legacy
   //used on construction
   template <RowOrColumn GroupType>
   void addBinToGroups(const Bin, std::vector<GroupType>&);
    

    //Max vectors of columns and rows and their avg
    std::vector<Column> columns;
    std::vector<Row> rows;
};


std::ostream& operator<<(std::ostream& str, const Group& group);

bool operator< (const Bin& bin, const Group & group);

bool operator> (const Bin& bin, const Group & group);

bool operator== (const Bin& bin, const Group & group);

#endif
