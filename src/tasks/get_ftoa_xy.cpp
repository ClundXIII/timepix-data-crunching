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

int PDC::get_ftoa_xy(colMapping thisColMapping, std::string input,
        std::string output, long zero, long range, bool maskToA) {

    if (maskToA) {
        zero = zero & 0b1111111;
        range = range & 0b1111111;
    }

    std::cout << "ftoa_clck / toa_clck: " << clck_ratio << std::endl;

    std::ifstream file(input);

    int crash_cout = 0;

    std::vector<long> ftoa_time(256*256,0);

    while (!file.eof()) {
        std::string line;
        std::getline(file, line);

        auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

        long ToA = -1;
        long fToA = -1;
        int matIdx = -1;

        try {
            ToA    = std::stol(row.at(thisColMapping.toa));
            fToA   = std::stoi(row.at(thisColMapping.ftoa));
            matIdx = std::stoi(row.at(thisColMapping.matIdx));
        } catch(...) {
            std::cout << "skipping line \"" <<  line << "\"" << std::endl;
            crash_cout ++;
            continue;
        }

        if (maskToA) {
            ToA = ToA & 0b1111111;
        }

        if (ToA > (zero-range) && ToA < (zero+range)) {
            long ftoaVal = ( (ToA*clck_ratio-fToA) );
            ftoa_time.at(matIdx) = ftoaVal-clck_ratio*zero;
        }
    }

    std::ofstream mapOut(output, std::ofstream::out | std::ofstream::trunc);

    for (unsigned int i=0; i<(256); i++) {
        for (unsigned int j=0; j<(256); j++) {
            mapOut << i << " " << j << " " << ftoa_time.at(i*256+j) << std::endl;
        }
    }

    mapOut.close();

    return 0;
}
