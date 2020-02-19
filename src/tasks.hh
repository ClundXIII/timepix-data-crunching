#ifndef PDC_tasks_hh_included
#define PDC_tasks_hh_included

#include <fstream>
#include <vector>
#include <string>

#include "classes/structs.hh"

namespace PDC {

int calibrate_tpx3(colMapping thisColMapping,
        std::vector<calibrationInputFile> inputFiles, std::string outputDir,
        calibrationParameter defaultCalibrationParameter);

int apply_simple_calibration(colMapping thisColMapping, std::string input,
        std::string output, calibrationParameter defaultCalibrationParameter,
        bool forceOverwrite);

int extract_pixet_abct(std::string input, std::string output, bool forceOverwrite);

int bin_toa_ftoa(colMapping thisColMapping, std::string input,
        std::string output, long zero, long range, bool maskToA);

int get_ftoa_xy(colMapping thisColMapping, std::string input,
        std::string output, long zero, long range, bool maskToA);

int get_binary_toa(colMapping thisColMapping, std::string input,
        std::string output);

int get_count(colMapping thisColMapping, std::string input, long zero,
        long range, bool maskToA, int totCut, bool volume, calibrationParameter params, bool calVolume);

} /* namespace PDC */

#endif /* PDC_tasks_hh_included */
