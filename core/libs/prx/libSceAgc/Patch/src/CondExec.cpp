#include "prx/libSceAgc/Patch/include/CondExec.hpp"

#include "prx/libSceAgc/Command/include/Memory.hpp"
#include "prx/libSceAgc/Command/include/Packet.hpp"
#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

extern "C" {

int APS5_VABI sceAgcCondExecPatchSetCommandAddress(uint32_t* cmd, const volatile uint32_t* command) {
    Agc::Command::ValidatePacket(cmd, 0x22u, 5, __func__);
    const auto address = reinterpret_cast<std::uintptr_t>(command);
    Agc::Command::CheckAddress(address, 4, __func__);
    cmd[1] = (cmd[1] & 3u) | (static_cast<std::uint32_t>(address) & ~3u);
    cmd[2] = static_cast<std::uint32_t>(address >> 32u);
    return 0;
}

int APS5_VABI sceAgcCondExecPatchSetEnd(uint32_t* cmd, const volatile uint32_t* buffer) {
    Agc::Command::ValidatePacket(cmd, 0x22u, 5, __func__);
    const auto end = reinterpret_cast<std::uintptr_t>(buffer);
    const auto packetEnd = reinterpret_cast<std::uintptr_t>(cmd + 5);
    Agc::Command::CheckAddress(end, 4, __func__);
    const auto numDwords = (end - packetEnd) / sizeof(std::uint32_t);
    Agc::Command::Require(numDwords <= 0x3fffu, __func__, "conditional execution range is too large");
    cmd[4] = (cmd[4] & ~0x3fffu) | static_cast<std::uint32_t>(numDwords);
    return 0;
}

int APS5_VABI sceAgcAsyncCondExecPatchSetCommandAddress(std::uint32_t* cmd, const volatile std::uint32_t* command) {
    (void)cmd;
    (void)command;
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAgcAsyncCondExecPatchSetEnd(std::uint32_t* cmd, const volatile std::uint32_t* buffer) {
    (void)cmd;
    (void)buffer;
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}
