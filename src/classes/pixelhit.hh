#ifndef PDC_pixelhit_hh_included
#define PDC_pixelhit_hh_included

#include <iostream>

namespace PDC {

class pixelhit {
public:

    pixelhit(unsigned int matIdx, unsigned int ToA, unsigned int ToT,
            unsigned int fToA);

    pixelhit(unsigned int matX, unsigned int matY, unsigned int ToA,
            unsigned int ToT, unsigned int fToA);

    pixelhit(unsigned int idx, unsigned int matIdx, unsigned int matX,
            unsigned int matY, unsigned int ToA, unsigned int ToT,
            unsigned int fToA, bool overflow);

    const unsigned int idx = 0;
    const unsigned int matIdx = 0;
    const unsigned int ToA = 0;
    const unsigned int ToT = 0;
    const unsigned int fToA = 0;
    const bool overflow = 0;

    const unsigned int matX = 0;
    const unsigned int matY = 0;

    ~pixelhit();
};

} /* namespace PDC */

#endif /* PDC_pixelhit_hh_included */
