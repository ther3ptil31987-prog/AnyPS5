#include "SceTypes.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include "prx/libkernel/File/include/FileFlags.hpp"
#include "prx/libkernel/KernelErrors.hpp"
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
extern "C" {
int APS5_VABI sceKernelOpen(const char* path, int flags, std::uint16_t mode);
int APS5_VABI sceKernelClose(int d);
int APS5_VABI sceKernelAioDeleteRequest(std::int32_t id, std::int32_t* ret);
int APS5_VABI sceKernelAioInitializeImpl(void* param, std::int32_t size);
void APS5_VABI sceKernelAioInitializeParam(void* param);
int APS5_VABI sceKernelAioSubmitReadCommands(KernelAioRwRequest* req, std::int32_t size, std::int32_t prio, std::int32_t* id);
int APS5_VABI sceKernelAioSubmitWriteCommands(KernelAioRwRequest* req, std::int32_t size, std::int32_t prio, std::int32_t* id);
int APS5_VABI sceKernelAioPollRequest(std::int32_t id, std::int32_t* state);
int APS5_VABI sceKernelAioWaitRequest(std::int32_t id, std::int32_t* state, std::uint32_t* usec);
}
static void Require(bool value, int line) {
    if (!value) {
        std::fprintf(stderr, "Aio check failed at line %d\n", line);
        std::abort();
    }
}
#define Check(value) Require((value), __LINE__)
int main() {
    const auto root = std::filesystem::path("anyps5-aio-test-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    Check(std::filesystem::create_directory(root));
    const auto file = root / "data.bin";
    const std::string seed = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
    { std::ofstream stream(file, std::ios::binary); stream << seed; }
    Check(sceKernelAioInitializeImpl(nullptr, 0) == 0);
    int initParam = 0;
    sceKernelAioInitializeParam(&initParam);
    const int fd = sceKernelOpen(file.string().c_str(), SCE_KERNEL_O_RDONLY, 0);
    Check(fd >= 0);
    std::array<char, 16> first{};
    std::array<char, 16> second{};
    KernelAioResult firstResult{-1, 0};
    KernelAioResult secondResult{-1, 0};
    KernelAioRwRequest requests[2] = {
        {0, first.size(), first.data(), &firstResult, fd},
        {32, second.size(), second.data(), &secondResult, fd},
    };
    std::int32_t id = 0;
    Check(sceKernelAioSubmitReadCommands(requests, 2, 0, &id) == 0);
    Check(id > 0);
    std::int32_t state = 0;
    Check(sceKernelAioWaitRequest(id, &state, nullptr) == 0);
    Check(state == 3);
    Check(firstResult.state == 3 && firstResult.return_value == 16);
    Check(secondResult.state == 3 && secondResult.return_value == 16);
    Check(std::memcmp(first.data(), seed.data(), 16) == 0);
    Check(std::memcmp(second.data(), seed.data() + 32, 16) == 0);
    std::int32_t polled = 0;
    Check(sceKernelAioPollRequest(id, &polled) == 0);
    Check(polled == 3);
    std::int32_t deleted = -1;
    Check(sceKernelAioDeleteRequest(id, &deleted) == 0);
    Check(deleted == 0);
    Check(sceKernelClose(fd) == 0);
    const int writeFd = sceKernelOpen(file.string().c_str(), SCE_KERNEL_O_RDWR, 0);
    Check(writeFd >= 0);
    std::array<char, 5> written = {'H', 'E', 'L', 'L', 'O'};
    KernelAioResult writeResult{-1, 0};
    KernelAioRwRequest writeRequest{0, written.size(), written.data(), &writeResult, writeFd};
    std::int32_t writeId = 0;
    Check(sceKernelAioSubmitWriteCommands(&writeRequest, 1, 0, &writeId) == 0);
    std::int32_t writeState = 0;
    Check(sceKernelAioWaitRequest(writeId, &writeState, nullptr) == 0);
    Check(writeState == 3 && writeResult.state == 3 && writeResult.return_value == 5);
    Check(sceKernelClose(writeFd) == 0);
    { std::ifstream stream(file, std::ios::binary); std::string contents; std::getline(stream, contents);
      Check(contents.rfind("HELLO", 0) == 0); }
    Check(sceKernelAioSubmitReadCommands(nullptr, 1, 0, &id) == SCE_KERNEL_ERROR_EFAULT);
    Check(sceKernelAioSubmitReadCommands(requests, 0, 0, &id) == SCE_KERNEL_ERROR_EINVAL);
    Check(sceKernelAioSubmitReadCommands(requests, 1, 0, nullptr) == SCE_KERNEL_ERROR_EFAULT);
    Check(sceKernelAioWaitRequest(9999, &state, nullptr) == SCE_KERNEL_ERROR_EINVAL);
    Check(sceKernelAioWaitRequest(id, nullptr, nullptr) == SCE_KERNEL_ERROR_EFAULT);
    Check(sceKernelAioPollRequest(id, nullptr) == SCE_KERNEL_ERROR_EFAULT);
    Check(sceKernelAioPollRequest(0, &polled) == SCE_KERNEL_ERROR_EINVAL);
    Check(sceKernelAioDeleteRequest(id, nullptr) == SCE_KERNEL_ERROR_EFAULT);
    Check(std::filesystem::remove_all(root) > 0);
}
