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
            case 'c':
                argPos++;
                if (string(argv[argPos]) == "tot") {
                    argPos++;
                    thisColMapping.tot = std::stoi(argv[argPos]);
                } else if (string(argv[argPos]) == "toa") {
                    argPos++;
                    thisColMapping.toa = std::stoi(argv[argPos]);
                } else if (string(argv[argPos]) == "ftoa") {
                    argPos++;
                    thisColMapping.ftoa = std::stoi(argv[argPos]);
                } else if (string(argv[argPos]) == "matIdx") {
                    argPos++;
                    thisColMapping.matIdx = std::stoi(argv[argPos]);
                } else if (string(argv[argPos]) == "overflow") {
                    argPos++;
                    thisColMapping.overflow = std::stoi(argv[argPos]);
                } else if (string(argv[argPos]) == "matIdx_x") {
                    argPos++;
                    thisColMapping.matIdx_x = std::stoi(argv[argPos]);
                } else if (string(argv[argPos]) == "matIdx_y") {
                    argPos++;
                    thisColMapping.matIdx_y = std::stoi(argv[argPos]);
                } else {
                    cout << "cannot recognize column type";
                    fail = true;
                }
                break;
            case 'f':
                forceOverwrite = true;
                break;
            case 'i': {
                argPos++;
                calibrationInputFile tmpCfg;
                tmpCfg.file = new ifstream(argv[argPos]);
                argPos++;

                auto energyStrs = split(argv[argPos], ',');

                for (auto const& s : energyStrs) {
                    tmpCfg.energies.push_back(std::stod(s));
                }

                calibrationInputFiles.push_back(tmpCfg);
                break;
            }
            case 'o':
                argPos ++;
                outputDirectory = argv[argPos];
                break;
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
        int retVal = calibrate_tpx3(thisColMapping, calibrationInputFiles, outputDirectory);
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
    }

    for (auto cfg : calibrationInputFiles) { // close all potential input files
        cfg.file->close();
        delete cfg.file;
    }

    return 0;
}
