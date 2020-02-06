#ifndef PDC_tasks_hh_included
#define PDC_tasks_hh_included

#include <fstream>
#include <vector>
#include "classes/structs.hh"

namespace PDC {

int calibrate_tpx3(colMapping thisColMapping,
        std::vector<calibrationInputFile> inputFiles, std::string outputDir,
        calibrationParameter defaultCalibrationParameter);

} /* namespace PDC */

#endif /* PDC_tasks_hh_included */
