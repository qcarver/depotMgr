//2023 qcarver@gmail.com MIT license 

#include "bin.h"
#include "argc_argv.hpp"
extern ArgcArgvInput aai;

Bin::Bin(Bin&& other) noexcept
    : marker(std::move(other.marker))
{
    // Adding other members?
}

uint16_t Bin::height() const {
    // marker.getRadius() returns the radius in pixels
    return marker.getRadius() * 2 * aai.rowToMarkerHeightRatio; 
}
uint16_t Bin::width() const {   
    return marker.getRadius() * 2 * aai.colToMarkerWidthRatio; // Assuming marker.getRadius() returns the radius in pixels
}

Bin::Center Bin::center() const {
    return Bin::Center{
        static_cast<uint16_t>(marker.getCenter().x),
        static_cast<uint16_t>(marker.getCenter().y)
    };
}

//Slot
void Slot::slideOut() {
    bin.reset();
}

Bin& Slot::slideIn(Bin& _bin) {
    bin = _bin;
    return *bin;
}
