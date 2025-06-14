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
    Bin(Bin&& other); // Move constructor declaration
    Bin& operator=(const Bin& other);

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