#ifndef PDC_pixelhit_hh_included
#define PDC_pixelhit_hh_included

#include <iostream>

namespace PDC {

class pixelhit {
public:

    pixelhit(std::string line);

    const unsigned int idx;
    const unsigned int matIdx;
    const unsigned int matX;
    const unsigned int matY;
    const unsigned int ToA;
    const unsigned int ToT;
    const unsigned int FToA;
    const bool overflow;

    ~pixelhit();
};

} /* namespace PDC */

#endif /* PDC_pixelhit_hh_included */
