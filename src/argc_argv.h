#include <string>
#pragma once

namespace LaunchArgs {

struct Config {
    int bin_id = 0;
    std::string config_filename; // an aruco .yml
    std::string camera_filename; // -c arg
    float marker_size = -1.0f;
    std::string dictionary = "ALL_DICTS"; // wrt Aruco Dictionary of Markers
    std::string image_filename= "capture.jpg";
    bool verbose = false;
    float colToMarkerWidthRatio = 3.0f;   // New parameter
    float rowToMarkerHeightRatio = 2.0f;  // New parameter
};
void show_help(const char*);
int get_args(int, char*[], LaunchArgs::Config&);

} // namespace LaunchArgs

