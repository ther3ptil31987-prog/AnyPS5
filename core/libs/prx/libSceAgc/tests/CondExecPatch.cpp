#include "prx/libSceAgc/Command/include/Packet.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

extern "C" {
std::uint32_t* APS5_VABI sceAgcDcbCondExec(CommandBuffer*, const volatile std::uint32_t*, std::uint32_t);
int APS5_VABI sceAgcCondExecPatchSetCommandAddress(std::uint32_t*, const volatile std::uint32_t*);
int APS5_VABI sceAgcCondExecPatchSetEnd(std::uint32_t*, const volatile std::uint32_t*);
}

namespace {

constexpr std::uint32_t Sentinel = 0xabcdef01u;

void check(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

template <typename TAction>
void expectFailure(TAction action) {
    try {
        action();
    } catch (const std::runtime_error&) {
        return;
    }
    throw std::runtime_error("expected invalid input to fail");
}

const volatile std::uint32_t* at(std::uintptr_t address) {
    return reinterpret_cast<const volatile std::uint32_t*>(address);
}

struct Storage {
    std::array<std::uint32_t, 16> words{};
    CommandBuffer buffer{words.data(), words.data() + words.size(), words.data(), words.data() + words.size(), nullptr, nullptr, 0};
    std::uint32_t condition = 1;
    std::uint32_t* packet = nullptr;

    Storage() {
        words.fill(Sentinel);
        packet = sceAgcDcbCondExec(&buffer, &condition, 3);
        check(packet == words.data() && packet[0] == 0xc0032200u && packet[4] == 3, "unexpected COND_EXEC packet");
    }
};

std::uintptr_t endAfter(const std::uint32_t* packet, std::uintptr_t numDwords) {
    return reinterpret_cast<std::uintptr_t>(packet + 5) + numDwords * sizeof(std::uint32_t);
}

template <typename TAction>
void expectUnchanged(TAction action) {
    Storage storage;
    const auto before = storage.words;
    expectFailure([&] { action(storage.packet); });
    check(storage.words == before, "failed patch modified the packet");
}

void testSetEnd() {
    for (const std::uintptr_t numDwords : {0u, 1u, 7u, 0x3fffu}) {
        Storage storage;
        const auto before = storage.words;
        check(sceAgcCondExecPatchSetEnd(storage.packet, at(endAfter(storage.packet, numDwords))) == 0, "SetEnd failed");
        check(storage.packet[4] == numDwords, "SetEnd wrote the wrong dword count");
        check(std::equal(before.begin(), before.begin() + 4, storage.words.begin()), "SetEnd modified other packet words");
        check(std::equal(before.begin() + 5, before.end(), storage.words.begin() + 5), "SetEnd modified following words");
    }

    expectUnchanged([](std::uint32_t* packet) { sceAgcCondExecPatchSetEnd(packet, at(endAfter(packet, 0x4000u))); });
    expectUnchanged([](std::uint32_t* packet) { sceAgcCondExecPatchSetEnd(packet, packet + 4); });
    expectUnchanged([](std::uint32_t* packet) { sceAgcCondExecPatchSetEnd(packet, nullptr); });
    expectUnchanged([](std::uint32_t* packet) { sceAgcCondExecPatchSetEnd(packet, at(endAfter(packet, 2) + 2)); });
    expectUnchanged([](std::uint32_t* packet) { sceAgcCondExecPatchSetEnd(packet + 1, at(endAfter(packet, 2))); });
}

void testSetCommandAddress() {
    for (const std::uintptr_t address : {std::uintptr_t{0x0000123456789abcu}, std::uintptr_t{4u}, std::uintptr_t{0x0000fffffffffffcu}}) {
        Storage storage;
        const auto before = storage.words;
        check(sceAgcCondExecPatchSetCommandAddress(storage.packet, at(address)) == 0, "SetCommandAddress failed");
        check(storage.packet[1] == static_cast<std::uint32_t>(address) && storage.packet[2] == static_cast<std::uint32_t>(address >> 32u), "SetCommandAddress wrote the wrong address");
        check(storage.packet[0] == before[0] && storage.packet[3] == before[3] && storage.packet[4] == before[4], "SetCommandAddress modified other packet words");
        check(std::equal(before.begin() + 5, before.end(), storage.words.begin() + 5), "SetCommandAddress modified following words");
    }

    expectUnchanged([](std::uint32_t* packet) { sceAgcCondExecPatchSetCommandAddress(packet, nullptr); });
    expectUnchanged([](std::uint32_t* packet) { sceAgcCondExecPatchSetCommandAddress(packet, at(0x1002u)); });
    expectUnchanged([](std::uint32_t* packet) { sceAgcCondExecPatchSetCommandAddress(packet + 1, at(0x1000u)); });
}

}

int main() {
    try {
        testSetEnd();
        testSetCommandAddress();
        std::puts("AGC COND_EXEC patch tests passed");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
