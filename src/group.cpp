//2023 qcarver@gmail.com MIT license 

#include "group.h"
#include <iostream>
#include <string>

size_t Group::count = 0;

float Group::getAvg() const
{
    return (binMarkers.size())?sum/binMarkers.size():0;
}

float Group::getRangeMin() const
{
    return getAvg()*(1-wiggle);
}

float Group::getRangeMax() const
{
    return getAvg()*(1+wiggle);
}

bool Group::addBin(const aruco::Marker & binMarker)
{
    binMarkers.push_back(binMarker.id);
    //cout << "num binMarkers in column is now " << binMarkers.size() << std::endl;
    //sum helps keep an average of the horiz or vert position of the row or column
    sum += get_position(binMarker.id);
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

uint Column::get_position(const aruco::Marker & binMarker) const
{
    return binMarker.getCenter().x;
}

uint Row::get_position(const aruco::Marker & binMarker) const
{
    return binMarker.getCenter().y;
}


