#include "../tasks.hh"
#include "../classes/pixelhit.hh"
#include "../util/split.hh"
#include "../util/tot_keV_conversion.hh"
#include "../util/base64.hh"

#include <vector>
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

using namespace PDC;

const long toa_clck = 40;
const long ftoa_clck = 640;

const long clck_ratio = ftoa_clck / toa_clck;

int PDC::extract_pixet_abct(std::string input, std::string output,
        bool forceOverwrite, bool includePixelIndex) {

    std::vector<double> a(256*256, 0);
    std::vector<double> b(256*256, 0);
    std::vector<double> c(256*256, 0);
    std::vector<double> t(256*256, 0);

    std::ifstream file(input);

    std::cout << "sizeof(unsigned char[524288])=" << sizeof(unsigned char[524288]) << std::endl;
    std::cout << "sizeof(double[256*256])=" << sizeof(double[256*256]) << std::endl;

    while (!file.eof()) {
        std::string line;
        std::getline(file, line);

        unsigned long find_val = line.find("<calib");
        if ( find_val != std::string::npos) {
            char calib_char = line[find_val + 6];
            std::cout << "found calib \"" << calib_char << "\"" << std::endl;

            line = line.substr(find_val + 8);

            std::string data;
            do {
                find_val = line.find("</calib");
                if (find_val != std::string::npos) {
                    line = line.substr(0, find_val);
                }

                data = data + removeNonBase64(line);

                if (find_val != std::string::npos) {
                    break;
                }

                std::getline(file, line);
            } while (!file.eof()) ;
            std::cout << "found calib tag end" << std::endl;
            std::cout << "str length: " << data.size() << std::endl;

            size_t output_len = -1;

            auto byteP = base64_decode(data.c_str(),
                    699052, output_len);

            _byte_double* byte_double_dataP = (_byte_double*) byteP;

            std::cout << "Long Data amount: " << output_len << std::endl;

            /*std::cout << "val 123:" << byte_double_dataP->d[123] << std::endl;
            std::cout << "val 0:" << byte_double_dataP->d[0] << std::endl;
            std::cout << "val 256*256-1:" << byte_double_dataP->d[256*256-1] << std::endl;
            std::cout << "val 30000:" << byte_double_dataP->d[30000] << std::endl;*/

            switch (calib_char) {
            case 'a': {
                for (int i = 0; i < (256 * 256); i++) {
                    a.at(i) = byte_double_dataP->d[i];
                }
                break;
            }
            case 'b': {
                for (int i = 0; i < (256 * 256); i++) {
                    b.at(i) = byte_double_dataP->d[i];
                }
                break;
            }
            case 'c': {
                for (int i = 0; i < (256 * 256); i++) {
                    c.at(i) = byte_double_dataP->d[i];
                }
                break;
            }
            case 't': {
                for (int i = 0; i < (256 * 256); i++) {
                    t.at(i) = byte_double_dataP->d[i];
                }
                break;
            }
            }

            free(byteP);
        }
    }

    file.close();

    std::ofstream calibOut(output, std::ofstream::out | std::ofstream::trunc);

    calibOut << "#a b c t";
    if (includePixelIndex) {
        calibOut << " pixelIdx";
    }
    calibOut << std::endl;

    for (int i=0; i<(256*256); i++) {
        calibOut << a.at(i) << " " << b.at(i) << " " << c.at(i) << " " << t.at(i);
        if (includePixelIndex) {
            calibOut << " " << i;
        }
        calibOut << std::endl;
    }

    calibOut.close();

    return 0;
}
