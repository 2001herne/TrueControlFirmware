//
// Created by benjaminherne on 3/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_PROFILECREATESCREEN_H
#define TRUECONTROLUSBDESCRIPTORS_PROFILECREATESCREEN_H

#include "Screen.h"
#include "component/Keyboard.h"
#include "ProfileManagement/Manager.h"


namespace TCFW::UI {

    class ProfileCreateScreen : public UIScreen {
    public:
        ProfileCreateScreen(ILI9341_TFT &targetDisplay, ProfileManagement::Manager& profileManager);

    private:
        ProfileManagement::Manager& profileManager;
        COMPONENT::Keyboard keyboard;

        void paintContent() override;

        bool shouldRepaintContent() override;

        void onEvent(ProfileManagement::Button event) override;

        bool needsRepaint = true;
    };

} // TCFW::UI

#endif //TRUECONTROLUSBDESCRIPTORS_PROFILECREATESCREEN_H
