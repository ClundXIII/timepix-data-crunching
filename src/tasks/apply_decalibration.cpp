#include "../tasks.hh"
#include "../util/split.hh"
#include "../util/tot_keV_conversion.hh"

#include <vector>
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

using namespace PDC;

int PDC::apply_decalibration(colMapping thisColMapping, std::string input,
        std::string output, calibrationParameter defaultCalibrationParameter,
        bool forceOverwrite, float zero, float range, float binwidth) {

    if (range == 0) {
        range = 100;
    }

    std::ifstream file(input);

    if (!forceOverwrite && fs::exists(output)) {
        std::cout << "Output file already exists, use -f to force overwrite." << std::endl;
        return 1;
    }

    int crash_count = 0;

    int binCount = ((float) range) / binwidth;

    std::vector<int> energyBins(binCount, 0);

    while (!file.eof() && crash_count < 30) {
        std::string line;
        std::getline(file, line);

        auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

        double energy = -1;

        try {
            energy = std::stod(row.at(thisColMapping.calEnergy));
        } catch(...) {
            std::cout << "skipping line \"" <<  line << "\"" << std::endl;
            crash_count ++;
            continue;
        }

        float tot = keVTotot(energy, defaultCalibrationParameter);
        int binPos = ((tot+binwidth-zero)/binwidth);
        if (binPos >= 0 && binPos < binCount) {
            energyBins.at(binPos) ++;
        }
    }

    std::ofstream binOut(output, std::ofstream::out | std::ofstream::trunc);

    for (int i=0; i<binCount; i++) {
        binOut << (zero + i*binwidth+binwidth/2) << " " << energyBins.at(i) << std::endl;
    }

    binOut.close();

    return 0;
}
