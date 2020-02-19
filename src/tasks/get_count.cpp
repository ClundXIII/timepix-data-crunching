#include "../tasks.hh"
#include "../classes/pixelhit.hh"
#include "../util/split.hh"
#include "../util/tot_keV_conversion.hh"

#include <vector>
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

using namespace PDC;

const long  toa_clck =  40;
const long ftoa_clck = 640;

const long clck_ratio = ftoa_clck / toa_clck;

int PDC::get_count(colMapping thisColMapping, std::string input, long zero,
        long range, bool maskToA, int totCut, bool volume, calibrationParameter params, bool calVolume) {
    if (maskToA) {
        zero = zero & 0b1111111;
        range = range & 0b1111111;
    }

    std::cout << "ftoa_clck / toa_clck: " << clck_ratio << std::endl;

    std::ifstream file(input);

    int crash_cout = 0;

    std::vector<long> ftoa_time(256*256,0);

    long count = 0;

    while (!file.eof()) {
        std::string line;
        std::getline(file, line);

        auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

        long ToA = -1;
        long fToA = -1;
        long ToT = 1;

        try {
            ToA    = std::stol(row.at(thisColMapping.toa));
            fToA   = std::stoi(row.at(thisColMapping.ftoa));
            if (totCut > 0) {
                ToT = std::stoi(row.at(thisColMapping.tot));
            }
        } catch(...) {
            std::cout << "skipping line \"" <<  line << "\"" << std::endl;
            crash_cout ++;
            continue;
        }

        if (maskToA) {
            ToA = ToA & 0b1111111;
        }

        if ((ToA > (zero-range) && ToA < (zero+range)) && ToT > totCut ) {
            if (volume) {
                if (calVolume) {
                    count += (totTokeV(ToT, params)*10.f+0.5f);
                } else {
                    count += ToT;
                }
            } else {
                count++;
            }
        }
    }
    std::cout << (count/10) << std::endl;
    return 0;
}
