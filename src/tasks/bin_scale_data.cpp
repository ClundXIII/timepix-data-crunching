#include "../tasks.hh"
#include "../util/split.hh"
#include "../util/tot_keV_conversion.hh"

#include <vector>
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

using namespace PDC;

inline double binToEnergy(int bin, long zero, float binwidth) {
    return ((double)bin) * binwidth + (double) zero;
}

inline double energyToFloatBin(double energy, long zero, float binwidth) {
    return (energy - ((double)zero)) / binwidth;
}

int PDC::bin_scale_data(colMapping thisColMapping, std::string input,
        std::string output, bool forceOverwrite, std::string efficiencyFile,
        long zero, long range, float binwidth) {

    if (!forceOverwrite && fs::exists(output)) {
        std::cout << "Output file already exists, use -f to force overwrite." << std::endl;
        return 1;
    }

    int totBins = (float)range / binwidth;
    std::vector<double> energyBins(totBins, 0);

    std::vector<double> efficiencyScale(totBins, 1);

    if (efficiencyFile != "") {
        std::ifstream effInputF(efficiencyFile);

        std::vector<double> effInputEnergy;
        std::vector<double> effInputFactor;
        while (!effInputF.eof()) {
            std::string line;
            std::getline(effInputF, line);

            if (line == "" || line[0] == '#') {
                continue;
            }

            auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

            effInputEnergy.push_back(std::stod(row[0]));
            effInputFactor.push_back(std::stod(row[1]));
        }

        int currentEnergyBin = 0;

        for (; currentEnergyBin < totBins &&
               currentEnergyBin < binToEnergy(currentEnergyBin, zero, binwidth);
               currentEnergyBin++) {
            efficiencyScale.at(currentEnergyBin) = 0; // set all lower bins than input file to 0
            std::cout << "skipping energy bin #" << currentEnergyBin << std::endl;
        }


        for (; currentEnergyBin < totBins &&
               currentEnergyBin < binToEnergy(currentEnergyBin+((double)range), zero, binwidth);
               currentEnergyBin++) {

            int lowerBin = energyToFloatBin(currentEnergyBin, zero, binwidth);
            double lowerEn = effInputEnergy.at(lowerBin);
            double lowerFa = effInputFactor.at(lowerBin);

            double higherEn = effInputEnergy.at(lowerBin+1);
            double higherFa = effInputEnergy.at(lowerBin+1);

            double interVal = 0; // interpolated value

            std::cout << "for bin with E=" << ((double)currentEnergyBin*binwidth+(double)zero) << " ( bin #" << lowerBin << "):" << std::endl;
            std::cout << "   interpolating between (" << lowerEn << "," << lowerFa << ") and (" << higherEn << "," << higherFa << ")" << std::endl;
            std::cout << "   value: (" << ((double)currentEnergyBin*binwidth+(double)zero) << "," << interVal << ")" << std::endl;
        }
    }

    int crash_count = 0;

    std::ifstream inputF(input);

    crash_count = 0;

    while (!inputF.eof() && crash_count < 5) {
        std::string line;
        std::getline(inputF, line);

        auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

        double calEnergy = -1;

        try {
            calEnergy = std::stod(row.at(thisColMapping.calEnergy));
        } catch(...) {
            std::cout << "skipping line \"" <<  line << "\"" << std::endl;
            crash_count ++;
            continue;
        }

        int binPosition = (int)( energyToFloatBin(calEnergy, zero, binwidth) + .5);
        if (binPosition >= 0 && binPosition < energyBins.size()){
            energyBins.at(binPosition) += efficiencyScale.at(binPosition);
        }

    }

    inputF.close();

    std::ofstream outputF(output, std::ofstream::out | std::ofstream::trunc);

    outputF << "#energy counts stddev" << std::endl;
    outputF << "#[keV] [1] [1]" << std::endl;

    for (int i = 0; i < totBins; i++) {
        outputF << (binToEnergy(i, zero, binwidth) + binwidth / 2) << " "
                << energyBins.at(i) << std::endl;
    }
    outputF.close();

    return 0;
}
