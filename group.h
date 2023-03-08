//2023 qcarver@gmail.com MIT license 
#include <aruco.h>
#include <iostream>
#include <string>

#ifndef QC_GROUP_H
#define QC_GROUP_H

#define COLUMN_WIGGLE 0.13f
#define ROW_WIGGLE 0.095f

//Pure Virtual Base class for Columns and Rows which are ... groups of things
class Group{
    private:
        Group():wiggle(0.0f){}
        Group(const aruco::Marker & binMarker):wiggle(0.0f){}
    public:
    static size_t count;
    virtual uint get_position(const aruco::Marker & binMarker) const = 0;

    protected:
    std::vector<int> binMarkers;
    uint sum = 0;
    const float wiggle;// = 0.12f;

    Group(const float _wiggle) : wiggle(_wiggle){};

    public: 
    float getAvg() const;

    float getRangeMin() const;

    float getRangeMax() const;

    bool addBin(const aruco::Marker & binMarker);

    bool containsBin(int id) const;

    bool isEmpty() const;

    friend std::ostream& operator<<(std::ostream& str, const Group& group);
};

class Column:public Group
{
    private:
    Column() : Group(COLUMN_WIGGLE){};

    public:
    Column(aruco::Marker & binMarker):Group(COLUMN_WIGGLE){};
    uint get_position(const aruco::Marker & binMarker) const override;
};

class Row:public Group
{
    private:
    Row() : Group(ROW_WIGGLE){};

    public:
    //Rows get a little more wiggle b/c there are fewer of them
    Row(aruco::Marker & binMarker) : Group(ROW_WIGGLE){};

    virtual uint get_position(const aruco::Marker & binMarker) const override;
};



#endif
