//2023 qcarver@gmail.com MIT license 

#include "group.h"
#include <iostream>
#include <string>

size_t Group::count = 0;

float Group::getAvg() const
{
    return avg; 
}

float Group::getRangeMin() const
{
    return avg*(1-wiggle);
}

float Group::getRangeMax() const
{
    return avg*(1+wiggle);
}

bool Group::addBin(const Bin& bin, uint position )
{
    bins.push_back(bin.marker.id);

    if (bins.size() == 1) {
        avg = position; 
    } else {
        avg = (avg * (bins.size() - 1) + position) / bins.size();
    }
    return true;
}

bool Group::containsBin(int id) const
{
    bool found = false;
    for (int bin : bins)
    {
        found |= bin == id;
    }
    return found;
}

bool Group::isEmpty() const
{
    return bins.size();
}

template<>
const char* groupTypeName<Column>() { return "Column"; }

uint Column::get_position(const Bin& bin) const
{
    return bin.marker.getCenter().x;
}

bool Column::addBin(const Bin& bin)
{
    return Group::addBin(bin, get_position(bin));
}

template<>
const char* groupTypeName<Row>() { return "Row"; }

uint Row::get_position(const Bin& bin) const
{
    return bin.marker.getCenter().y;
}

bool Row::addBin(const Bin& bin)
{
    return Group::addBin(bin, get_position(bin));
}
