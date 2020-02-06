#ifndef PDC_tkconversion_hh_included
#define PDC_tkconversion_hh_included

#include "../classes/structs.hh"

namespace PDC {

inline double totTokeV(double tot, calibrationParameter params) {
    return tot * (params.a * tot + params.b - params.c - params.t)
            / (params.a * tot + params.b - params.t);
}

inline double keVTotot(double keV, calibrationParameter params) {
    return params.a * keV + params.b - params.c / (keV - params.t);
}

} /* namespace PDC */

#endif /* PDC_tkconversion_hh_included */
