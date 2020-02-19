#include "../tasks.hh"
#include "../classes/pixelhit.hh"
#include "../util/split.hh"
#include "../util/tot_keV_conversion.hh"

#include <vector>
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

using namespace PDC;

const long  toa_clck =  40;
const long ftoa_clck = 640;

const long clck_ratio = ftoa_clck / toa_clck;

int PDC::extract_pixet_abct(std::string input, std::string output, bool forceOverwrite) {


    return 0;
}
