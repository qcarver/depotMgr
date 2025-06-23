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
    Bin& operator=(const Bin& other) {
        if (&other != this) {
            marker = other.marker;
            // Add more fields here if needed in the future
        }
        return *this;
    };

    struct Center {  // sadface, can't just return anon {uint16_t x,y}
        uint16_t x;  //     meh... I made this instead. 
        uint16_t y;  //     I kinda miss Python right now.
    };
    Center center() const; // Center in pixels, returns anon {x, y} struct

    //Add more fields?, Then update '=' copy/move constructors
    aruco::Marker marker;
};

Bin& assignBin(Bin& lhs, const Bin& rhs) {
    if (&lhs != &rhs) {
        const_cast<aruco::Marker&>(lhs.marker) = rhs.marker;
        // Add more fields here if needed in the future
    }
    return lhs;
};

class Slot {
public:
    std::optional<Bin> bin; // Slot may or may contain a Bin

    Slot() : bin(std::nullopt) {} // Empty slot
    Slot(const Bin& b) : bin(b) {} // Filled slot

    void slideOut();
    Bin& slideIn(Bin&);

    bool isFilled() const { return bin.has_value(); }
    bool hasBin() const { return isFilled(); } // Alias for clarity
    bool isEmpty() const { return !bin.has_value(); }
};