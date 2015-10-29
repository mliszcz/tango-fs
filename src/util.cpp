#include <vector>
#include <string>
#include <regex>

#include <util.hpp>

namespace util {

    std::vector<std::string> split_string(const std::string& str,
                                          const std::regex& sep) {
        return std::vector<std::string>(
            std::sregex_token_iterator(str.begin(), str.end(), sep, -1),
            std::sregex_token_iterator()
        );
    }
}
