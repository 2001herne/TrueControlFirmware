//
// Created by benjaminherne on 2/02/26.
//

#include <numeric>
#include <algorithm>
#include <iostream>
#include "ui/ProfileSelectionScreen.h"
#include "ui/ProfileCreateScreen.h"
#include "ui/ProfileDeleteScreen.h"
#include "ui/ProfileEditScreen.h"
#include "ui/component/CreateTile.h"
#include "ui/ProfileLoadScreen.h"


namespace TCFW::UI {
    ProfileSelectScreen::ProfileSelectScreen(ILI9341_TFT &targetDisplay, ProfileManagement::Manager &profileManager):
            UIScreen(targetDisplay),
            profileManager {profileManager} {
        setPrimaryHeader({
                                 .content = "Select Profile",
                         });

        updateNavbar();

    }

    void ProfileSelectScreen::paintContent() {
        needsRepaint = false;

        if (needsFullClear) {
            selectedTileIndex = 0;
            oldSelected = 0;
            updateNavbar();
            tiles = {};
            tiles.emplace_back(std::make_shared<COMPONENT::CreateTile>(display, 320, 40));
            for (const auto& profile : profileManager.getProfiles()) {
                if (!profile.second.expired() && profile.second.lock()->id != 0) {
                    tiles.emplace_back(std::make_shared<ConcreteProfileSelectionTile>(display, profile.second));
                }
            }
        }

        size_t firstTile = selectedTileIndex < 3 ? 0 : selectedTileIndex - 2;
        size_t oldFirst = oldSelected < 3 ? 0 : oldSelected - 2;

        //Tile positions haven't changed, don't need to fully clear before drawing
        if (firstTile == oldFirst) {
            for (size_t i = 0; i + firstTile < tiles.size(); i++) {
                tiles[i + firstTile]->draw(0, 41 + 40 * i, (i + firstTile) == selectedTileIndex);
            }
        }
        else {
            // Clear old tiles
            for (size_t i = 0; i <= 3; i++) {
                if (i + oldFirst < tiles.size()) {
                    tiles[i + oldFirst]->clear();
                }
            }

            for (size_t i = 0; i <= 3; i++) {
                // Draw new tile
                if (i + firstTile < tiles.size()) {
                    tiles[i + firstTile]->draw(0, 41 + 40 * i, (i + firstTile) == selectedTileIndex);
                }
                // Clear if nothing done
                else {
                    display.fillRect(0, 41 + 40 * i, 320, 40, ILI9341_TFT::C_BLACK);
                }
            }
        }

        oldSelected = selectedTileIndex;
    }

    bool ProfileSelectScreen::shouldRepaintContent() {
        if (needsRepaint) {return true;}

        return needsRepaint;
    }

    void ProfileSelectScreen::onEvent(ProfileManagement::Button event) {
        if (event.buttonType == ProfileManagement::Button::HAT) {
            needsRepaint = true;
            switch(event.hatButton) {
                case GAMEPAD_HAT_UP:
                    selectedTileIndex = std::max(selectedTileIndex, (size_t)1) - 1;
                    break;
                case GAMEPAD_HAT_DOWN:
                    selectedTileIndex = std::min(selectedTileIndex + 1, tiles.size() - 1);
                    break;
                default:
                    needsRepaint = false;
            }
            updateNavbar();
            return;
        }
        if (event.buttonType == ProfileManagement::Button::GAMEPAD) {
            if (event.gamepadButton == GAMEPAD_BUTTON_SOUTH) {
                if (selectedTileIndex == 0) {
                    UIScreen::pushChild(std::make_shared<ProfileCreateScreen>(display, profileManager));
                }
                else {
                    auto selectedTile = std::reinterpret_pointer_cast<ConcreteProfileSelectionTile>(
                            tiles.at(selectedTileIndex)
                    );
                    UIScreen::pushChild(std::make_shared<ProfileLoadScreen>(display, profileManager, selectedTile->profile.lock()));
                }
            }
            if (event.gamepadButton == GAMEPAD_BUTTON_EAST && selectedTileIndex != 0) {
                auto selectedTile = std::reinterpret_pointer_cast<ConcreteProfileSelectionTile>(tiles[selectedTileIndex]);
                UIScreen::pushChild(std::make_shared<ProfileDeleteScreen>(display, selectedTile->profile, profileManager));
            }
            if (event.gamepadButton == GAMEPAD_BUTTON_WEST && selectedTileIndex != 0) {
                auto selectedTile = std::reinterpret_pointer_cast<ConcreteProfileSelectionTile>(tiles[selectedTileIndex]);
                UIScreen::pushChild(std::make_shared<ProfileEditScreen>(display, selectedTile->profile.lock()));
            }
        }
    }

    void ProfileSelectScreen::updateNavbar() {
        if (selectedTileIndex == 0) {
            setNavbarItems({NavbarItem{
                    .inputType = NavbarItem::D_PAD,
                    .text = "Navigate",
                    .states = {true, true, false, false}
            }, NavbarItem{
                    .inputType = NavbarItem::GAMEPAD,
                    .text = "Create",
                    .states = {false, true, false, false}
            }});
        }
        else {
            setNavbarItems({NavbarItem{
                .inputType = NavbarItem::D_PAD,
                .text = "Navigate",
                .states = {true, true, false, false}
            }, NavbarItem{
                .inputType = NavbarItem::GAMEPAD,
                .text = "Edit",
                .states = {false, false, false, true}
            }, NavbarItem{
                .inputType = NavbarItem::GAMEPAD,
                .text = "Delete",
                .states = {false, false, true, false}
            }, NavbarItem{
                .inputType = NavbarItem::GAMEPAD,
                .text = "Load",
                .states = {false, true, false, false}
            }});
        }
    }

    void ConcreteProfileSelectionTile::userDraw(uint16_t x, uint16_t y, bool selected) {
        Tile::userDraw(x, y, selected);
        std::string name = profile.lock()->name;
        display.setFont(font_retro);
        display.setTextColor(ILI9341_TFT::C_WHITE, ILI9341_TFT::C_BLACK);
        display.setCursor(x + 10, y + (height - 16)/2);
        display.print(name);
        display.fillRect(x + 10 + 8 * name.length(), y + (height - 16)/2,
                         width - (x + 10 + 8 * name.length()), 16,
                         ILI9341_TFT::C_BLACK);
    }

    void ConcreteProfileSelectionTile::clear() {
        if (lastX >= 0 && lastY >= 0) {
            display.setFont(font_retro);
            display.setCursor(lastX + 10, lastY + (height - 16) / 2);
            display.setTextColor(ILI9341_TFT::C_BLACK, ILI9341_TFT::C_BLACK);
            display.print(profile.expired() ? "" : profile.lock()->name);
            Tile::clear();
        }
    }

    ConcreteProfileSelectionTile::ConcreteProfileSelectionTile(ILI9341_TFT &display,
                                                               const std::weak_ptr<ProfileManagement::Profile> &profile)
            : Tile(display, 320, 40), profile(profile) {}
}// TCFW::UI