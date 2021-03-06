#ifndef PDC_structs_hh_included
#define PDC_structs_hh_included

#include <fstream>

namespace PDC {

// When adding functions --> convert to a class with separate header and src

struct colMapping {
    int tot = -1;
    int toa = -1;
    int ftoa = -1;
    int matIdx = -1;
    int overflow = -1;
    int matIdx_x = -1;
    int matIdx_y = -1;
    int calEnergy = -1;
    int time = -1; // ftao + toa
};

struct calibrationParameter {
    double a =  2;
    double b = 15;
    double c = 20;
    double t =  2;
};

struct gaussParameter { // ampl * exp( (x-mean)^2 / (2*sigma^2)) + t / (exp( (x-mean) / sigma ) + 1)
    double ampl;
    double mean;
    double sigma;
    double a = 0;
    double t = 0;
};

struct calibrationInputFile {
    std::string filename;
    std::vector<double> energies;
};

} /* namespace PDC */

#endif /* PDC_structs_hh_included */
