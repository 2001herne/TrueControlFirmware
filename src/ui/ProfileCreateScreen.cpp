//
// Created by benjaminherne on 3/02/26.
//

#include <iostream>
#include <utility>
#include "ui/ProfileCreateScreen.h"
#include "ProfileManagement/Manager.h"

namespace TCFW {
    namespace UI {
        ProfileCreateScreen::ProfileCreateScreen(ILI9341_TFT &targetDisplay, ProfileManagement::Manager& profileManager):
                UIScreen(targetDisplay),
                profileManager{profileManager},
                keyboard(display, [this](std::string profileName) {
                    this->profileManager.createProfile(std::move(profileName));
                    this->close();
                }){
            setPrimaryHeader({
                                     .content = "Profile"
                             });
            setSecondaryHeader({
                                       .content = "New..."
                               });

            setNavbarItems({NavbarItem{
                .inputType = NavbarItem::D_PAD,
                .text = "Navigate",
                .states = {true, true, true, true}
            }, NavbarItem{
                .inputType = NavbarItem::GAMEPAD,
                .text = "Back",
                .states = {false, false, true, false}
            }, NavbarItem{
                .inputType = NavbarItem::GAMEPAD,
                .text = "Select",
                .states = {false, true, false, false}
            }});
        }
        void ProfileCreateScreen::paintContent() {
            needsRepaint = false;
            keyboard.draw(0, 41, false);
        }

        bool ProfileCreateScreen::shouldRepaintContent() {
            return needsRepaint;
        }

        void ProfileCreateScreen::onEvent(ProfileManagement::Button event) {
            if (event.buttonType == ProfileManagement::Button::HAT) {
                needsRepaint = true;
                switch(event.hatButton) {
                    case GAMEPAD_HAT_UP:
                        keyboard.up();
                        break;
                    case GAMEPAD_HAT_DOWN:
                        keyboard.down();
                        break;
                    case GAMEPAD_HAT_LEFT:
                        keyboard.left();
                        break;
                    case GAMEPAD_HAT_RIGHT:
                        keyboard.right();
                        break;
                    default:
                        needsRepaint = false;
                        break;
                }
                return;
            }
            else {
                if (event.gamepadButton == GAMEPAD_BUTTON_EAST) {
                    close();
                }
                else if (event.gamepadButton == GAMEPAD_BUTTON_SOUTH) {
                    keyboard.action();
                    needsRepaint = true;
                }
            }
        }
    } // UI
} // TCFW