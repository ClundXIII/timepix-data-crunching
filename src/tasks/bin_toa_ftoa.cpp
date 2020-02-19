#include "../tasks.hh"
#include "../classes/pixelhit.hh"
#include "../util/split.hh"

#include <vector>
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

using namespace PDC;

const long  toa_clck =  40;
const long ftoa_clck = 640;

const long clck_ratio = ftoa_clck / toa_clck;

int PDC::bin_toa_ftoa(colMapping thisColMapping, std::string input,
        std::string output, long zero, long range, bool maskToA) {

    if (maskToA) {
        zero = zero & 0b1111111;
        range = range & 0b1111111;
    }

    std::cout << "ftoa_clck / toa_clck: " << clck_ratio << std::endl;

    std::ifstream file(input);

    int crash_cout = 0;
    int lineCount = 0;

    std::vector<long> ftoa_time_bins(clck_ratio*range*2,0);

    long ftoa_min = clck_ratio*(zero-range);

    while (!file.eof()) {
        std::string line;
        std::getline(file, line);

        auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

        long ToA = -1;
        long fToA = -1;

        try {
            ToA  = std::stol(row.at(thisColMapping.toa));
            fToA = std::stoi(row.at(thisColMapping.ftoa));
        } catch(...) {
            std::cout << "skipping line \"" <<  line << "\"" << std::endl;
            crash_cout ++;
            continue;
        }

        if (maskToA) {
            ToA = ToA & 0b1111111;
        }

        long arrayPos = ( (ToA*clck_ratio-fToA) ) - ftoa_min;
        if ( arrayPos >= 0 && arrayPos < ftoa_time_bins.size()) {
            ftoa_time_bins.at(arrayPos)++;
        }
    }

    std::ofstream histOut(output, std::ofstream::out | std::ofstream::trunc);

    const long vSizeH = ftoa_time_bins.size()/2;
    for (long i=0; i<ftoa_time_bins.size(); i++) {
        if (ftoa_time_bins.at(i) != 0) {
            //std::cout << i << " " << ftoa_time_bins.at(i) << std::endl;
            histOut << (i-vSizeH) << " " << ftoa_time_bins.at(i) << std::endl;
        }
    }

    histOut.close();

    return 0;
}
