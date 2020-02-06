#include "../tasks.hh"
#include "gnuplot-iostream.h"
#include "../classes/pixelhit.hh"
#include "../util/split.hh"
#include "../util/transform.hh"
#include "../util/tot_keV_conversion.hh"

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

        std::vector<pixelhit> fileData;

        std::ifstream file(fileConf.filename);

        while (!file.eof()) {
            std::string line;
            std::getline(file, line);

            auto row = split(split(split(split(line, ';'), ','), '\t'), ' ');

            if (thisColMapping.matIdx > 0) {
                try {
                    int matIdx = std::stoi(row.at(thisColMapping.matIdx));
                    int ToA = std::stoi(row.at(thisColMapping.toa));
                    int ToT = std::stoi(row.at(thisColMapping.tot));
                    int fToA = std::stoi(row.at(thisColMapping.ftoa));

                    fileData.push_back(pixelhit(matIdx, ToA, ToT, fToA));
                } catch(...) {
                    std::cout << "skipping line \"" <<  line << "\"" << std::endl;
                }
            } else if ((thisColMapping.matIdx_x > 0) && (thisColMapping.matIdx_y > 0)) {
                try {
                    int matIdx_x = std::stoi(row.at(thisColMapping.matIdx_x));
                    int matIdx_y = std::stoi(row.at(thisColMapping.matIdx_y));
                    int ToA = std::stoi(row.at(thisColMapping.toa));
                    int ToT = std::stoi(row.at(thisColMapping.tot));
                    int fToA = std::stoi(row.at(thisColMapping.ftoa));

                    fileData.push_back(pixelhit(matIdx_x, matIdx_y, ToA, ToT, fToA));
                } catch(...) {
                    std::cout << "skipping line \"" <<  line << "\"" << std::endl;
                }
            }
        }

        file.close();

        for (auto energy : fileConf.energies) {
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

            for (auto hit : fileData) {
                auto thisToT = hit.ToT;

                int binIdx = (int) ((double)thisToT-x_keV_bins.at(0)+0.5);

                if ((0 <= binIdx) && (binIdx < x_keV_bins.size())) {
                    totHist.at(hit.matIdx).at(binIdx) += 1;
                    totHist.at(0).at(binIdx) += 1;
                }
            }

            Gnuplot gp("gnuplot >/dev/null 2>&1");
            for (unsigned int i=0; i<(256*256); i++) {
                if (i%256 == 0) { std::cout << "."; std::cout.flush();}
                std::stringstream ss;
                auto f_split = split(fileConf.filename, '/');
                ss << outputDir + "/hist/" + f_split.at(f_split.size()-1) + "_";
                ss << energy;
                ss << "_";
                ss << std::setw(5) << std::setfill('0');
                ss << i;
                ss << ".dat";
                std::ofstream histOut(ss.str(), std::ofstream::out);

                for (unsigned int j=0; j<x_keV_bins.size(); j++) {
                    histOut << x_keV_bins.at(j) << " " << totHist.at(i).at(j) << std::endl;
                }

                histOut.close();

                gp << "set fit errorvariables\n";
                gp << "set fit quiet\n";
                gp << "set fit logfile '/dev/null'\n";
                gp << "set xrange [" << x_keV_bins.at(1) << ":]\n";
                gp << "f(x) = ampl * exp( -(x-mean)**2 / (2*sigma**2)) + a / ( exp( (x-mean) / sigma ) + 1 )\n";
                int binIdx = (int) ((double)totGuess-x_keV_bins.at(0)+0.5);
                gp << "ampl = " << totHist.at(i).at(binIdx) << "\n";
                gp << "mean = " << totGuess << "\n";
                gp << "sigma = 3\n";
                gp << "a = ampl\n";

                gp << "fit f(x) '-' u 1:2 via ampl, mean, sigma, a\n";
                std::vector<std::vector<double>> tmpV = {x_keV_bins, totHist.at(i)};
                gp.send1d(transform(tmpV));

                /*break;*/
            }
            std::cout << std::endl;
            gp.close();

        }

    }

    return 0;
}
