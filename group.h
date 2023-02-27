//2023 qcarver@gmail.com MIT license 

#include "bin.h"
#include <iostream>
#include <string>

#ifndef QC_GROUP_H
#define QC_GROUP_H
class Group{
    private:
        Group():wiggle(0.0f){}

    public:
        static size_t count;
        virtual uint get_position(const Bin & bin) const = 0;

    protected:
        std::vector<Bin *> vpBins;
        uint sum = 0;
        const float wiggle;// = 0.12f;

    Group(const float _wiggle) : wiggle(_wiggle)
    {
        //cout << "There are now " << ++Group::count << " Groups." << endl;
    }

    ~Group()
    {
        //for (Bin * pBin : vpBins) delete pBin;

        //cout << "There are now "<< --Group::count <<" Groups." << endl;
        
        //cout << "There are " << (Group::count ? "still Groups to destruct!" : "no more Groups" ) <<endl;;
    }

    public: 
    float getAvg()
    {
        return (vpBins.size())?sum/vpBins.size():0;
    }

    float getRangeMin()
    {
        return getAvg()*(1-wiggle);
    }

    float getRangeMax()
    {
        return getAvg()*(1+wiggle);
    }

    bool addBin(Bin * pBin)
    {
        vpBins.push_back(pBin);
        //cout << "num bins in column is now " << vpBins.size() << endl;
        Bin & bin = *pBin;
        sum += get_position(bin);
        //cout << "Added bin w/ value: "<< pBin->marker.getCenter().x << " to column. Column avg is now: " << sum << "/" << vpBins.size() << " = " << sum/vpBins.size() << endl;

        return true;
    }

    bool isEmpty()
    {
        return vpBins.size();
    }

    friend std::ostream& operator<<(std::ostream& str, const Group& group);
};

class Column:public Group
{
    public:
    Column() : Group(0.13f){}

    ~Column()
    {
        //for (Bin * pBin : vpBins) delete pBin;
    }

    virtual uint get_position(const Bin & bin) const
    {
        return bin.marker.getCenter().x;
    }
};

class Row:public Group
{
    public:
    //Rows get a little more wiggle b/c there are fewer of them
    Row() : Group(0.095f){}

    virtual uint get_position(const Bin & bin) const
    {
        return bin.marker.getCenter().y;
    }
};

size_t Group::count = 0;
#endif