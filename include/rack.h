#ifndef QC_RACK_H
#define QC_RACK_H
#include <aruco.h>
#include "group.h"
#include <concepts>

template <typename T>
concept RowOrColumn = std::is_base_of<Group, T>::value && (std::is_same_v<T, Column> || std::is_same_v<T, Row>);

class Rack{
 public:
    Rack(const std::vector<aruco::Marker> & Markers);
    int findBinRow(int id) const;
    int findBinColumn(int id) const;
    ~Rack();

 private:
   //used on construction
   template <RowOrColumn GroupType>
   void addMarkerToGroups(const aruco::Marker& binMarker, std::vector<GroupType>& groups);
    
    //Max vectors of columns and rows and their avg
    std::vector<Column> columns;
    std::vector<Row> rows;
};


std::ostream& operator<<(std::ostream& str, const Group& group);

bool operator< (const aruco::Marker & binMarker, const Group & group);

bool operator> (const aruco::Marker & binMarker, const Group & group);

bool operator== (const aruco::Marker & binMarker, const Group & group);

#endif
