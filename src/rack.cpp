//2023 qcarver@gmail.com MIT license 

#include "rack.h"
#include <iostream>
#include <ranges>
#include "group.h"

using namespace aruco;

template <std::ranges::input_range Range>
requires std::convertible_to<std::ranges::range_value_t<Range>, Slot>
Rack::Rack(Range&& r)
    : slots(std::forward<Range>(r).begin(), std::forward<Range>(r).end()) {
    std::cout << "Rack constructed from Slot range\n";

    // Calculate stats from the slots
    stats(slots); 
    // verbose output of stats
    std::cout << stats << std::endl;
}

/*Rack::Rack(const std::vector<aruco::Marker> & Markers)
{
    for (const aruco::Marker& binMarker : Markers) {
        std::cout << "looking binMarker column with X coord " << binMarker.getCenter().x << std::endl;
        addBinToGroups<Column>(Bin(binMarker), columns);

        std::cout << "looking binMarker row with Y coord " << binMarker.getCenter().y << std::endl;
        addBinToGroups<Row>(Bin(binMarker), rows);
    }
}*/

// Constructor that accepts a range of Bins
template <std::ranges::input_range Range>
    Rack(Range&& r) : bins(std::forward<Range>(r).begin(), std::forward<Range>(r).end()) {
        std::cout << "Rack constructed from Bin range\n";
}

template <RowOrColumn GroupType>
void Rack::addBinToGroups(const Bin bin, std::vector<GroupType>& groups) {
    bool added_to_existing = false;

    for (auto& group : groups) {
        if (binMarker < group) continue;
        else if (binMarker == group) {
            std::cout << "adding bin to existing " << groupTypeName<GroupType>()
                      << " spanning " << group.getRangeMin() << "<-- "
                      << group.get_position(binMarker)
                      << " -->" << group.getRangeMax() << std::endl;
            added_to_existing = group.addBin(binMarker);
            break;
        }
    }
    if (!added_to_existing) {
        auto it = groups.begin();
        for (; it != groups.end(); ++it) {
            if constexpr (std::is_same<GroupType, Column>::value) {
                if (binMarker.getCenter().x < it->getAvg()) {
                    break;
                }
            } else if constexpr (std::is_same<GroupType, Row>::value) {
                if (binMarker.getCenter().y < it->getAvg()) {
                    break;
                }
            }
        }
        groups.insert(it, GroupType(binMarker));
    }
}




int Rack::findBinRow(int id) const
{
    std::cout << "looking for bin " << id << "'s row" << std::endl;
    for (Row row : rows)
    {
        if (row.containsBin(id))
        {
            std::cout << "Bin found in row which has Y coord avg " << row.getAvg() << std::endl;
            return row.getAvg(); 
        } 
    }

    //return a false value if not found
    return 0;
}

int Rack::findBinColumn(int id) const
{
    std::cout << "looking for bin " << id << "'s column" << std::endl;
    for (Column column : columns)
    {
        if (column.containsBin(id))
        {
            std::cout << "Bin found in column which has X coord avg " << column.getAvg() << std::endl;
            return column.getAvg();
        }
    }

    //return a false value if not found
    return 0;
}

Rack::~Rack()
{
    for (Column column : columns)
    {
        //std::cout << "Avg: " << colum.getAvg() <<  column << std::endl;
        //any cleanup?
    }

    for (Row row : rows)
    {
        //std::cout << "Avg: " << row.getAvg() <<  row << std::endl;
        //any cleanup?
    } 

    //std::cout << "num columns before exit: " << Group::count << ", num bins before exit: " << Bin::count << std::endl;
    //std::cout << "num row before exit: " << Group::count << ", num bins before exit: " << Bin::count << std::endl;
}

std::ostream& operator<<(std::ostream& str, const Group& group)
{
    str << "{";

    uint first = 0;

    for (Marker binMarker : group.binMarkers )
    {
        str << (first++?", ":"") << static_cast<uint>(group.get_position(binMarker));
    }
    str << "}" << std::endl;

    return str;
}

bool operator< (const aruco::Marker & binMarker, const Group & group)
{
    float group_low_range = group.getRangeMin();
    uint bin_position = group.get_position(binMarker);

    //std::cout    << "binMarker's position of " << bin_position <<  ( bin_position < bin_position ? " < ":" ≮ ")
      //      << " column's low range " << group_low_range << std::endl;

    return  bin_position < group_low_range ;
}

bool operator> (const aruco::Marker & binMarker, const Group & group)
{
    float group_hi_range = group.getRangeMax();
    uint bin_position = group.get_position(binMarker);

    //std::cout    << "binMarker's value " << bin_position <<  (bin_position > group_hi_range ? " > ":"≯ ")
       //      << " column's high range" << group_hi_range << std::endl;

    return  bin_position > group_hi_range ;
}

bool operator== (const aruco::Marker & binMarker, const Group & group)
{
    return  ((!(binMarker<group))&&(!(binMarker>group))) ;
}
