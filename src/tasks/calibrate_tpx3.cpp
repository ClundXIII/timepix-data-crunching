#include "../tasks.hh"
#include "gnuplot-iostream.h"
#include "../classes/pixelhit.hh"
#include "../util/split.hh"
#include "../util/transform.hh"
#include "../util/tot_keV_conversion.hh"
#include "../util/fit_gauss.hh"

#include <vector>
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

using namespace PDC;

int PDC::calibrate_tpx3(colMapping thisColMapping,
        std::vector<calibrationInputFile> inputFiles, std::string outputDir,
        calibrationParameter defaultCalibrationParameter) {

    std::vector<std::tuple<double, double>> tot_per_keV;

    fs::create_directories(outputDir + "/hist/");


    for (auto const& fileConf : inputFiles) {

        std::vector<std::vector<std::vector<double>>> totHistEList;
        std::vector<std::vector<double>> x_keV_binsEList;
        std::vector<std::vector<double>> allPixelHistEList;


        for (unsigned int energyNumber = 0; energyNumber < fileConf.energies.size(); energyNumber++) {
            auto energy = fileConf.energies.at(energyNumber);
            gaussParameter fitParams;

            std::vector<std::vector<double>> totHist; // per pixel Index
            std::vector<double> x_keV_bins;

            double totGuess = keVTotot(energy, defaultCalibrationParameter);

            std::cout << "estimated for keV=" << energy << " tot=" << totGuess << std::endl;

            for (int bin = 0.5*totGuess; bin<=1.25*totGuess; bin++) {
                x_keV_bins.push_back(bin);
            }

            for (unsigned int i=0; i<(256*256); i++) {
                std::vector<double> tmpHist(x_keV_bins.size(), 0);
                totHist.push_back(tmpHist);
            }

            std::vector<double> allPixelHist(x_keV_bins.size(), 0);  // all pixel added

            totHistEList.push_back(totHist);
            x_keV_binsEList.push_back(x_keV_bins);
            allPixelHistEList.push_back(allPixelHist);
        }

        std::ifstream file(fileConf.filename);

        int crash_cout = 0;
        int lineCount = 0;

        std::cout << "binning data (1 dot ^= 50000 lines)" << std::endl;
        while (!file.eof() && crash_cout <= 5) {
            std::string line;
            std::getline(file, line);

            lineCount++;
            if (lineCount % 50000 == 0) {
                std::cout << "."; std::cout.flush();
            }

            auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

            int matIdx = -1;
            int ToA = -1;
            int ToT = -1;
            int fToA = -1;

            //if (thisColMapping.matIdx > 0) {
                try {
                    matIdx = std::stoi(row.at(thisColMapping.matIdx));
                    ToA = std::stol(row.at(thisColMapping.toa));
                    ToT = std::stoi(row.at(thisColMapping.tot));
                    fToA = std::stoi(row.at(thisColMapping.ftoa));
                } catch(...) {
                    std::cout << "skipping line \"" <<  line << "\"" << std::endl;
                    crash_cout ++;
                    continue;
                }
            /*} else if ((thisColMapping.matIdx_x > 0) && (thisColMapping.matIdx_y > 0)) {
                try {
                    int matIdx_x = std::stoi(row.at(thisColMapping.matIdx_x));
                    int matIdx_y = std::stoi(row.at(thisColMapping.matIdx_y));
                    int ToA = std::stoi(row.at(thisColMapping.toa));
                    int ToT = std::stoi(row.at(thisColMapping.tot));
                    int fToA = std::stoi(row.at(thisColMapping.ftoa));

                    fileDataHit = pixelhit(matIdx_x, matIdx_y, ToA, ToT, fToA);
                } catch(...) {
                    std::cout << "skipping line \"" <<  line << "\"" << std::endl;
                    crash_cout ++;
                }
            }*/
            pixelhit fileDataHit = pixelhit(matIdx, ToA, ToT, fToA);

            for (unsigned int energyNumber = 0; energyNumber < fileConf.energies.size(); energyNumber++) {
                auto energy = fileConf.energies.at(energyNumber);

                auto thisToT = fileDataHit.ToT;

                int binIdx = (int) ((double)thisToT-(double)x_keV_binsEList.at(energyNumber).at(0)+0.5);

                if ((0 <= binIdx) && (binIdx < x_keV_binsEList.at(energyNumber).size())) {
                    totHistEList.at(energyNumber).at(fileDataHit.matIdx).at(binIdx) += 1;
                    allPixelHistEList.at(energyNumber).at(binIdx) += 1;
                }

            }

        }

        file.close();

        for (unsigned int energyNumber = 0; energyNumber < fileConf.energies.size(); energyNumber++) {
            auto energy = fileConf.energies.at(energyNumber);
            std::stringstream ss1;
            auto f_all = split(fileConf.filename, '/');
            ss1 << outputDir + "/hist/" + f_all.at(f_all.size()-1) + "_";
            ss1 << energy;
            ss1 << "_all.dat";
            std::ofstream histOut(ss1.str(), std::ofstream::out);

            for (unsigned int j=0; j<x_keV_binsEList.at(energyNumber).size(); j++) {
                histOut << x_keV_binsEList.at(energyNumber).at(j) << " " << allPixelHistEList.at(energyNumber).at(j) << " " << totHistEList.at(energyNumber).at(1234).at(j) << std::endl;
            }

            histOut.close();
        }

        gaussParameter p;
        //fitGauss(0, 0, p);

        std::cout << std::endl << "binning done" << std::endl;
        std::cout << "fit data (1 dot = 128 fits)" << std::endl;

    }

    return 0;
}
