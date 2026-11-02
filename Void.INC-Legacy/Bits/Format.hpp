#pragma once

#include "../Misc/Globals/GIncludes.hpp"

inline std::string format(long double bits, bool forceDecimals = false) {
    if (bits < 1000.L) {
        std::ostringstream oss;
        if (forceDecimals) oss << std::fixed << std::setprecision(2) << bits;
        else oss << static_cast<long>(bits);
        return oss.str();
    }

    static const std::string suffix[] = {
        "", "K", "M", "B", "T", "Qa", "Qi", "Sx", "Sp", "Oc", "No", "Dc",
        "Ud", "Dd", "Td", "Qad", "Qid", "Sxd", "Spd", "Ocd", "Nvd", "Vg",
        "Uvg", "Dvg", "Tvg", "Qavg", "Qivg", "Sxvg", "Spvg", "Ocvg", "Nvvg", "Tg",
        "Utg", "Dtg", "Ttg", "Qattg", "Qitg", "Sxtg", "Sptg", "Octg", "Nvtg", "Qg",
        "Uqg", "Dqg", "Tqg", "Qaqg", "Qiqg", "Sxqg", "Spqg", "Ocqg", "Nvqg", "Qig",
        "Uqig", "Dqig", "Tqig", "Qaqig", "Qiqig", "Sxqig", "Spqig", "Ocqig", "Nvqig", "Sxg",
        "Usxg", "Dsxg", "Tsxg", "Qasxg", "Qisxg", "Sxsxg", "Spsxg", "Ocsxg", "Nvsxg", "Spg"
    };

    int tier = static_cast<int>(std::floor(std::log10(bits) / 3));
    if (tier >= static_cast<int>(std::size(suffix))) return "Infinity";

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << (bits / std::pow(10, tier * 3)) << suffix[tier];
    return oss.str();
}