//2023 qcarver@gmail.com MIT license 

#include "rack.h"
#include <iostream>
#include <ranges>

using namespace aruco;

// Constructor that accepts any input std::range of Bins, organizes from there.
template <std::ranges::input_range Range>
requires std::convertible_to<std::ranges::range_value_t<Range>, Bin>
Rack::Rack(Range&& r)
    : slots(std::forward<Range>(r).begin(), std::forward<Range>(r).end()) {
    std::cout << "Rack constructed from Slot range\n";

    // Calculate stats from the slots.. Eg: num bins, average bin width, height, etc.
    stats(slots); 
    
    // turns the range of slots into the member vector of vectors: slotRows
    buildGridFromExistingBins(slots);

    // order the vectors of 'rows' from top to bottom
    sortRowOrderByFirstBinInRow();

    // if bins are pulled out, then back fill empty slots in the grid
    backFillEmptySlots();
}

/* Constructor that accepts a range of Bins
template <std::ranges::input_range Range>
    Rack(Range&& r) : bins(std::forward<Range>(r).begin(), std::forward<Range>(r).end()) {
        std::cout << "Rack constructed from Bin range\n";
}*/

/**
 * Builds the grid of slots from existing bins.
 * This function organizes the slots into rows based on their CenterY values,
 * ensuring that slots with similar CenterY values are grouped together.
 * It also sorts the slots within each row by their CenterX values.
 * @param slots A vector of Slot objects to be organized into rows.
 * @return Returns true if the grid was built successfully.
 * 
 * Private helper function called by the constructor.
 */
bool Rack::buildGridFromExistingBins(const std::vector<Slot>& slots) {
    // Define the tolerance as half of the average bin width
    float tolerance = stats.avgBinWidth / 2.0f;

    // Step 1: Ingest the slots into the new vector of vectors
    for (const auto& slot : slots) {
        bool added = false;
        for (auto& row : slotRows) {
            // Check if the slot's CenterY is within tolerance of the row's first slot
            if (!row.empty() && 
                std::abs(slot.bin.value().center().y - row.front().bin.value().center().y) <= tolerance) {
                // Insert slot into row in order of ascending bin.CenterX
                auto it = std::lower_bound(
                    row.begin(), row.end(), slot,
                    [](const Slot& a, const Slot& b) {
                        return a.bin.value().center().x < b.bin.value().center().x;
                    }
                );
                row.insert(it, slot);
                added = true;
                break;
            }
        }
        if (!added) {
            // No matching group found, create a new group
            slotRows.emplace_back(std::vector<Slot>{slot});
        }
    }

    return true;
}

/** 
 * Assumes 1st bin in each row is a typical CenterY value and sorts vectors high to low  
 * @returns true (assumes) the sorting was successful.
 * 
 * Private helper function called by the constructor.
 */

bool Rack::sortRowOrderByFirstBinInRow(){
    std::sort(slotRows.begin(), slotRows.end(),
        [](const std::vector<Slot>& a, const std::vector<Slot>& b) {
            if (a.empty() || b.empty()) return false;
            return a.front().bin.value().center().x < b.front().bin.value().center().x;
        }
    );
    return true;
}

/**
 * Fills empty slots in the grid by inserting new empty slots where necessary.
 * This function iterates through columns of the slot grid, looking for 
 * 'deviant' slots which exceed other slots in the columns' CenterX values
 * Once detected, new empty slots are inserted in the row before that deviant slot.
 * 
 * @return Returns true if empty slots were successfully filled.
 */
