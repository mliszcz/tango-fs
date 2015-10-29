#pragma once

#include <vector>
#include <string>
#include <regex>

namespace util {

    std::vector<std::string> split_string(const std::string& str,
                                          const std::regex& sep);

    inline std::vector<std::string> split_string(const std::string& str,
                                                 const std::string& sep) {
        return split_string(str, std::regex(sep));
    }
}
