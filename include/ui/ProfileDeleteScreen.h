//
// Created by benjaminherne on 3/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_PROFILEDELETESCREEN_H
#define TRUECONTROLUSBDESCRIPTORS_PROFILEDELETESCREEN_H

#include "Screen.h"
#include "ProfileManagement/Manager.h"

namespace TCFW {
    namespace UI {

        class ProfileDeleteScreen: public UIScreen{
        protected:
        public:
            ProfileDeleteScreen(ILI9341_TFT &targetDisplay,
                                std::weak_ptr<ProfileManagement::Profile> profile,
                                ProfileManagement::Manager& profileManager);

        protected:
            void paintContent() override;

            bool shouldRepaintContent() override;

            void onEvent(ProfileManagement::Button event) override;
        private:
            std::weak_ptr<ProfileManagement::Profile> profile;
            ProfileManagement::Manager& profileManager;
            bool needsRepaint = true;
        };

    } // UI
} // TCFW

#endif //TRUECONTROLUSBDESCRIPTORS_PROFILEDELETESCREEN_H
