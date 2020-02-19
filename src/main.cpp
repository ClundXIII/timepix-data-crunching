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
    tNONE = 0,
    CALIBRATE,
    EXTRACT_PIXET_ABCT,
    APPLY_SIMPLE_CALIBRATION,
    APPLY_CALIBRATION,
    CLUSTER,
    TIME_BIN,
    GET_FTOA_XY,
    GET_BINARY_TOA,
    GET_COUNT
};

map<string, task> taskNameMap = {
    { "calibrate", CALIBRATE },
    { "extract_pixet_abct", EXTRACT_PIXET_ABCT},
    { "apply_simple_calibration", APPLY_SIMPLE_CALIBRATION },
    { "apply_calibration", APPLY_CALIBRATION },
    { "cluster", CLUSTER },
    { "time_bin", TIME_BIN },
    { "get_ftoa_xy", GET_FTOA_XY },
    { "get_binary_toa", GET_BINARY_TOA },
    { "get_count", GET_COUNT }
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
    string output;
    string input;
    calibrationParameter defaultCalibrationParameter;
    long zero = 0;
    long range = 0;
    bool maskToA = false;
    bool volume = false;
    bool calVolume = false;

    bool fail = false;

    int totCut = 0;

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

                switch (todo) {
                case CALIBRATE: {
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
                default:
                    cout << "no task yet given, treat -i as single input file string" << endl;
                case TIME_BIN: {
                    input = argv[argPos];
                }
                }
                break;
            }
            case 'o':
                argPos++;
                output = argv[argPos];
                break;
            case 'k': {
                argPos++;

                auto paramStrs = split(argv[argPos], ',');

                defaultCalibrationParameter.a = std::stod(paramStrs[0]);
                defaultCalibrationParameter.b = std::stod(paramStrs[1]);
                defaultCalibrationParameter.c = std::stod(paramStrs[2]);
                defaultCalibrationParameter.t = std::stod(paramStrs[3]);

                calVolume = true;
                break;
            }
            case '0':
                argPos++;
                zero = stol(argv[argPos]);
                break;
            case 'r':
                argPos++;
                range = stol(argv[argPos]);
                break;
            case 'm':
                maskToA = true;
                break;
            case 'C':
                argPos++;
                totCut = stol(argv[argPos]);
                break;
            case 'V':
                volume = true;
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

        fs::path data_dir(output);

        if (fs::exists(data_dir)) {
            if (forceOverwrite) {
                fs::remove_all(data_dir);
            } else {
                cout << "output directory already exists!" << endl;
                return 1;
            }
        }

        fs::create_directories(data_dir);

        int retVal = calibrate_tpx3(thisColMapping, calibrationInputFiles, output, defaultCalibrationParameter);
        cout << "finished with status " << retVal << endl;
        break;
    }
    case EXTRACT_PIXET_ABCT:
        cout << "extracting pixet a,b,c,t parameter" << endl;
        int retVal = extract_pixet_abct(input, output, forceOverwrite);
        cout << "finished with status " << retVal << endl;
        break;
    case APPLY_CALIBRATION:
        cout << "applying calibration" << endl;
        break;
    case APPLY_SIMPLE_CALIBRATION:
        cout << "applying simple calibration" << endl;
        break;
    case CLUSTER:
        cout << "doing clustering" << endl;
        break;
    case TIME_BIN: {
        cout << "doing time binning" << endl;
        int retVal = bin_toa_ftoa(thisColMapping, input, output, zero, range, maskToA);
        cout << "finished with status " << retVal << endl;
        break;
    }
    case GET_FTOA_XY: {
        cout << "doing time get_ftoa_xy" << endl;
        int retVal = get_ftoa_xy(thisColMapping, input, output, zero, range, maskToA);
        cout << "finished with status " << retVal << endl;
        break;
    }
    case GET_BINARY_TOA: {
        cout << "doing time get_binary_toa" << endl;
        int retVal = get_binary_toa(thisColMapping, input, output);
        cout << "finished with status " << retVal << endl;
        break;
    }
    case GET_COUNT: {
        cout << "doing time get_count" << endl;
        int retVal = get_count(thisColMapping, input, zero, range, maskToA,
                totCut, volume, defaultCalibrationParameter, calVolume);
        cout << "finished with status " << retVal << endl;
        break;
    }
    case tNONE:
    default:
        cout << "" << endl << endl;
        cout << " supported general arguments:" << endl;
        cout << "   -d tpx3" << endl;
        cout << "      specifies what detector is assumed" << endl;
        cout << "   -t [ calibrate | apply_calibration | cluster | time_bin | get_binary_toa | get_ftoa_xy | get_count ]" << endl;
        cout << "      specifies what task is being executed" << endl;
        cout << "   -c [ tot | toa | ftoa | matIdx | overflow | matIdx_x | matIdx_y] <number>" << endl;
        cout << "      specifies the column of the parameter for input files, counting starts at 1" << endl;
        cout << "   -f overwrite output files / directories, deletes target files before writing" << endl;
        cout << "   -m mask toa, use only lowest 14 bits. Useful for high-intensity bursts and cheaper readouts" << endl << endl;
        cout << " arguments for specific tasks:" << endl;
        cout << "   calibrate:" << endl;
        cout << "     -i <filename> e1,e2,..." << endl;
        cout << "        input file name with pixel data and peaks that are expected at energy e1, e2 etc (in keV)" << endl;
        cout << "     -o <directory>" << endl;
        cout << "        directory with output files" << endl;
        cout << "     -k <A>,<B>,<C>,<T>" << endl;
        cout << "        initial coefficients" << endl;
        cout << "   extract_pixet_abct:" << endl;
        cout << "     -i <filename>" << endl;
        cout << "        input .xml file" << endl;
        cout << "     -o <filename>" << endl;
        cout << "        base output file name; result will be in <filename>.a etc" << endl;
        cout << "   get_binary_toa" << endl;
        cout << "     -i <filename>" << endl;
        cout << "        input file name with pixel data" << endl;
        cout << "     -o <filename>" << endl;
        cout << "        output file name" << endl;
        cout << "   time_bin / get_ftoa_xy: output will be [t]=ns from zero-range until zero+range" << endl;
        cout << "     -i <filename>" << endl;
        cout << "        input file name with pixel data" << endl;
        cout << "     -o <filename>" << endl;
        cout << "        output file name" << endl;
        cout << "     -0 <toa>" << endl;
        cout << "        time that will appear as zero" << endl;
        cout << "     -r <toa>" << endl;
        cout << "        range of what time will be used" << endl;
        cout << "   get_count" << endl;
        cout << "     -i <filename>" << endl;
        cout << "        input file name with pixel data" << endl;
        cout << "     -0 <toa>" << endl;
        cout << "        time that will appear as zero" << endl;
        cout << "     -r <toa>" << endl;
        cout << "        range of what time will be used" << endl;
        cout << "     -C <tot>" << endl;
        cout << "        (optional) only count if above certain tot" << endl;
        cout << "     -k <A>,<B>,<C>,<T>" << endl;
        cout << "        calibration parameters" << endl;
        cout << "     -V calculate Volume (aka add tots)" << endl << endl;
    }

    return 0;
}
