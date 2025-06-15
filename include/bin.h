//2023 qcarver@gmail.com MIT license 

#include <aruco.h>
#include <optional>
#pragma once

class Bin{
    private:
    Bin() = delete; //Bins must be labeled with a Marker
    public:
    Bin(const aruco::Marker _marker):marker(_marker) {}; 
    Bin(const Bin& bin) : marker(bin.marker) {} // Copy constructor
    Bin(Bin&& other) noexcept; // Move constructor declaration
    uint16_t height() const; // Height in pixels
    uint16_t width() const;  // Width in pixels

    struct Center {  // sadface, can't just return anon {uint16_t x,y}
        uint16_t x;  //     meh... I made this instead. 
        uint16_t y;  //     I kinda miss Python right now.
    };
    Center center() const; // Center in pixels, returns anon {x, y} struct

    //Add more fields?, Then update '=' copy/move constructors
    const aruco::Marker marker;
};

class Slot {
public:
    std::optional<Bin> bin; // Slot may or may contain a Bin

    Slot() : bin(std::nullopt) {} // Empty slot
    Slot(const Bin& b) : bin(b) {} // Filled slot

    void slideOut() {
        bin.reset();
    }

    Bin& slideIn(Bin& b) {
        bin = b;
        return *bin;
    }

    bool isFilled() const { return bin.has_value(); }
};