#ifndef PDC_tkconversion_hh_included
#define PDC_tkconversion_hh_included

#include "../classes/structs.hh"
#include <math.h>

namespace PDC {

inline double energyThr(calibrationParameter params) {
    return sqrt(params.a*params.a*params.t*params.t + 2*params.a*params.b*params.t + 4*params.a*params.c + params.b*params.b)
            + params.a * params.t - params.b;
}

inline double totTokeV(double tot, calibrationParameter params) {
    return (1 / (2 * params.a))
            * (sqrt(
                    pow((params.a * params.t + params.b - tot), 2)
                            + 4 * params.a * params.c)
                    - (params.b - params.a * params.t - tot));
}

inline double keVTotot(double keV, calibrationParameter params) {
    if (keV < energyThr(params) ) {
        return 0;
    }
    return params.a * keV + params.b - params.c / (keV - params.t);
}

} /* namespace PDC */

#endif /* PDC_tkconversion_hh_included */
