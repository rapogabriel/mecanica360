#pragma once

#include <vector>

#include "ValueClasses.h"

namespace mecanica360 {


struct OpeningData {
    int clientId = 0;
    Vehicle vehicle;
    std::string diagnosis;
    std::vector<NecessaryPart> parts;
    double labor = 0.0;
    int deadline = 0;
};

}  // namespace mecanica360