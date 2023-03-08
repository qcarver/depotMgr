//2023 qcarver@gmail.com MIT license 

#include <aruco.h>

#ifndef QC_BIN_H
#define QC_BIN_H
class Bin{
    private:
    Bin();
    public:
    const aruco::Marker & marker;
    Bin(aruco::Marker & _marker);
    Bin(const Bin & bin);
};
#endif