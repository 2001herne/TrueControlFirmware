//
// Created by benjaminherne on 5/02/26.
//

#include "ui/ProfileLoadScreen.h"
#include "ProfileManagement/Manager.h"

#include <utility>

namespace TCFW {
    namespace UI {
        ProfileLoadScreen::ProfileLoadScreen(ILI9341_TFT &targetDisplay,
                                             ProfileManagement::Manager& manager,
                                             std::shared_ptr<ProfileManagement::Profile> profile)
                :UIScreen(targetDisplay),
                manager {manager},
                profile {std::move(profile)} {
            setPrimaryHeader({"Profile Loaded"});
            manager.loadProfile(ProfileLoadScreen::profile->id);
        }

        void ProfileLoadScreen::paintContent() {
            needsRepaint = false;
            display.setFont(font_inconsola);

            auto cPrintText = [&display = display](std::string text, uint16_t y) {
                int x = (320 - text.length() * 24)/2;
                display.setCursor(x, y);
                display.setTextColor(ILI9341_TFT::C_WHITE, ILI9341_TFT::C_BLACK);
                display.print(text);
            };

            int yoff = (160 - 32 * 3)/2 + 41;
            cPrintText("Profile", yoff);
            cPrintText("\"" + profile->name + "\"", (yoff += 32));
            cPrintText("Loaded", yoff + 32);
        }

        bool ProfileLoadScreen::shouldRepaintContent() {
            return needsRepaint;
        }

        void ProfileLoadScreen::onEvent(ProfileManagement::Button event) {

        }
    } // UI
} // TCFW