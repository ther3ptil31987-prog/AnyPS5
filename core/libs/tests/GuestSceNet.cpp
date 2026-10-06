#include "prx/libc/include/general/VabiMacros.hpp"
#include "SceTypes.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <thread>

extern "C" {
int APS5_VABI sceNetInit_nid_postfix(void);
int APS5_VABI sceNetSocket(const char*, int, int, int);
int APS5_VABI sceNetBind_nid_postfix(int, const void*, std::uint32_t);
int APS5_VABI sceNetListen(int, int);
int APS5_VABI sceNetGetsockname(int, void*, std::uint32_t*);
int APS5_VABI sceNetConnect(int, const void*, std::uint32_t);
int APS5_VABI sceNetAccept(int, void*, std::uint32_t*);
std::int64_t APS5_VABI sceNetSend(int, const void*, std::size_t, int);
std::int64_t APS5_VABI sceNetRecv(int, void*, std::size_t, int);
std::int64_t APS5_VABI sceNetSendto(int, const void*, std::size_t, int, const void*, std::uint32_t);
std::int64_t APS5_VABI sceNetRecvfrom(int, void*, std::size_t, int, void*, std::uint32_t*);
int APS5_VABI sceNetSocketClose(int);
int APS5_VABI sceNetSetsockopt(int, int, int, const void*, std::uint32_t);
int* APS5_VABI sceNetErrnoLoc(void);
int APS5_VABI sceNetEpollCreate(const char*, int);
int APS5_VABI sceNetEpollControl(int, int, int, const NetEpollEvent*);
int APS5_VABI sceNetEpollWait(int, NetEpollEvent*, int, int);
int APS5_VABI sceNetEpollDestroy(int);
int APS5_VABI sceNetResolverCreate(const char*, int, int);
int APS5_VABI sceNetResolverStartNtoa(int, const char*, void*, int, int, int);
int APS5_VABI sceNetResolverDestroy(int);
int APS5_VABI sceNetResolverGetError(int, int*);
int APS5_VABI sceNetCtlGetState(int*);
int APS5_VABI sceNetInetPton(int, const char*, void*);
const char* APS5_VABI sceNetInetNtop(int, const void*, char*, std::uint32_t);
int* APS5_VABI sceNetErrnoLoc(void);
}

static void Require(bool condition) {
    if (!condition) std::abort();
}

static void CheckAddressText(int family, const char* text) {
    std::array<std::uint8_t, 16> address{};
    Require(sceNetInetPton(family, text, address.data()) == 1);
    std::array<char, 64> output{};
    const auto length = static_cast<std::uint32_t>(std::strlen(text));
    for (std::uint32_t size = 0; size <= length; ++size) {
        *sceNetErrnoLoc() = 123;
        Require(sceNetInetNtop(family, address.data(), output.data(), size) == nullptr);
        Require(*sceNetErrnoLoc() == 28);
    }
    output.fill('x');
    *sceNetErrnoLoc() = 123;
    Require(sceNetInetNtop(family, address.data(), output.data(), length + 1) == output.data());
    Require(std::strcmp(output.data(), text) == 0 && output[length + 1] == 'x');
    Require(*sceNetErrnoLoc() == 123);
    Require(sceNetInetNtop(99, address.data(), output.data(), output.size()) == nullptr && *sceNetErrnoLoc() == 47);
    Require(sceNetInetNtop(family, nullptr, output.data(), output.size()) == nullptr && *sceNetErrnoLoc() == 22);
    Require(sceNetInetNtop(family, address.data(), nullptr, output.size()) == nullptr && *sceNetErrnoLoc() == 22);
}

