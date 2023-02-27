//2023 qcarver@gmail.com MIT license 

#include <aruco.h>
#include <marker.h>
#include <iostream>
#include <string>

#ifndef QC_BIN_H
#define QC_BIN_H
class Bin{
    private:
    public:
        static size_t count;
        const aruco::Marker & marker;

    Bin(aruco::Marker & _marker) : marker(_marker)
    {
        //cout << "There are now " << ++Bin::count << " Bins." << endl;
    }
    ~Bin() 
    {
        //cout << "There are now " << --Bin::count << " Bins." << endl;
    }
};

size_t Bin::count = 0;
#endif