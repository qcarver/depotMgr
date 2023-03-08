#ifndef QC_RACK_H
#define QC_RACK_H
#include <aruco.h>
#include <group.h>



class Rack{
 public:
    Rack(const std::vector<aruco::Marker> & Markers);
    int findBinRow(int id) const;
    int findBinColumn(int id) const;
    ~Rack();
 private:
    //Max vectors of columns and rows and their avg
    std::vector<Column> columns;
    std::vector<Row> rows;
};

std::ostream& operator<<(std::ostream& str, const Group& group);

bool operator< (const aruco::Marker & binMarker, const Group & group);

bool operator> (const aruco::Marker & binMarker, const Group & group);

bool operator== (const aruco::Marker & binMarker, const Group & group);

#endif
