#ifndef PDC_transform_hh_included
#define PDC_transform_hh_included

#include <vector>
#include <string>

namespace PDC {

template <class T>
inline std::vector<std::vector<T>> transform(std::vector<std::vector<T>> in) {
    std::vector<std::vector<T>> retVal;

    for (unsigned int i=0; i<in.at(0).size(); i++) {
        std::vector<T> tmpV;
        for (unsigned int j=0; j<in.size(); j++) {
            tmpV.push_back(in.at(j).at(i));
        }
        retVal.push_back(tmpV);
    }

    return retVal;
}
}

#endif /* PDC_transform_hh_included */
