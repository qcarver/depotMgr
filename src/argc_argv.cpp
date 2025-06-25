#include <iostream>
#include <getopt.h>
#include "argc_argv.h"
#include <cstdlib> // for std::atoi, std::stof

LaunchArgs::Config config; // Global config object

namespace LaunchArgs {
void show_help(const char* progname) {
    std::cout << "Usage: " << progname << " [OPTIONS] [filename]\n\n"
              << "Options:\n"
              << "  -h, --help              Show this help message and exit\n"
              << "  -v, --verbose           Print parsed values after processing\n"
              << "  -d <DICT>               Aruco Fiducial config.Dictionary (default: ALL_DICTS)\n"
              << "  -b <config.BIN_ID>         Storage config.bin_id ID of interest (required)\n"
              << "  -f <config.CONFIG_filename>    Aruco configuration file (optional)\n"
              << "  -c <config.CAMERA_filename>    Camera parameters (*.yml) (optional)\n"
              << "  -s <SIZE>               Aruco marker size in meters (default: -1)\n"
              << "  -W <RATIO>              Column-to-marker width ratio (default: 3)\n"
              << "  -H <RATIO>              Row-to-marker height ratio (default: 2)\n"
              << "\n"
              << "Positional arguments:\n"
              << "  config.image_filename      Image config.image_filename to analyze (default: capture.jpg)\n";
}

int get_args(int argc, char* argv[], LaunchArgs::Config& config) {
    int num_args_parsed = 0;

    const char* const short_opts = "hd:b:f:c:s:vW:H:";
    const option long_opts[] = {
        {"help",     no_argument,       nullptr, 'h'},
        {"verbose",  no_argument,       nullptr, 'v'},
        {"dict",     required_argument, nullptr, 'd'},
        {"config.bin_id", required_argument, nullptr, 'b'},
        {"file",     required_argument, nullptr, 'f'},
        {"camera",   required_argument, nullptr, 'c'},
        {"size",     required_argument, nullptr, 's'},
        {"colToMarkerWidthRatio", required_argument, nullptr, 'W'},
        {"rowToMarkerHeightRatio", required_argument, nullptr, 'H'},
        {nullptr,    0,                 nullptr,  0 }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
        switch (opt) {
            case 'h':
                num_args_parsed++;
                show_help(argv[0]);
                return 0;
            case 'd':
                num_args_parsed++;
                config.dictionary = optarg;
                break;
            case 'b':
                num_args_parsed++;
                config.bin_id = std::atoi(optarg);
                break;
            case 'f':
                num_args_parsed++;
                config.config_filename = optarg;
                break;
            case 'c':
                num_args_parsed++;
                config.camera_filename = optarg;
                break;
            case 's':
                num_args_parsed++;
                config.marker_size = std::stof(optarg);
                break;
            case 'W':
                num_args_parsed++;
                config.colToMarkerWidthRatio = std::stof(optarg);
                break;
            case 'H':
                num_args_parsed++;
                config.rowToMarkerHeightRatio = std::stof(optarg);
                break;
            case 'v':
                num_args_parsed++;
                config.verbose = true;
                break;
            default:
                num_args_parsed++;
                show_help(argv[0]);
                return 1;
        }
    }

    // Remaining argument is config.image_file nameif present
    if (optind < argc) {
        num_args_parsed++;
        config.image_filename = argv[optind];
    }

    // Check required argument
    if (!config.bin_id) {
        std::cerr << "Error: -b <config.BIN_ID> is required.\n";
        show_help(argv[0]);
        return 1;
    }

    // Show parsed values (you can replace this with actual logic)
    if (config.verbose){
        std::cout << "Parsed values("<< num_args_parsed <<"):\n"
              << "  config.Dictionary     : " << config.dictionary << "\n"
              << "  Bin ID         : " << config.bin_id << "\n"
              << "  Config File    : " << (config.config_filename.empty() ? "(none)" : config.config_filename) << "\n"
              << "  Camera File    : " << (config.camera_filename.empty() ? "(none)" : config.camera_filename) << "\n"
              << "  Marker Size    : " << (config.marker_size < 0 ? "(none)" : std::to_string(config.marker_size)) << "\n"
              << "  Col/Marker W Ratio : " << config.colToMarkerWidthRatio << "\n"
              << "  Row/Marker H Ratio : " << config.rowToMarkerHeightRatio << "\n"
              << "  Input Filename : " << config.image_filename << "\n";
    }

    return num_args_parsed;
}

} // namespace LaunchArgs