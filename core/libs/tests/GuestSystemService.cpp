#include "SceTypes.hpp"
#include "prx/libSceSystemService/SystemService.hpp"
#include <cstdlib>

extern "C" int APS5_VABI sceSystemServiceGetHdrToneMapLuminance(SystemServiceHdrToneMapLuminance* luminance);

namespace {

void Require(bool value) { if (!value) std::abort(); }

}

int main() {
    Require(sceSystemServiceGetHdrToneMapLuminance(nullptr) == SYSTEM_SERVICE_ERROR_PARAMETER);
    SystemServiceHdrToneMapLuminance luminance{-1.0f, -1.0f, -1.0f};
    Require(sceSystemServiceGetHdrToneMapLuminance(&luminance) == SYSTEM_SERVICE_OK);
    Require(luminance.max_full_frame_tone_map_luminance == 100.0f);
    Require(luminance.max_tone_map_luminance == 100.0f);
    Require(luminance.min_tone_map_luminance == 0.0f);
}
