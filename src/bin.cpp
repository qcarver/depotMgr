//2023 qcarver@gmail.com MIT license 

#include "bin.h"
Bin::Bin(Bin&& other) noexcept
    : marker(std::move(other.marker))
{
    // Adding other members?
}


//Slot
void Slot::slideOut() {
    bin.reset();
}

Bin& Slot::slideIn(Bin& _bin) {
    bin = _bin;
    return *bin;
}
