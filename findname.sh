#!/bin/bash
IFS=$'\n'

# 
# HELP: Print usage and exit
# 
if [[ "$1" == "-h" || "$1" == "--help" || -z "$1" ]]; then
    echo ""
    echo "Usage: $(basename "$0") <search_term>"
    echo ""
    echo "Example: ./findname.sh relay"
    echo "   Scans inventory.xml for bins with name or item tags containing 'relay'"
    echo "    Case-insensitive partial matches are accepted."
    echo ""
    echo "Expected layout:"
    echo "  - This script should live in the same directory as inventory.xml"
    echo "  - Requires a config file: local_esp32_MAC_n_IP_dont_check_me_in.txt"
    echo ""
    echo "That file should define the following variables:"
    echo "  esp32cam_mac=\"3C:61:05:3F:79:0C\""
    echo "  esp32cam_ip=\"192.168.1.14\""
    echo ""
    exit 0
fi

#
# To use this script, create a file named:
#     local_esp32_MAC_n_IP_dont_check_me_in.txt
# in the same directory. It should contain lines like:
#
#     esp32cam_mac="3C:61:05:3F:79:0C"
#     esp32cam_ip="192.168.1.14"
#
# This file should be added to .gitignore to keep local config private.
# Load camera IP and MAC from external config file
#
config_file="local_esp32_MAC_n_IP_dont_check_me_in.txt"
if [ -f "$config_file" ]; then
    source "$config_file"
else
    echo "Missing $config_file. Please create it with esp32cam_mac and esp32cam_ip variables."
    exit 1
fi

# Get XML field name from script filename (e.g. findname.sh  name)
script_name=$(basename "$0")
target_field=$(echo "$script_name" | sed -E 's/^find([a-zA-Z]+)\.sh$/\1/' | tr '[:upper:]' '[:lower:]')

# Fallback if run via symlink or named just 'find'
if [ -z "$target_field" ]; then
    target_field="name"
fi

search_term="$1"

# Normalize input for case-insensitive search
search_term_lc=$(echo "$search_term" | tr '[:upper:]' '[:lower:]')

# Build XPaths for attribute and element content
bins_xpath="/inventory//*[@${target_field} and contains(translate(@${target_field}, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), '$search_term_lc')]/ancestor-or-self::bin/@id"
items_xpath="/inventory//${target_field}[contains(translate(text(), 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), '$search_term_lc')]/ancestor::bin/@id"

# DEBUG: Print out XPaths being used
# echo "Using XPath: $bins_xpath | $items_xpath"

# Try both
result=$(xmllint --xpath "$bins_xpath | $items_xpath" inventory.xml 2>/dev/null)

if [ $? -eq 0 ]; then
    echo "$result"
    #bin_id=$(echo "$result" | sed -n 's/.*id="\([0-9]*\)".*/\1/p')
    bin_id=$(echo "$result" | sed -n 's/.*[[:space:]]*id="\([0-9]*\)".*/\1/p')
    #bin_id=$(echo "$result" | awk -F'"' '{print $2}')
    echo "id is ${bin_id}"
else
    echo "No bin found for $target_field containing \"$search_term\" (case-insensitive)"
    exit 1
fi

# Request a picture from the ESP32-CAM
curl -s "${esp32cam_ip}/capture" --output capture.jpg
if [ $? -ne 0 ]; then
    echo "Couldn't connect to camera, using old picture"
fi

# Run bin classifier
./build/depotMgr capture.jpg -b "$bin_id"
