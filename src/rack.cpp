//2023 qcarver@gmail.com MIT license 

#include "rack.h"
#include <iostream>
#include <iomanip>
#include <ranges>


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
bool Rack::buildGridFromExistingBins(const Row& slots) {
    // Define the tolerance as half of the average bin height
    float tolerance = stats.avgBinHeight / 2.0f;

    // Step 1: Ingest the slots into the new vector of vectors
    for (const auto& slot : slots) {
        bool added = false;
        if (slotRows.empty()) {
            // If slotRows is empty, create the first row with the current slot
            slotRows.emplace_back(Row{slot});
            continue;
        }
        for (auto& row : slotRows) {
            /* Check if the slot's CenterY is within tolerance of the row's first slot*/ float
            first_bins_vertical_center = row.front().bin.value().center().y,
            this_bins_vertical_center = slot.bin.value().center().y,
            vertical_offset_from_beginning_of_row = this_bins_vertical_center - first_bins_vertical_center;

            if (std::abs(vertical_offset_from_beginning_of_row) <= tolerance) {
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
            slotRows.emplace_back(Row{slot});
        }
    }
    // Print each Row using std::ostream << row
    for (const auto& row : slotRows) {
        std::cout << "[" << (&row - &slotRows[0]) << "] " << row << std::endl;
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
        [](const Row& a, const Row& b) {
            if (a.empty() || b.empty()) return false;
            return a.front().bin.value().center().y < b.front().bin.value().center().y;
        }
    );

    std::cout << "sorted row order hi to low:" << std::endl;
    std::cout << slotRows;

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
            ColumnCrosscut crossCut = getColumnCrossCut(column);
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
 * Returns slots at the same column index across all rows.
 * Using reference_wrapper to store references in vector.
 */
ColumnCrosscut Rack::getColumnCrossCut(size_t columnIndex) const {
    ColumnCrosscut crossCut;
    
    for (const auto& row : slotRows) {
        if (columnIndex < row.size()) {
            crossCut.push_back(std::cref(row[columnIndex]));
        }
    }
    
    std::cout << "crossCut for column " << columnIndex << ": " << crossCut << std::endl;    
    return crossCut;
}

/**
 * Returns the minimum CenterX value in a cross-cut of slots.
 * Updated to work with reference_wrapper.
 */
uint16_t Rack::getMinXInCrossCut(const ColumnCrosscut& crossCut, size_t column) const {
    uint16_t minX = std::numeric_limits<uint16_t>::max();
    
    for (const auto& slotRef : crossCut) {
        const Slot& slot = slotRef.get(); // Get the actual reference
        if (slot.isFilled()) {
            uint16_t x = slot.bin.value().center().x;
            if (x < minX) {
                minX = x;
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
            if (slot.bin.has_value() && slot.bin.value().marker.id == bin_id) {
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
            if (slot.bin.has_value() && slot.bin.value().marker.id == bin_id) {
                return col;
            }
        }
    }
    return 0;
}

Rack::~Rack()
{
}