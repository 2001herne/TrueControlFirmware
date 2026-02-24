//
// Created by benjaminherne on 3/02/26.
//

#include <iostream>
#include <utility>
#include <numeric>
#include "ui/ProfileEditScreen.h"
#include "ui/ProfileRenameScreen.h"
#include "ui/EditSequenceScreen.h"

namespace TCFW {
    namespace UI {
        ProfileEditScreen::ProfileEditScreen(ILI9341_TFT &targetDisplay, std::shared_ptr<ProfileManagement::Profile> profile)
                :UIScreen(targetDisplay),
                profile{profile}{
            setPrimaryHeader({profile->name});
            setSecondaryHeader({"Edit"});
            updateNavbar();

            for (const auto& mapping : profile->mapping) {
                tiles.emplace_back(display, mapping);
            }

        }

        void ProfileEditScreen::paintContent() {
            needsRepaint = false;

            if (needsFullClear) {
                std::cout << profile->name << std::endl;
                setPrimaryHeader({profile->name});
                tiles.clear();
                for (const auto& mapping : profile->mapping) {
                    tiles.emplace_back(display, mapping);
                }
            }

            size_t firstTile = selectedTileIndex < 3 ? 0 : selectedTileIndex - 2;
            size_t oldFirst = oldSelected < 3 ? 0 : oldSelected - 2;

            //Tile positions haven't changed, don't need to fully clear before drawing
            if (firstTile == oldFirst) {
                for (size_t i = 0; i + firstTile < tiles.size(); i++) {
                    tiles[i + firstTile].draw(0, 41 + 40 * i, (i + firstTile) == selectedTileIndex);
                }
            }
            else {
                // Clear old tiles
                for (size_t i = 0; i < 4; i++) {
                    if (i + oldFirst < tiles.size()) {
                        tiles[i + oldFirst].clear();
                    }
                }

                for (size_t i = 0; i < 4; i++) {
                    // Draw new tile
                    if (i + firstTile < tiles.size()) {
                        tiles[i + firstTile].draw(0, 41 + 40 * i, (i + firstTile) == selectedTileIndex);
                    }
                        // Clear if nothing done
                    else {
                        display.fillRect(0, 41 + 40 * i, 320, 40, ILI9341_TFT::C_BLACK);
                    }
                }
            }

            oldSelected = selectedTileIndex;
        }

        bool ProfileEditScreen::shouldRepaintContent() {
            return needsRepaint;
        }

        void ProfileEditScreen::onEvent(ProfileManagement::Button event) {
            if (event.buttonType == ProfileManagement::Button::HAT) {
                needsRepaint = true;
                switch(event.hatButton) {
                    case GAMEPAD_HAT_UP:
                        selectedTileIndex = std::max(selectedTileIndex, 1) - 1;
                        break;
                    case GAMEPAD_HAT_DOWN:
                        selectedTileIndex = std::min(selectedTileIndex + 1, (int)tiles.size() - 1);
                        break;
                    default:
                        needsRepaint = false;
                }
            }
            if (event.buttonType == ProfileManagement::Button::GAMEPAD) {
                if (event.gamepadButton == GAMEPAD_BUTTON_WEST) {
                    UIScreen::pushChild(std::make_shared<ProfileRenameScreen>(display, profile));
                }
                if (event.gamepadButton == GAMEPAD_BUTTON_EAST) {
                    close();
                }
                if (event.gamepadButton == GAMEPAD_BUTTON_SOUTH) {
                    UIScreen::pushChild(std::make_shared<EditSequenceScreen>(display, profile, tiles[selectedTileIndex].mapping.button));
                }
            }
        }

        void ProfileEditScreen::updateNavbar() {
            setNavbarItems({NavbarItem{
                NavbarItem::D_PAD,
                "Navigate",
                {true, true, false, false}
            }, NavbarItem{
                NavbarItem::GAMEPAD,
                "Rename",
                {false, false, false, true}
            }, NavbarItem{
                NavbarItem::GAMEPAD,
                "Back",
                {false, false, true, false}
            }, NavbarItem {
                NavbarItem::GAMEPAD,
                "Select",
                {false, true, false, false}
            }});
        }

