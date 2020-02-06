#include "pixelhit.hh"

#include <sstream>

using namespace PDC;

pixelhit::pixelhit(unsigned int matIdx, unsigned int ToA, unsigned int ToT,
        unsigned int fToA) :
        pixelhit(0, matIdx, matIdx / 256, matIdx % 256, ToA, ToT, fToA, false) {
}

pixelhit::pixelhit(unsigned int matX, unsigned int matY, unsigned int ToA,
        unsigned int ToT, unsigned int fToA) :
        pixelhit(0, matX * 256 + matY, matX, matY, ToA, ToT, fToA, false) {
}

pixelhit::pixelhit(unsigned int idx, unsigned int matIdx, unsigned int matX,
        unsigned int matY, unsigned int ToA, unsigned int ToT,
        unsigned int fToA, bool overflow) :
        idx(idx), matIdx(matIdx), ToA(ToA), ToT(ToT), fToA(fToA), overflow(
                overflow), matX(matX), matY(matY) {
}

pixelhit::~pixelhit() {
}
