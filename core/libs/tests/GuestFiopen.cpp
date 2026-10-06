#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdio>
#include <stdexcept>
#include <string_view>

extern "C" std::FILE* APS5_VABI _ZSt7_FiopenPKcNSt5_IosbIiE9_OpenmodeEi_nid_postfix(const char*, int, int);

int main() {
    try {
        _ZSt7_FiopenPKcNSt5_IosbIiE9_OpenmodeEi_nid_postfix("unused", 1, 0);
    } catch (const std::runtime_error& error) {
        return std::string_view(error.what()) == "std::_Fiopen not implemented" ? 0 : 1;
    }
    return 1;
}
