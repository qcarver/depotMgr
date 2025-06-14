#include <iostream>
#include <string>
#include <getopt.h>
#include <cstdlib>
#include <cstring>

struct ArgcArgvInput {
    int bin_id = 0;
    std::string config_filename; // an aruco .yml
    std::string camera_filename; // -c arg
    float marker_size = -1.0f;
    std::string dictionary = "ALL_DICTS";
    std::string image_filename= "capture.jpg";
    bool verbose = false;
    float colToMarkerWidthRatio = 3.0f;   // New parameter
    float rowToMarkerHeightRatio = 2.0f;  // New parameter
} aai;

void show_help(const char* progname) {
    std::cout << "Usage: " << progname << " [OPTIONS] [filename]\n\n"
              << "Options:\n"
              << "  -h, --help              Show this help message and exit\n"
              << "  -v, --verbose           Print parsed values after processing\n"
              << "  -d <DICT>               Aruco Fiducial aai.Dictionary (default: ALL_DICTS)\n"
              << "  -b <aai.BIN_ID>         Storage aai.bin_id ID of interest (required)\n"
              << "  -f <aai.CONFIG_filename>    Aruco configuration file (optional)\n"
              << "  -c <aai.CAMERA_filename>    Camera parameters (*.yml) (optional)\n"
              << "  -s <SIZE>               Aruco marker size in meters (default: -1)\n"
              << "  -W <RATIO>              Column-to-marker width ratio (default: 3)\n"
              << "  -H <RATIO>              Row-to-marker height ratio (default: 2)\n"
              << "\n"
              << "Positional arguments:\n"
              << "  aai.image_filename      Image aai.image_filename to analyze (default: capture.jpg)\n";
}

int get_args(int argc, char* argv[], ArgcArgvInput& aai) {
    int num_args_parsed = 0;
    aai = {};

    const char* const short_opts = "hd:b:f:c:s:vW:H:";
    const option long_opts[] = {
        {"help",     no_argument,       nullptr, 'h'},
        {"verbose",  no_argument,       nullptr, 'v'},
        {"dict",     required_argument, nullptr, 'd'},
        {"aai.bin_id", required_argument, nullptr, 'b'},
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
                aai.dictionary = optarg;
                break;
            case 'b':
                num_args_parsed++;
                aai.bin_id = std::atoi(optarg);
                break;
            case 'f':
                num_args_parsed++;
                aai.config_filename = optarg;
                break;
            case 'c':
                num_args_parsed++;
                aai.camera_filename = optarg;
                break;
            case 's':
                num_args_parsed++;
                aai.marker_size = std::stof(optarg);
                break;
            case 'W':
                num_args_parsed++;
                aai.colToMarkerWidthRatio = std::stof(optarg);
                break;
            case 'H':
                num_args_parsed++;
                aai.rowToMarkerHeightRatio = std::stof(optarg);
                break;
            case 'v':
                num_args_parsed++;
                aai.verbose = true;
                break;
            default:
                num_args_parsed++;
                show_help(argv[0]);
                return 1;
        }
    }

    // Remaining argument is aai.image_file nameif present
    if (optind < argc) {
        num_args_parsed++;
        aai.image_filename = argv[optind];
    }

    // Check required argument
    if (!aai.bin_id) {
        std::cerr << "Error: -b <aai.BIN_ID> is required.\n";
        show_help(argv[0]);
        return 1;
    }

    // Show parsed values (you can replace this with actual logic)
    if (aai.verbose){
        std::cout << "Parsed values("<< num_args_parsed <<"):\n"
              << "  aai.Dictionary     : " << aai.dictionary << "\n"
              << "  Bin ID         : " << aai.bin_id << "\n"
              << "  Config File    : " << (aai.config_filename.empty() ? "(none)" : aai.config_filename) << "\n"
              << "  Camera File    : " << (aai.camera_filename.empty() ? "(none)" : aai.camera_filename) << "\n"
              << "  Marker Size    : " << (aai.marker_size < 0 ? "(none)" : std::to_string(aai.marker_size)) << "\n"
              << "  Col/Marker W Ratio : " << aai.colToMarkerWidthRatio << "\n"
              << "  Row/Marker H Ratio : " << aai.rowToMarkerHeightRatio << "\n"
              << "  Input Filename : " << aai.image_filename << "\n";
    }

    return num_args_parsed;
}