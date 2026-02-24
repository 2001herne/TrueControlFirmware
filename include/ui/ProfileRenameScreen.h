//
// Created by benjaminherne on 3/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_PROFILERENAMESCREEN_H
#define TRUECONTROLUSBDESCRIPTORS_PROFILERENAMESCREEN_H

#include "Screen.h"
#include "ui/component/Keyboard.h"

namespace TCFW::UI {
    class ProfileRenameScreen : public UIScreen {
    public:
        ProfileRenameScreen(ILI9341_TFT &targetDisplay, std::shared_ptr<ProfileManagement::Profile> profile);

    protected:
        void paintContent() override;

        bool shouldRepaintContent() override;

        void onEvent(ProfileManagement::Button event) override;

    private:
        COMPONENT::Keyboard keyboard;
        std::shared_ptr<ProfileManagement::Profile> profile;
        bool needsRepaint = true;
    };
}


#endif //TRUECONTROLUSBDESCRIPTORS_PROFILERENAMESCREEN_H
