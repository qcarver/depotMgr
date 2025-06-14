//2023 qcarver@gmail.com MIT license 
#include <iostream>
#include <string>
#include "bin.h"

#pragma once
#define COLUMN_WIGGLE 0.13f
#define ROW_WIGGLE 0.095f

//Pure Virtual Base class for Columns and Rows which are ... groups of things
class Group{
    private:
        Group():wiggle(0.0f){}
    public:
    static size_t count;
    virtual uint get_position(const Bin& bin) const = 0;

    protected:
    std::vector<int> bins;
    uint avg = 0;
    float wiggle;// = 0.12f;
    Group(const float _wiggle):wiggle(_wiggle){}
    Group(const Bin& bin, uint position, const float _wiggle):wiggle(_wiggle){addBin(bin, position);}

    public: 
    float getAvg() const;

    float getRangeMin() const;

    float getRangeMax() const;

    bool addBin(const Bin & bin, uint position);

    bool containsBin(int id) const;

    bool isEmpty() const;

    friend std::ostream& operator<<(std::ostream& str, const Group& group);
};

class Column:public Group
{
    private:
    Column() : Group(COLUMN_WIGGLE){};

    public:
    Column(const Bin& bin):Group(bin, bin.marker.getCenter().x, COLUMN_WIGGLE){};
    uint get_position(const Bin& bin) const override;
    bool addBin(const Bin& bin);
};

class Row:public Group
{
    private:
    Row() : Group(ROW_WIGGLE){};

    public:
    //Rows get a little more wiggle b/c there are fewer of them
    Row(const Bin& bin):Group(bin, bin.marker.getCenter().y, ROW_WIGGLE){};
    virtual uint get_position(const Bin& bin) const override;
    bool addBin(const Bin& bin);
};

template<typename T>
const char* groupTypeName();