        void ProfileMappingTile::userDraw(uint16_t x, uint16_t y, bool selected) {
            COMPONENT::Tile::userDraw(x, y, selected);
            displaylib_16_graphics::pixel_color565_e iconColour = ILI9341_TFT::C_WHITE;
            switch(mapping.button.buttonType) {
                case ProfileManagement::Button::HAT:
                    UIScreen::drawDPad(display, x + 20, y + 20, 12, {
                        mapping.button.hatButton == GAMEPAD_HAT_UP,
                        mapping.button.hatButton == GAMEPAD_HAT_DOWN,
                        mapping.button.hatButton == GAMEPAD_HAT_RIGHT,
                        mapping.button.hatButton == GAMEPAD_HAT_LEFT,
                    }, iconColour);
                    break;
                case ProfileManagement::Button::GAMEPAD:
                    switch (mapping.button.gamepadButton) {
                        case GAMEPAD_BUTTON_NORTH:
                        case GAMEPAD_BUTTON_SOUTH:
                        case GAMEPAD_BUTTON_EAST:
                        case GAMEPAD_BUTTON_WEST:
                            UIScreen::drawGamepad(display, x + 20, y + 20, 12, {
                                    mapping.button.gamepadButton == GAMEPAD_BUTTON_NORTH,
                                    mapping.button.gamepadButton == GAMEPAD_BUTTON_SOUTH,
                                    mapping.button.gamepadButton == GAMEPAD_BUTTON_EAST,
                                    mapping.button.gamepadButton == GAMEPAD_BUTTON_WEST,
                            }, iconColour);
                            break;
                        case GAMEPAD_BUTTON_THUMBL:
                        case GAMEPAD_BUTTON_THUMBR:
                            UIScreen::drawJoysticks(display, x + 20, y + 20, 12, {
                                .left_stick = mapping.button.gamepadButton == GAMEPAD_BUTTON_THUMBL,
                                .right_stick = mapping.button.gamepadButton == GAMEPAD_BUTTON_THUMBR
                            }, iconColour);
                            break;
                        case GAMEPAD_BUTTON_TL:
                        case GAMEPAD_BUTTON_TL2:
                        case GAMEPAD_BUTTON_TR:
                        case GAMEPAD_BUTTON_TR2:
                            UIScreen::drawTopButtons(display, x + 20, y + 20, 12, {
                                .left_trigger = mapping.button.gamepadButton == GAMEPAD_BUTTON_TL2,
                                .right_trigger = mapping.button.gamepadButton == GAMEPAD_BUTTON_TR2,
                                .left_shoulder = mapping.button.gamepadButton == GAMEPAD_BUTTON_TL,
                                .right_shoulder = mapping.button.gamepadButton == GAMEPAD_BUTTON_TR,
                            }, iconColour);
                            break;
                        default:
                            auto buttonIndex = std::to_string(std::countr_zero((uint32_t)mapping.button.gamepadButton));
                            display.setCursor(x + 12, y + 12);
                            display.setFont(font_retro);
                            display.setTextColor(ILI9341_TFT::C_WHITE, ILI9341_TFT::C_BLACK);
                            display.print(buttonIndex);
                    }

                    break;
            }

            display.fillTriangle(x + 36, y + 8, x + 36 + 8, y + 20, x + 36, y + 8 + 24, ILI9341_TFT::C_RED);

            std::vector<std::array<StatePack, 4>> toDraw {};
            for (auto sequenceElement : mapping.onPress) {
                StatePack hatStates {};
                StatePack gamepadFaceStates {};
                StatePack triggerShoulderStates {};
                StatePack joystickStates {};
                for (auto event : sequenceElement.combo) {
                    std::shared_ptr<ProfileManagement::ButtonEvent> buttonEvent;
                    switch (event->eventType) {
                        case ProfileManagement::Event::BUTTON_PRESS:
                        case ProfileManagement::Event::BUTTON_RELEASE:
                            buttonEvent = std::reinterpret_pointer_cast<ProfileManagement::ButtonEvent>(event);
                            switch(buttonEvent->button.buttonType) {
                                case ProfileManagement::Button::HAT:
                                    hatStates.north |= buttonEvent->button.hatButton == GAMEPAD_HAT_UP;
                                    hatStates.south |= buttonEvent->button.hatButton == GAMEPAD_HAT_DOWN;
                                    hatStates.east |= buttonEvent->button.hatButton == GAMEPAD_HAT_LEFT;
                                    hatStates.west |= buttonEvent->button.hatButton == GAMEPAD_HAT_RIGHT;
                                    break;
                                case ProfileManagement::Button::GAMEPAD:
                                    gamepadFaceStates.north |= buttonEvent->button.gamepadButton == GAMEPAD_BUTTON_NORTH;
                                    gamepadFaceStates.south |= buttonEvent->button.gamepadButton == GAMEPAD_BUTTON_SOUTH;
                                    gamepadFaceStates.east |= buttonEvent->button.gamepadButton == GAMEPAD_BUTTON_EAST;
                                    gamepadFaceStates.west |= buttonEvent->button.gamepadButton == GAMEPAD_BUTTON_WEST;
                                    triggerShoulderStates.left_shoulder |= buttonEvent->button.gamepadButton == GAMEPAD_BUTTON_TL;
                                    triggerShoulderStates.left_trigger |= buttonEvent->button.gamepadButton == GAMEPAD_BUTTON_TL2;
                                    triggerShoulderStates.right_shoulder |= buttonEvent->button.gamepadButton == GAMEPAD_BUTTON_TR;
                                    triggerShoulderStates.right_trigger |= buttonEvent->button.gamepadButton == GAMEPAD_BUTTON_TR2;
                                    joystickStates.left_stick |= buttonEvent->button.gamepadButton == GAMEPAD_BUTTON_THUMBL;
                                    joystickStates.right_stick |= buttonEvent->button.gamepadButton == GAMEPAD_BUTTON_THUMBR;
                                    break;
                                default:
                                    std::cout << "Got weird button type" << std::endl;
                                    break;
                            }
                            break;
                        default:
                            std::cout << "Got different event than buttons" << std::endl;
                            break;
                    }
                }
                toDraw.push_back({
                    hatStates,
                    gamepadFaceStates,
                    triggerShoulderStates,
                    joystickStates
                });
            }

            int sequenceWidth = 8;
            for (auto sequenceElement : toDraw) {
                sequenceWidth += 32 * (
                    sequenceElement[0] +
                    sequenceElement[1] +
                    sequenceElement[2] +
                    sequenceElement[3]
                );
            }

            int xoff = std::max(48, width - sequenceWidth);
            for (auto combo : toDraw) {
                if (combo[0]) {
                    UIScreen::drawDPad(display, xoff + 16, y + 20, 12, combo[0], iconColour);
                    if ((xoff += 32) >= width - 8) break;
                }
                if (combo[1]) {
                    UIScreen::drawGamepad(display, xoff + 16, y + 20, 12, combo[1], iconColour);
                    if ((xoff += 32) >= width - 8) break;
                }
                if (combo[2]) {
                    UIScreen::drawTopButtons(display, xoff + 16, y + 20, 12, combo[2], iconColour);
                    if ((xoff += 32) >= width - 8) break;
                }
                if (combo[3]) {
                    UIScreen::drawJoysticks(display, xoff + 16, y + 20, 12, combo[3], iconColour);
                    if ((xoff += 32) >= width - 8) break;
                }
                display.drawFastVLine(xoff, y + 8, 24, ILI9341_TFT::C_RED);
            }
        }

        void ProfileMappingTile::clear() {
            COMPONENT::Tile::clear();
            display.fillRect(lastX + 8, lastY + 8, width - 16, 25, ILI9341_TFT::C_BLACK);
        }

        ProfileMappingTile::ProfileMappingTile(ILI9341_TFT &display, ProfileManagement::ButtonMapping mapping)
                :Tile(display, 320, 40), mapping{std::move(mapping)} {}
    } // UI
} // TCFW