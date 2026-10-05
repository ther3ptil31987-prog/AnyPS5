#include "prx/libScePlayerInvitationDialog/libScePlayerInvitationDialog.h"

#include <cstdlib>

namespace {

void expect(std::int32_t actual, std::int32_t expected) {
    if (actual != expected) {
        std::abort();
    }
}

}

int main() {
    using enum PlayerInvitationDialogStatus;
    expect(scePlayerInvitationDialogUpdateStatus(), static_cast<std::int32_t>(None));
    expect(scePlayerInvitationDialogClose(), 0);
    expect(scePlayerInvitationDialogTerminate(), 0);

    expect(scePlayerInvitationDialogInitialize(), 0);
    expect(scePlayerInvitationDialogInitialize(), 0);
    expect(scePlayerInvitationDialogGetStatus(), static_cast<std::int32_t>(Initialized));
    expect(scePlayerInvitationDialogUpdateStatus(), static_cast<std::int32_t>(Initialized));

    ScePlayerInvitationDialogParam param{};
    expect(scePlayerInvitationDialogOpen(&param), 0);
    expect(scePlayerInvitationDialogGetStatus(), static_cast<std::int32_t>(Running));
    expect(scePlayerInvitationDialogUpdateStatus(), static_cast<std::int32_t>(Finished));
    expect(scePlayerInvitationDialogUpdateStatus(), static_cast<std::int32_t>(Finished));
    expect(scePlayerInvitationDialogOpen(nullptr), 0);
    expect(scePlayerInvitationDialogClose(), 0);
    expect(scePlayerInvitationDialogGetStatus(), static_cast<std::int32_t>(Finished));
    expect(scePlayerInvitationDialogTerminate(), 0);
    expect(scePlayerInvitationDialogGetStatus(), static_cast<std::int32_t>(None));

    expect(scePlayerInvitationDialogOpen(nullptr), 0);
    expect(scePlayerInvitationDialogUpdateStatus(), static_cast<std::int32_t>(Finished));
    expect(scePlayerInvitationDialogTerminate(), 0);
}
