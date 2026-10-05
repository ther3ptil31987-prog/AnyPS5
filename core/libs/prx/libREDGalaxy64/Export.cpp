#include <cstdint>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

extern "C" {

// CD Projekt RED GOG Galaxy wrapper, bundled with the game as
// modules/libREDGalaxy64.prx (seen in Cyberpunk 2077 PPSA04029).
// Online Galaxy services cannot work under emulation: Init/Shutdown/
// ProcessData are accepted and ignored, interface getters return null
// (all observed engine call sites null-check the result).
// Roles below were reversed from the module's own export bodies:
// Init references clientID/clientSecret/configFilePath, Shutdown
// releases the facade singletons, ProcessData throttles facade
// ProcessData calls, GetErrorString formats Gog/Std exceptions.

APS5_EXPORT("1icN9EO2WSg", redGalaxy64Init);
std::uint64_t APS5_VABI redGalaxy64Init(const void* options) {
    (void)options;
    NotImplemented_nid_no_patch(__func__);
    return 1;
}

APS5_EXPORT("SA7aCTRE5zg", redGalaxy64Shutdown);
void APS5_VABI redGalaxy64Shutdown(void) {
    NotImplemented_nid_no_patch(__func__);
}

APS5_EXPORT("ZVPOpEqpUvo", redGalaxy64ProcessData);
void APS5_VABI redGalaxy64ProcessData(void) {
    NotImplemented_nid_no_patch(__func__);
}

APS5_EXPORT("4trn+7BssaY", redGalaxy64GetErrorString);
const char* APS5_VABI redGalaxy64GetErrorString(void) {
    NotImplemented_nid_no_patch(__func__);
    return "";
}

APS5_EXPORT("DQAUzFVaGl8", redGalaxy64GameServerInit);
std::uint64_t APS5_VABI redGalaxy64GameServerInit(const void* options) {
    (void)options;
    NotImplemented_nid_no_patch(__func__);
    return 1;
}

APS5_EXPORT("GKomji4WVHw", redGalaxy64ProcessGameServerData);
void APS5_VABI redGalaxy64ProcessGameServerData(void) {
    NotImplemented_nid_no_patch(__func__);
}

APS5_EXPORT("36lort-kFp4", redgalaxy_stub_00);
std::uint64_t APS5_VABI redgalaxy_stub_00(std::uint64_t a0, std::uint64_t a1, std::uint64_t a2) {
    (void)a0;
    (void)a1;
    (void)a2;
    NotImplemented_nid_no_patch("36lort-kFp4");
    return 0;
}

APS5_EXPORT("NgDLhCc6F7Y", redgalaxy_stub_01);
std::uint64_t APS5_VABI redgalaxy_stub_01(const void* a0, const char* a1, std::uint64_t a2) {
    (void)a0;
    (void)a1;
    (void)a2;
    NotImplemented_nid_no_patch("NgDLhCc6F7Y");
    return 0;
}

APS5_EXPORT("huJ+beHcWBE", redgalaxy_stub_02);
std::uint64_t APS5_VABI redgalaxy_stub_02(void) {
    NotImplemented_nid_no_patch("huJ+beHcWBE");
    return 0;
}

APS5_EXPORT("sxiZFliOEDU", redgalaxy_stub_03);
std::uint64_t APS5_VABI redgalaxy_stub_03(void) {
    NotImplemented_nid_no_patch("sxiZFliOEDU");
    return 0;
}

APS5_EXPORT("49Bdn1OOJ90", redgalaxy_stub_04);
std::uint64_t APS5_VABI redgalaxy_stub_04(void) {
    NotImplemented_nid_no_patch("49Bdn1OOJ90");
    return 0;
}

APS5_EXPORT("8ahh9DUqZLY", redgalaxy_stub_05);
std::uint64_t APS5_VABI redgalaxy_stub_05(void) {
    NotImplemented_nid_no_patch("8ahh9DUqZLY");
    return 0;
}

APS5_EXPORT("Ard4nEfw5Vs", redgalaxy_stub_06);
std::uint64_t APS5_VABI redgalaxy_stub_06(void) {
    NotImplemented_nid_no_patch("Ard4nEfw5Vs");
    return 0;
}

APS5_EXPORT("AzO2fwK+MUs", redgalaxy_stub_07);
std::uint64_t APS5_VABI redgalaxy_stub_07(void) {
    NotImplemented_nid_no_patch("AzO2fwK+MUs");
    return 0;
}

APS5_EXPORT("HKIwQPq45o4", redgalaxy_stub_08);
std::uint64_t APS5_VABI redgalaxy_stub_08(void) {
    NotImplemented_nid_no_patch("HKIwQPq45o4");
    return 0;
}

APS5_EXPORT("IXRuom5imkM", redgalaxy_stub_09);
std::uint64_t APS5_VABI redgalaxy_stub_09(void) {
    NotImplemented_nid_no_patch("IXRuom5imkM");
    return 0;
}

APS5_EXPORT("LjDqfncevos", redgalaxy_stub_10);
std::uint64_t APS5_VABI redgalaxy_stub_10(void) {
    NotImplemented_nid_no_patch("LjDqfncevos");
    return 0;
}

APS5_EXPORT("Onwg0aGsFhM", redgalaxy_stub_11);
std::uint64_t APS5_VABI redgalaxy_stub_11(void) {
    NotImplemented_nid_no_patch("Onwg0aGsFhM");
    return 0;
}

APS5_EXPORT("oSyqfoYcK40", redgalaxy_stub_12);
std::uint64_t APS5_VABI redgalaxy_stub_12(void) {
    NotImplemented_nid_no_patch("oSyqfoYcK40");
    return 0;
}

APS5_EXPORT("qoS0gqW7RxI", redgalaxy_stub_13);
std::uint64_t APS5_VABI redgalaxy_stub_13(void) {
    NotImplemented_nid_no_patch("qoS0gqW7RxI");
    return 0;
}

APS5_EXPORT("SOtPNNwajSw", redgalaxy_stub_14);
std::uint64_t APS5_VABI redgalaxy_stub_14(void) {
    NotImplemented_nid_no_patch("SOtPNNwajSw");
    return 0;
}

APS5_EXPORT("udfpd9vbE+w", redgalaxy_stub_15);
std::uint64_t APS5_VABI redgalaxy_stub_15(void) {
    NotImplemented_nid_no_patch("udfpd9vbE+w");
    return 0;
}

APS5_EXPORT("vuGujcpcUEk", redgalaxy_stub_16);
std::uint64_t APS5_VABI redgalaxy_stub_16(void) {
    NotImplemented_nid_no_patch("vuGujcpcUEk");
    return 0;
}

APS5_EXPORT("VXUTdiaHbD4", redgalaxy_stub_17);
std::uint64_t APS5_VABI redgalaxy_stub_17(void) {
    NotImplemented_nid_no_patch("VXUTdiaHbD4");
    return 0;
}

APS5_EXPORT("w5-trCio4bQ", redgalaxy_stub_18);
std::uint64_t APS5_VABI redgalaxy_stub_18(void) {
    NotImplemented_nid_no_patch("w5-trCio4bQ");
    return 0;
}

APS5_EXPORT("YpZjREtZb20", redgalaxy_stub_19);
std::uint64_t APS5_VABI redgalaxy_stub_19(void) {
    NotImplemented_nid_no_patch("YpZjREtZb20");
    return 0;
}

APS5_EXPORT("gPJN5c9XCec", redgalaxy_stub_20);
std::uint64_t APS5_VABI redgalaxy_stub_20(void) {
    NotImplemented_nid_no_patch("gPJN5c9XCec");
    return 0;
}

APS5_EXPORT("z4SpWc89no8", redgalaxy_stub_21);
std::uint64_t APS5_VABI redgalaxy_stub_21(void) {
    NotImplemented_nid_no_patch("z4SpWc89no8");
    return 0;
}

APS5_EXPORT("zdzopKdBeU4", redgalaxy_stub_22);
std::uint64_t APS5_VABI redgalaxy_stub_22(void) {
    NotImplemented_nid_no_patch("zdzopKdBeU4");
    return 0;
}

}
