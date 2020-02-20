#include "../tasks.hh"
#include "../util/split.hh"
#include "../util/tot_keV_conversion.hh"

#include <vector>
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

using namespace PDC;

const long  toa_clck =  40;
const long ftoa_clck = 640;

const long clck_ratio = ftoa_clck / toa_clck;

int PDC::apply_calibration(colMapping thisColMapping, std::string input,
        std::string output, std::string calibrationFileName, bool forceOverwrite,
        bool maskToA) {

    if (!forceOverwrite && fs::exists(output)) {
        std::cout << "Output file already exists, use -f to force overwrite." << std::endl;
        return 1;
    }

    std::ofstream outputF(output, std::ofstream::out | std::ofstream::trunc);

    std::vector<calibrationParameter> allCalibrationParameter;
    allCalibrationParameter.reserve(256*256);

    int crash_count = 0;

    std::ifstream calibrationFile(calibrationFileName);

    while (!calibrationFile.eof() && crash_count < 5) {
        std::string line;
        std::getline(calibrationFile, line);

        try {
            auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

            double a = std::stod(row.at(0));
            double b = std::stod(row.at(1));
            double c = std::stod(row.at(2));
            double t = std::stod(row.at(3));

            allCalibrationParameter.push_back({a, b, c, t});
        } catch(...) {
            std::cout << "skipping line \"" <<  line << "\"" << std::endl;
            crash_count ++;
            continue;
        }
    }
    calibrationFile.close();

    std::ifstream inputF(input);

    crash_count = 0;

    outputF << "#matIdx energy time overflow" << std::endl;
    outputF << "#[1] [keV] [1.5*ns] [bool]" << std::endl;
    while (!inputF.eof() && crash_count < 5) {
        std::string line;
        std::getline(inputF, line);

        auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

        long ToT    = -1;
        long matIdx = -1;
        long ToA    = -1;
        long FToA   = -1;
        int  overflow = -1;

        try {
            ToT = std::stol(row.at(thisColMapping.tot));
            matIdx = std::stol(row.at(thisColMapping.matIdx));
            if (thisColMapping.toa >= 0) ToA = std::stol(row.at(thisColMapping.toa));
            if (thisColMapping.ftoa >= 0) FToA = std::stol(row.at(thisColMapping.ftoa));
            if (thisColMapping.overflow >= 0) overflow = std::stol(row.at(thisColMapping.overflow));
        } catch(...) {
            std::cout << "skipping line \"" <<  line << "\"" << std::endl;
            crash_count ++;
            continue;
        }

        float energy = totTokeV(ToT, allCalibrationParameter.at(matIdx));

        if (maskToA) {
            ToA = ToA & 0b1111111;
        }
        long time = ( (ToA*clck_ratio-FToA) );

        outputF << matIdx << " " << energy << " " << time << " " << overflow << std::endl;
    }

    inputF.close();
    outputF.close();

    return 0;
}
