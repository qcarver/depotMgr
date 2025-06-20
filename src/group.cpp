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

bool Group::addBin(const aruco::Marker & binMarker, uint position )
{
    binMarkers.push_back(binMarker.id);

    if (binMarkers.size() == 1) {
        avg = position; 
    } else {
        avg = (avg * (binMarkers.size() - 1) + position) / binMarkers.size();
    }
    return true;
}

bool Group::containsBin(int id) const
{
    bool found = false;
    for (int binMarker : binMarkers)
    {
        found |= binMarker == id;
    }
    return found;
}

bool Group::isEmpty() const
{
    return binMarkers.size();
}

template<>
const char* groupTypeName<Column>() { return "Column"; }

uint Column::get_position(const aruco::Marker & binMarker) const
{
    return binMarker.getCenter().x;
}

bool Column::addBin(const aruco::Marker & binMarker)
{
    return Group::addBin(binMarker, get_position(binMarker));
}

template<>
const char* groupTypeName<Row>() { return "Row"; }

uint Row::get_position(const aruco::Marker & binMarker) const
{
    return binMarker.getCenter().y;
}

bool Row::addBin(const aruco::Marker & binMarker)
{
    return Group::addBin(binMarker, get_position(binMarker));
}
