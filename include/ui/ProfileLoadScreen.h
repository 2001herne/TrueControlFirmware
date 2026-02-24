//
// Created by benjaminherne on 5/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_PROFILELOADSCREEN_H
#define TRUECONTROLUSBDESCRIPTORS_PROFILELOADSCREEN_H

#include "Screen.h"
#include "ProfileManagement/Manager.h"

namespace TCFW {
    namespace UI {

        class ProfileLoadScreen: public UIScreen {
        public:
            ProfileLoadScreen(ILI9341_TFT &targetDisplay,
                              ProfileManagement::Manager& manager,
                              std::shared_ptr<ProfileManagement::Profile> profile);

        protected:

            void paintContent() override;

            bool shouldRepaintContent() override;

            void onEvent(ProfileManagement::Button event) override;
        private:
            std::shared_ptr<ProfileManagement::Profile> profile;
            ProfileManagement::Manager& manager;
            bool needsRepaint = true;
        };

    } // UI
} // TCFW

#endif //TRUECONTROLUSBDESCRIPTORS_PROFILELOADSCREEN_H
