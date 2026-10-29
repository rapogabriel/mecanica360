#pragma once

#include "necessary_part.hpp"
#include "vehicle.hpp"
#include <string>
#include <vector>
struct OpeningData {
    int clientId;
    Vehicle vehicle;
    std::string diagnosis;
    std::vector<NecessaryPart> necessary_parts;
    double labor;
    int deadline;
};