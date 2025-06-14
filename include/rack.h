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