bool Rack::backFillEmptySlots() {
    // Ensure there are rows to work with
    if (slotRows.empty()) return false;

    // How far off can center deviate from slot to slot and still be in the same column? 
    float tolerance = stats.avgBinWidth / 2.0f;

    // Indexes get hosed on insertion. This keeps engagement till no more slots inserted
    bool slotInserted;
    do {
        slotInserted = false;
        // For each column (cross-cut)
        for (size_t column = 0; column < getMaxColumns(); ++column) {
            // Use getColumnCrossCut to get the cross-cut slots
            std::vector<const Slot*> crossCut = getColumnCrossCut(column);
            if (crossCut.empty()) continue;

            // Columns aren't aligned yet, should all align the the leftmost slot in the cross-cut
            uint16_t minCenterX = getMinXInCrossCut(crossCut, column);

            // For each slot in the cross-cut, check if it needs an empty slot inserted before it
            for (size_t row = 0; row < slotRows.size(); ++row) {
                if (column < numColumnsInRow(row)) {
                    const Slot& slot = slotRows[row][column];
                    if (slot.isEmpty()) continue; // Skip empty slots
                    if (slot.bin.value().center().x - tolerance > minCenterX) {
                        // Insert an empty slot before this slot in slotRows[row]
                        Slot emptySlot = slot;
                        slotRows[row].insert(slotRows[row].begin() + column, emptySlot);
                        slotInserted = true;
                        // After insertion, break to restart the process (since indices are now invalid)
                        break;
                    }
                }
            }
            if (slotInserted) break;
        }
    } while (slotInserted);

    // Final assertion: all vectors should have the same size
    size_t finalSize = slotRows.front().size();
    for (const auto& group : slotRows) {
        assert(group.size() == finalSize && "All slotRows vectors must have the same size after backFillEmptySlots");
    }

    return true;
}

/**
 * Returns the number of slots down all rows in a given column (index) 
 * Private helper function.
 */
std::vector<const Slot*> Rack::getColumnCrossCut(size_t column) const {
    std::vector<const Slot*> crossCut;
    for (const auto& row : slotRows) {
        if (column < row.size()) {
            crossCut.push_back(&row[column]);
        }
    }
    return crossCut;
}

/**
 * Return the minimum CenterX value in a cross-cut of slots.
 * crossCut is a vector of Slot pointers, 
 * column is the index of the column being checked. 
 * @returns (slot has atleast 1 bin)? minimum CenterX value found in cross-cut: max uint16_t
 * 
 * Private helper function used by backFillEmptySlots. 
 */
uint16_t Rack::getMinXInCrossCut(std::vector<const Slot*> crossCut, size_t column) const {
    uint16_t minX = std::numeric_limits<uint16_t>::max();
    for (const Slot* slotPtr : crossCut) {
        if (slotPtr && slotPtr->bin.has_value()) {
            uint16_t centerX = slotPtr->bin.value().center().x;
            if (centerX < minX) {
                minX = centerX;
            }
        }
    }
    return minX;
}

/** * Gets the maximum number of columns (slots) in any row of the rack.
 * @return The width of the rack.
 */
size_t Rack::getMaxColumns() const {
    size_t maxColumns = 0;
    for (const auto& row : slotRows) {
        if (row.size() > maxColumns) maxColumns = row.size();
    }
    return maxColumns;
}

/** Finds the row index of a bin by its ID.
 * @param bin_id The ID of the bin to find.
 * @return The row where the bin is located, or 0 if not found.
 */ 
size_t Rack::findBinRow(uint32_t bin_id) const {
    for (size_t row = 0; row < slotRows.size(); ++row) {
        for (const auto& slot : slotRows[row]) {
            if (slot.bin.has_value() && slot.bin.value().id == bin_id) {
                return row;
            }
        }
    }
    return 0;
}

/** gets the column number that the bin is in
 * @param bin_id The ID of the bin to find.
 * @return The column where the bin is located, or 0 if not found.
 */
size_t Rack::findBinColumn(uint32_t bin_id) const {
    for (size_t row = 0; row < slotRows.size(); ++row) {
        for (size_t col = 0; col < slotRows[row].size(); ++col) {
            const auto& slot = slotRows[row][col];
            if (slot.bin.has_value() && slot.bin.value().id == bin_id) {
                return col;
            }
        }
    }
    return 0;
}

Rack::~Rack()
{
}


