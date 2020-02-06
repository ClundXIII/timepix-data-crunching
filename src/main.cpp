#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#include "include.hh"

#include <iostream>
#include <map>
#include <string>

using namespace PDC;
using namespace std;

namespace PDC {

enum task {
    tNONE = 0, CALIBRATE, APPLY_CALIBRATION, CLUSTER
};

map<string, task> taskNameMap = {
    { "calibrate", CALIBRATE },
    { "apply_calibration", APPLY_CALIBRATION },
    { "cluster", CLUSTER }
};

enum detectorType {
    dNONE = 0, TPX3
};

map<string, detectorType> detectorNameMap = {
    { "tpx3", TPX3 }
};

} /* namespace PDC */

int main(int argc, char* argv[]) {
    cout << endl << "timepix-data-crunching v1 (c) Simon Michalke 2020" << endl;

    task todo = tNONE;
    colMapping thisColMapping;
    detectorType thisDetectorType = TPX3;
    bool forceOverwrite = false;
    vector<calibrationInputFile> calibrationInputFiles;
    string outputDirectory;
    calibrationParameter defaultCalibrationParameter;

    bool fail = false;

    for (int argPos = 1; argPos < argc; argPos++) {
        if (argv[argPos][0] == '-') {
            switch (argv[argPos][1]) {
            case 'd':
                argPos++;
                if (detectorNameMap.find(argv[argPos]) == detectorNameMap.end()) {
                    cout << "tasks not recognized!" << endl;
                    fail = true;
                } else {
                    thisDetectorType = detectorNameMap.at(argv[argPos]);
                }
                break;
            case 't': // what task
                argPos++;
                if (taskNameMap.find(argv[argPos]) == taskNameMap.end()) {
                    cout << "tasks not recognized!" << endl;
                    fail = true;
                } else {
                    todo = taskNameMap.at(argv[argPos]);
                }
                break;
            case 'c': {
                argPos++;
                if (string(argv[argPos]) == "tot") {
                    argPos++;
                    thisColMapping.tot = std::stoi(argv[argPos])-1;
                } else if (string(argv[argPos]) == "toa") {
                    argPos++;
                    thisColMapping.toa = std::stoi(argv[argPos])-1;
                } else if (string(argv[argPos]) == "ftoa") {
                    argPos++;
                    thisColMapping.ftoa = std::stoi(argv[argPos])-1;
                } else if (string(argv[argPos]) == "matIdx") {
                    argPos++;
                    thisColMapping.matIdx = std::stoi(argv[argPos])-1;
                } else if (string(argv[argPos]) == "overflow") {
                    argPos++;
                    thisColMapping.overflow = std::stoi(argv[argPos])-1;
                } else if (string(argv[argPos]) == "matIdx_x") {
                    argPos++;
                    thisColMapping.matIdx_x = std::stoi(argv[argPos])-1;
                } else if (string(argv[argPos]) == "matIdx_y") {
                    argPos++;
                    thisColMapping.matIdx_y = std::stoi(argv[argPos])-1;
                } else {
                    cout << "cannot recognize column type";
                    fail = true;
                }
                break;
            }
            case 'f':
                forceOverwrite = true;
                break;
            case 'i': {
                argPos++;
                calibrationInputFile tmpCfg;
                tmpCfg.filename = argv[argPos];
                argPos++;

                auto energyStrs = split(argv[argPos], ',');

                for (auto const& s : energyStrs) {
                    tmpCfg.energies.push_back(std::stod(s));
                }

                calibrationInputFiles.push_back(tmpCfg);
                break;
            }
            case 'o':
                argPos++;
                outputDirectory = argv[argPos];
                break;
            case 'k': {
                argPos++;

                auto paramStrs = split(argv[argPos], ',');

                defaultCalibrationParameter.a = std::stod(paramStrs[0]);
                defaultCalibrationParameter.b = std::stod(paramStrs[1]);
                defaultCalibrationParameter.c = std::stod(paramStrs[2]);
                defaultCalibrationParameter.t = std::stod(paramStrs[3]);

                break;
            }
            default:
                cout << "unexpected flag in argument number " << argPos << endl;
                fail = true;
            }
        } else {
            cout << "expected \"-\" in argument number " << argPos << endl;
            fail = true;
        }
    }

    if (fail) {
        cout << "aborted" << endl;
        todo = tNONE;
    }

    switch (todo) {
    case CALIBRATE:{
        cout << "doing calibration" << endl;

        fs::path data_dir(outputDirectory);

        if (fs::exists(data_dir)) {
            if (forceOverwrite) {
                fs::remove_all(data_dir);
            } else {
                cout << "output directory already exists!" << endl;
                return 1;
            }
        }

        fs::create_directories(data_dir);

        int retVal = calibrate_tpx3(thisColMapping, calibrationInputFiles, outputDirectory, defaultCalibrationParameter);
        cout << "finished with status " << retVal << endl;
        break;
    }
    case APPLY_CALIBRATION:
        cout << "applying calibration" << endl;
        break;
    case CLUSTER:
        cout << "doing clustering" << endl;
        break;
    case tNONE:
    default:
        cout << "" << endl << endl;
        cout << " supported general arguments:" << endl;
        cout << "   -d tpx3" << endl;
        cout << "      specifies what detector is assumed" << endl;
        cout << "   -t [ calibrate | apply_calibration | cluster ]" << endl;
        cout << "      specifies what task is being executed" << endl;
        cout << "   -c [ tot | toa | ftoa | matIdx | overflow | matIdx_x | matIdx_y] <number>" << endl;
        cout << "      specifies the column of the parameter for input files, counting starts at 1" << endl;
        cout << "   -f overwrite output files / directories, deletes target files before writing" << endl << endl;
        cout << " arguments for specific tasks:" << endl;
        cout << "   calibrate:" << endl;
        cout << "     -i <filename> e1,e2,..." << endl;
        cout << "        input file name with pixel data and peaks that are expected at energy e1, e2 etc (in keV)" << endl;
        cout << "     -o <directory>" << endl;
        cout << "        directory with output files" << endl;
        cout << "     -k <A>,<B>,<C>,<T>" << endl;
        cout << "        initial coefficients" << endl << endl;
    }

    return 0;
}
