#include "pixelhit.hh"

#include <sstream>

using namespace PDC;

pixelhit::pixelhit(unsigned int matIdx, unsigned int ToA, unsigned int ToT,
        unsigned int FToA) :
        pixelhit(0, matIdx, ToA, ToT, FToA, false) {
}

pixelhit::pixelhit(unsigned int idx, unsigned int matIdx, unsigned int ToA,
        unsigned int ToT, unsigned int FToA, bool overflow) :
        pixelhit(idx, matIdx, matIdx % 256, matIdx / 256, ToA, ToT, FToA,
                overflow) {
}

pixelhit::pixelhit(unsigned int idx, int matX, int matY, unsigned int ToA,
        unsigned int ToT, unsigned int FToA, bool overflow) :
        pixelhit(idx, matX * 256 + matY, matX, matY, ToA, ToT, FToA, overflow) {
}

pixelhit::pixelhit(unsigned int idx, unsigned int matIdx, int matX, int matY,
        unsigned int ToA, unsigned int ToT, unsigned int FToA, bool overflow) :
        idx(idx), matIdx(matIdx), ToA(ToA), ToT(ToT), FToA(
                FToA), overflow(overflow), matX(matX), matY(matY) {
}

pixelhit::~pixelhit() {}