int main() {
    Require(sceNetInit_nid_postfix() == 0);
    CheckAddressText(2, "127.0.0.1");
    CheckAddressText(2, "255.255.255.255");
    CheckAddressText(28, "::1");
    CheckAddressText(28, "1234:5678:9abc:def0:1234:5678:9abc:def0");

    const int listener = sceNetSocket(nullptr, 2, 1, 6);
    Require(listener >= 0);
    std::array<std::uint8_t, 16> address{16, 2, 0, 0, 127, 0, 0, 1};
    Require(sceNetBind_nid_postfix(listener, address.data(), address.size()) == 0);
    Require(sceNetListen(listener, 4) == 0);
    std::uint32_t address_size = address.size();
    Require(sceNetGetsockname(listener, address.data(), &address_size) == 0 && address_size == 16);
    Require(address[2] != 0 || address[3] != 0);

    const int client = sceNetSocket(nullptr, 2, 1, 6);
    Require(client >= 0);
    Require(sceNetConnect(client, address.data(), address.size()) == 0);
    std::array<std::uint8_t, 16> peer{};
    address_size = peer.size();
    const int accepted = sceNetAccept(listener, peer.data(), &address_size);
    Require(accepted >= 0 && address_size == 16);
    const int nonblocking = 1;
    Require(sceNetSetsockopt(accepted, 0xffff, 0x1200, &nonblocking, sizeof(nonblocking)) == 0);
    char pending = 0;
    Require(sceNetRecv(accepted, &pending, sizeof(pending), 0) == static_cast<int>(0x80410123) && *sceNetErrnoLoc() == 35);

    const int epoll = sceNetEpollCreate("guest-sce-net", 0);
    Require(epoll >= 0);
    NetEpollEvent registration{};
    registration.events = 1;
    registration.ident = static_cast<std::uint64_t>(accepted);
    registration.data.u32 = 77;
    Require(sceNetEpollControl(epoll, 1, accepted, &registration) == 0);
    const char request[] = "guest tcp loopback";
    char response[sizeof(request)]{};
    Require(sceNetSend(client, request, sizeof(request), 0) == sizeof(request));
    NetEpollEvent ready{};
    const int ready_count = sceNetEpollWait(epoll, &ready, 1, 1000000);
    Require(ready_count == 1 && (ready.events & 1) && ready.ident == static_cast<std::uint64_t>(accepted));
    Require(sceNetRecv(accepted, response, sizeof(response), 0) == sizeof(response));
    Require(std::strcmp(request, response) == 0);
    Require(sceNetEpollDestroy(epoll) == 0);
    Require(sceNetSocketClose(accepted) == 0);
    bool send_failed = false;
    for (int attempt = 0; attempt < 100 && !send_failed; ++attempt) {
        send_failed = sceNetSend(client, request, sizeof(request), 0) < 0;
        if (!send_failed) std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    Require(send_failed);
    Require(sceNetSocketClose(client) == 0);
    Require(sceNetSocketClose(listener) == 0);
    Require(sceNetSocketClose(listener) == static_cast<int>(0x80410109) && *sceNetErrnoLoc() == 9);

    const int udp_receiver = sceNetSocket(nullptr, 2, 2, 17);
    const int udp_sender = sceNetSocket(nullptr, 2, 2, 17);
    Require(udp_receiver >= 0 && udp_sender >= 0);
    address = {16, 2, 0, 0, 127, 0, 0, 1};
    Require(sceNetBind_nid_postfix(udp_receiver, address.data(), address.size()) == 0);
    address_size = address.size();
    Require(sceNetGetsockname(udp_receiver, address.data(), &address_size) == 0);
    const char datagram[] = "guest udp loopback";
    char datagram_result[sizeof(datagram)]{};
    Require(sceNetSendto(udp_sender, datagram, sizeof(datagram), 0, address.data(), address.size()) == sizeof(datagram));
    std::array<std::uint8_t, 16> source{};
    address_size = source.size();
    Require(sceNetRecvfrom(udp_receiver, datagram_result, sizeof(datagram_result), 0,
        source.data(), &address_size) == sizeof(datagram_result));
    Require(std::strcmp(datagram, datagram_result) == 0 && source[1] == 2);
    Require(sceNetSocketClose(udp_sender) == 0);
    Require(sceNetSocketClose(udp_receiver) == 0);

    const int resolver = sceNetResolverCreate("guest-sce-net", 0, 0);
    Require(resolver >= 0);
    int resolver_error = -1;
    Require(sceNetResolverGetError(resolver, &resolver_error) == 0 && resolver_error == 0);
    std::array<std::uint8_t, 4> ipv4{};
    Require(sceNetResolverStartNtoa(resolver, "guest-sce-net.invalid", ipv4.data(), 5000000, 1, 0) ==
        static_cast<int>(0x804101E1));
    Require(sceNetResolverGetError(resolver, &resolver_error) == 0 &&
        resolver_error == static_cast<int>(0x804101E1));
    Require(sceNetResolverStartNtoa(resolver, nullptr, ipv4.data(), 5000000, 1, 0) == static_cast<int>(0x80410116));
    Require(sceNetResolverGetError(resolver, &resolver_error) == 0 &&
        resolver_error == static_cast<int>(0x804101E1));
    Require(sceNetResolverStartNtoa(resolver, "localhost", ipv4.data(), 5000000, 1, 0) == 0);
    Require(ipv4[0] == 127);
    Require(sceNetResolverGetError(resolver, &resolver_error) == 0 && resolver_error == 0);
    Require(sceNetResolverGetError(resolver, nullptr) == static_cast<int>(0x80410116) && *sceNetErrnoLoc() == 22);
    Require(sceNetResolverDestroy(resolver) == 0);
    resolver_error = -1;
    Require(sceNetResolverGetError(resolver, &resolver_error) == static_cast<int>(0x80410109) &&
        *sceNetErrnoLoc() == 9 && resolver_error == -1);

    std::array<std::uint8_t, 16> ipv6{};
    Require(sceNetInetPton(28, "::1", ipv6.data()) == 1);
    char text[64]{};
    Require(sceNetInetNtop(28, ipv6.data(), text, sizeof(text)) == text);
    Require(std::strcmp(text, "::1") == 0);

    int state = -1;
    Require(sceNetCtlGetState(&state) == 0);
    Require(state == 0 || state == 3);
}
