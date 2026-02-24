//
// Created by benjaminherne on 3/02/26.
//

#include <iostream>
#include "ui/ProfileRenameScreen.h"

namespace TCFW::UI {

    ProfileRenameScreen::ProfileRenameScreen(ILI9341_TFT &targetDisplay, std::shared_ptr<ProfileManagement::Profile> profile)
            : UIScreen(targetDisplay),
              keyboard(display, [this](auto content){
                  std::cout << this->profile->name << ' ';
                  this->profile->name = content;
                  std::cout << this->profile->name << std::endl;
                  close();
              }, profile->name),
              profile{profile}{
        setPrimaryHeader({profile->name});
        setSecondaryHeader({"Rename"});
        setNavbarItems({NavbarItem{
            NavbarItem::D_PAD,
            "Navigate",
            {true, true, true, true}
        }, NavbarItem{
            NavbarItem::GAMEPAD,
            "Back",
            {false, false, true, false}
        }, NavbarItem{
            NavbarItem::GAMEPAD,
            "Select",
            {false, true, false, false}
        }});
    }

    void ProfileRenameScreen::paintContent() {
        needsRepaint = false;
        keyboard.draw(0, 41, false);
    }

    bool ProfileRenameScreen::shouldRepaintContent() {
        return needsRepaint;
    }

    void ProfileRenameScreen::onEvent(ProfileManagement::Button event) {
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
}