#ifndef PDC_split_hh_included
#define PDC_split_hh_included

#include <vector>
#include <string>

namespace PDC {

inline std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

inline std::vector<std::string> split(const std::vector<std::string>& v, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    for ( auto const& s : v) {
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

} /* namespace PDC */

#endif /* PDC_split_hh_included */
