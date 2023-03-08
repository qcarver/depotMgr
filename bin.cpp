//2023 qcarver@gmail.com MIT license 

#include "bin.h"

Bin::Bin(aruco::Marker & _marker) : marker(_marker){}

Bin::Bin(const Bin & bin) : marker(bin.marker){}