//2023 qcarver@gmail.com MIT license 

#include "rack.h"
#include <iostream>
#include "group.h"

using namespace aruco;

Rack::Rack(const std::vector<aruco::Marker> & Markers)
{
//  Place the id of each binMarker in a column and a row
    for (aruco::Marker binMarker : Markers)
    {
        bool added_to_existing = false;

        std::cout << "looking binMarker column with X coord " << binMarker.getCenter().x << std::endl;
        
        for (Column& column : columns)
        {
            if (binMarker < column)  continue;

            else if (binMarker == column){
                std::cout << "adding bin to existing column spanning " << column.getRangeMin() << "<-- " << binMarker.getCenter().x << " -->" << column.getRangeMax() << std::endl;
                added_to_existing = column.addBin(binMarker);
                break;
            }
        }
        if (!added_to_existing) columns.push_back(Column(binMarker));

        added_to_existing = false;

        std::cout << "looking binMarker row with Y coord " << binMarker.getCenter().y << std::endl;

        for (Row& row : rows)
        {
            if (binMarker < row)  continue;

            else if (binMarker == row){
                std::cout << "adding bin to row " << row.getRangeMin() << "<-- " << binMarker.getCenter().y << " -->" << row.getRangeMax() << std::endl;
                added_to_existing = row.addBin(binMarker);
                break;
            }
        }
        if (!added_to_existing) rows.push_back(Row(binMarker));

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
