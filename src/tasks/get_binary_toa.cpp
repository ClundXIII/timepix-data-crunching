#include "../tasks.hh"
#include "../classes/pixelhit.hh"
#include "../util/split.hh"

#include <vector>
#include <fstream>
#include <iostream>
#include <bitset>

using namespace PDC;

int PDC::get_binary_toa(colMapping thisColMapping, std::string input,
        std::string output) {

    std::ifstream file(input);

    std::ofstream binOut(output, std::ofstream::out | std::ofstream::trunc);

    int crash_cout = 0;

    while (!file.eof()) {
        std::string line;
        std::getline(file, line);

        auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

        long ToA = -1;

        try {
            ToA    = std::stol(row.at(thisColMapping.toa));
        } catch(...) {
            std::cout << "skipping line \"" <<  line << "\"" << std::endl;
            crash_cout ++;
            continue;
        }

        binOut << (std::bitset<32>(ToA)) << " " << (std::bitset<8>(ToA & 0b1111111)) << " " << (ToA & 0b1111111) << " " << std::endl;
    }

    binOut.close();

    return 0;
}
