#include "../tasks.hh"
#include "../util/split.hh"
#include "../util/tot_keV_conversion.hh"

#include <vector>
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

using namespace PDC;

int apply_simple_calibration(colMapping thisColMapping, std::string input,
        std::string output, calibrationParameter, bool forceOverwrite) {

    std::ifstream file(input);

    if (!forceOverwrite && fs::exists(output)) {
        std::cout << "Output file already exists, use -f to force overwrite." << std::endl;
        return 1;
    }

    std::ofstream binOut(output, std::ofstream::out | std::ofstream::trunc);

    int crash_count = 0;

    while (!file.eof() && crash_count < 5) {
        std::string line;
        std::getline(file, line);

        auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

        long ToA = -1;

        try {
            ToA    = std::stol(row.at(thisColMapping.toa));
        } catch(...) {
            std::cout << "skipping line \"" <<  line << "\"" << std::endl;
            crash_count ++;
            continue;
        }

        binOut << std::endl;
    }

    binOut.close();


    return 0;
}
