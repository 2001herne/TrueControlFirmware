//
// Created by benjaminherne on 3/02/26.
//

#include "ui/ProfileDeleteScreen.h"
#include "ProfileManagement/Profile.h"

namespace TCFW {
    namespace UI {
        ProfileDeleteScreen::ProfileDeleteScreen(ILI9341_TFT &targetDisplay,
                                                 std::weak_ptr<ProfileManagement::Profile> profile,
                                                 ProfileManagement::Manager& profileManager)
                :UIScreen(targetDisplay),
                profile {profile},
                profileManager{profileManager}{
            setPrimaryHeader({
                .content = profile.lock()->name,
            });
            setSecondaryHeader({
                .content = "Delete",
                .bgColour = ILI9341_TFT::C_RED
            });
            setNavbarItems({NavbarItem{
                NavbarItem::GAMEPAD,
                "Cancel",
                {false, false, true, false}
            }, NavbarItem{
                NavbarItem::GAMEPAD,
                "Delete",
                {false, true, false, false}
            }});
        }

        void UI::ProfileDeleteScreen::paintContent() {
            needsRepaint = false;
            auto textwidth = [](std::string text) {
                return 16 * text.length();
            };
            display.setFont(font_orla);
            display.setTextColor(ILI9341_TFT::C_WHITE, ILI9341_TFT::C_BLACK);
            std::string query = "Do you wish";
            display.setCursor((320 - textwidth(query))/2, 3.5 * 24);
            display.print(query);
            query = "to delete";
            display.setCursor((320 - textwidth(query))/2, 4.5 * 24);
            display.print(query);
            display.setCursor((320 - textwidth(profile.lock()->name))/2, 5.5 * 24);
        }

        bool UI::ProfileDeleteScreen::shouldRepaintContent() {
            return needsRepaint;
        }

        void UI::ProfileDeleteScreen::onEvent(ProfileManagement::Button event) {
            if (event.buttonType == ProfileManagement::Button::GAMEPAD) {
                if (event.gamepadButton == GAMEPAD_BUTTON_EAST) {
                    close();
                }
                else if (event.gamepadButton == GAMEPAD_BUTTON_SOUTH) {
                    profileManager.deleteProfile(profile.lock()->id);
                    close();
                }
            }
        }
    } // UI
} // TCFW