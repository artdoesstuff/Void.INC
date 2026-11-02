#pragma once

#include "../../Misc/Globals/GIncludes.hpp"
#include "../../Hardware/Logic.hpp"

inline std::pair<long double, int> getDownload(Logic& lg, long double currentCost, Download mode) {
    int n = 0;
    long double r = static_cast<long double>(logicGateInflation);

    if (mode == Download::X1) n = 1;
    else if (mode == Download::X5) n = 5;
    else if (mode == Download::X10) n = 10;
    else if (mode == Download::X50) n = 50;
    else if (mode == Download::X100) n = 100;
    else if (mode == Download::MAX) {
        if (currentCost < lg.currentCost) return { lg.currentCost, 1 };
        n = static_cast<int>(std::floor(std::log(1.0L + (currentCost * (r - 1.0L)) / lg.currentCost) / std::log(r)));
        if (n < 1) n = 1;
    }

    long double totalCost = lg.currentCost * (std::pow(r, static_cast<long double>(n)) - 1.0L) / (r - 1.0L);

    if (mode == Download::MAX && totalCost > currentCost && n > 1) {
        totalCost = lg.currentCost * (std::pow(r, static_cast<long double>(n)) - 1.0L) / (r - 1.0L);
        n--;
    }
    return { (n <= 1) ? lg.currentCost : totalCost, (n <= 1) ? 1 : n };
}