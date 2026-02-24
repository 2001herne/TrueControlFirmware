//
// Created by benjaminherne on 4/02/26.
//

#include "ui/EditSequenceScreen.h"
#include "ui/component/CreateTile.h"
#include "ui/EditComboScreen.h"

#include <utility>
#include <algorithm>
#include <iostream>

namespace TCFW {
    namespace UI {
        EditSequenceScreen::EditSequenceScreen(ILI9341_TFT &targetDisplay,
                                               std::shared_ptr<ProfileManagement::Profile> profile,
                                               ProfileManagement::Button button)
                :UIScreen(targetDisplay), profile {std::move(profile)}, button {button} {
            setPrimaryHeader({
                                     EditSequenceScreen::profile->name
                             });
            setSecondaryHeader({"Edit"});

            updateNavbar();

            switch (button.buttonType) {
                case ProfileManagement::Button::HAT:
                    setHeaderSymbol(24, [hatButton = button.hatButton](ILI9341_TFT& display, uint16_t x, uint16_t y,
                                           displaylib_16_graphics::pixel_color565_e fg,
                                           displaylib_16_graphics::pixel_color565_e bg) {
                        UIScreen::drawDPad(display, x, y, 12, {
                            hatButton == GAMEPAD_HAT_UP,
                            hatButton == GAMEPAD_HAT_DOWN,
                            hatButton == GAMEPAD_HAT_RIGHT,
                            hatButton == GAMEPAD_HAT_LEFT
                        }, fg, bg);
                    });
                    break;
                case ProfileManagement::Button::GAMEPAD:
                    switch (button.gamepadButton) {
                        case GAMEPAD_BUTTON_NORTH:
                        case GAMEPAD_BUTTON_SOUTH:
                        case GAMEPAD_BUTTON_EAST:
                        case GAMEPAD_BUTTON_WEST:
                            setHeaderSymbol(24, [gamepadButton = button.gamepadButton](ILI9341_TFT& display, uint16_t x, uint16_t y,
                                                                                       displaylib_16_graphics::pixel_color565_e fg,
                                                                                       displaylib_16_graphics::pixel_color565_e bg) {
                                UIScreen::drawGamepad(display, x, y, 12, {
                                        gamepadButton == GAMEPAD_BUTTON_NORTH,
                                        gamepadButton == GAMEPAD_BUTTON_SOUTH,
                                        gamepadButton == GAMEPAD_BUTTON_EAST,
                                        gamepadButton == GAMEPAD_BUTTON_WEST
                                }, fg, bg);
                            });
                            break;
                        case GAMEPAD_BUTTON_THUMBL:
                        case GAMEPAD_BUTTON_THUMBR:
                            setHeaderSymbol(24, [stick = button.gamepadButton](ILI9341_TFT& display, uint16_t x, uint16_t y,
                                                                           displaylib_16_graphics::pixel_color565_e fg,
                                                                           displaylib_16_graphics::pixel_color565_e bg) {
                                UIScreen::drawDPad(display, x, y, 12, {
                                        .left_stick = stick == GAMEPAD_BUTTON_THUMBL,
                                        .right_stick = stick == GAMEPAD_BUTTON_THUMBR,
                                }, fg, bg);
                            });
                            break;
                        case GAMEPAD_BUTTON_TL:
                        case GAMEPAD_BUTTON_TL2:
                        case GAMEPAD_BUTTON_TR:
                        case GAMEPAD_BUTTON_TR2:
                            setHeaderSymbol(24, [gamepadButton = button.gamepadButton](ILI9341_TFT& display, uint16_t x, uint16_t y,
                                                                                       displaylib_16_graphics::pixel_color565_e fg,
                                                                                       displaylib_16_graphics::pixel_color565_e bg) {
                                UIScreen::drawGamepad(display, x, y, 12, {
                                        .left_trigger = gamepadButton == GAMEPAD_BUTTON_TL2,
                                        .right_trigger = gamepadButton == GAMEPAD_BUTTON_TR2,
                                        .left_shoulder = gamepadButton == GAMEPAD_BUTTON_TL,
                                        .right_shoulder = gamepadButton == GAMEPAD_BUTTON_TR
                                }, fg, bg);
                            });
                            break;
                    }
                    break;

            }
        }

        void EditSequenceScreen::paintContent() {
            needsRepaint = false;

            if (needsFullClear) {
                tiles = {};
                auto mapping = std::find_if(EditSequenceScreen::profile->mapping.begin(),
                                            EditSequenceScreen::profile->mapping.end(), [&button = button](auto& elem) {
                            return elem.button == button;
                        });

                if (mapping != EditSequenceScreen::profile->mapping.end()) {
                    mapping->onRelease = {};

                    for (auto& elem : mapping->onPress) {
                        ProfileManagement::EventSequenceElement releaseElement {};
                        for (auto event : elem.combo) {
                            if (event->eventType == ProfileManagement::Event::BUTTON_PRESS || event->eventType == ProfileManagement::Event::BUTTON_RELEASE) {
                                auto buttonEvent = std::make_shared<ProfileManagement::ButtonEvent>(
                                        *std::reinterpret_pointer_cast<ProfileManagement::ButtonEvent>(event)
                                );
                                buttonEvent->eventType = ProfileManagement::Event::BUTTON_RELEASE;
                                releaseElement.combo.push_back(buttonEvent);
                            } else {
                                releaseElement.combo.push_back(std::make_shared<ProfileManagement::Event>(*event));
                            }
                        }
                        releaseElement.delayMs = elem.delayMs;
                        mapping->onRelease.push_back(releaseElement);

                        tiles.push_back(std::make_shared<SequenceElementComboTile>(display, elem.combo));
                        tiles.push_back(std::make_shared<SequenceElementDelayTile>(display, elem.delayMs));
                    }
                }

                tiles.push_back(std::make_shared<COMPONENT::CreateTile>(display, 320, 40));

                updateNavbar();

                oldSelectedTileIndex = selectedTileIndex;
            }

            auto selectedOffset = [&tiles = tiles](int firstTile, int selected) {
                int yoff = 0;
                for (int i = firstTile; i < selected; i++) {
                    yoff += tiles[i]->height;
                }
                return yoff;
            };

            size_t firstTile = 0;
            while (selectedOffset(firstTile, selectedTileIndex) > 80) {
                firstTile--;
            }
            size_t oldFirst = 0;
            while (selectedOffset(oldFirst, oldSelectedTileIndex) > 80) {
                oldFirst--;
            }

            //Tile positions haven't changed, don't need to fully clear before drawing
            if (firstTile == oldFirst) {
                int yoff = 0;
                for (int i = 0; i < tiles.size(); i++) {
                    if (i >= firstTile) {
                        auto tile = tiles[i];
                        if (yoff + tile->height > 160) break;
                        tile->draw(0, 41 + yoff, i == selectedTileIndex);
                        yoff += tile->height;
                    }
                }
            }
            else {
                // Clear old tiles
                int yoff = 0;
                for (int i = 0; i < tiles.size(); i++) {
                    if (i >= oldSelectedTileIndex) {
                        auto tile = tiles[i];
                        if (yoff + tile->height > 160) break;
                        tile->clear();
                        yoff += tile->height;
                    }
                }

                yoff = 0;
                for (int i = 0; i < tiles.size(); i++) {
                    if (i >= firstTile) {
                        auto tile = tiles[i];
                        if (yoff + tile->height > 160) break;
                        tile->draw(0, 41 + yoff, i == selectedTileIndex);
                        yoff += tile->height;
                    }
                }
                display.drawRectWH(0, yoff, 320, 160 - yoff, ILI9341_TFT::C_BLACK);
            }

            oldSelectedTileIndex = selectedTileIndex;
        }

        bool EditSequenceScreen::shouldRepaintContent() {
            return needsRepaint;
        }

        void EditSequenceScreen::onEvent(ProfileManagement::Button event) {
            switch (event.buttonType) {
                case ProfileManagement::Button::HAT:
                    needsRepaint = true;
                    switch (event.hatButton) {
                        case GAMEPAD_HAT_UP:
                            selectedTileIndex = std::max(0, selectedTileIndex - 1);
                            updateNavbar();
                            break;
                        case GAMEPAD_HAT_DOWN:
                            selectedTileIndex = std::min((int)tiles.size() - 1, selectedTileIndex + 1);
                            updateNavbar();
                            break;
                        case GAMEPAD_HAT_LEFT:
                            std::cout << selectedTileIndex % 2 << std::endl;
                            if (selectedTileIndex % 2 == 1) {
                                auto mapping = std::find_if(profile->mapping.begin(), profile->mapping.end(), [&button = button](auto elem) {
                                    return elem.button == button;
                                });
                                mapping->onPress[selectedTileIndex/2].delayMs = std::max(mapping->onPress[selectedTileIndex/2].delayMs, 10) - 10;
                                mapping->onRelease[selectedTileIndex/2].delayMs = std::max(mapping->onRelease[selectedTileIndex/2].delayMs, 10) - 10;
                                std::reinterpret_pointer_cast<SequenceElementDelayTile>(tiles[selectedTileIndex])->updateDelay(mapping->onPress[selectedTileIndex/2].delayMs);
                            } else needsRepaint = false;
                            break;
                        case GAMEPAD_HAT_RIGHT:
                            if (selectedTileIndex % 2 == 1) {
                                auto mapping = std::find_if(profile->mapping.begin(), profile->mapping.end(), [button = button](auto elem) {
                                    return elem.button == button;
                                });
                                mapping->onPress[selectedTileIndex/2].delayMs += 10;
                                mapping->onRelease[selectedTileIndex/2].delayMs += 10;
                                std::reinterpret_pointer_cast<SequenceElementDelayTile>(tiles[selectedTileIndex])->updateDelay(mapping->onPress[selectedTileIndex/2].delayMs);
                            } else needsRepaint = false;
                            break;
                        default:
                            break;

                    }
                case ProfileManagement::Button::GAMEPAD:
                    switch (event.gamepadButton) {
                        case GAMEPAD_BUTTON_EAST:
                            close();
                            break;
                        case GAMEPAD_BUTTON_WEST:
                            if (selectedTileIndex % 2 == 0 && selectedTileIndex != tiles.size() - 1) {
                                auto mapping = std::find_if(profile->mapping.begin(), profile->mapping.end(), [&button = button](auto elem) {
                                    return elem.button == button;
                                });
                                mapping->onPress.erase(mapping->onPress.begin() + selectedTileIndex/2);
                                mapping->onRelease.erase(mapping->onRelease.begin() + selectedTileIndex/2);
                                needsFullClear = true;
                            }
                            break;
                        case GAMEPAD_BUTTON_SOUTH:
                            if (selectedTileIndex == tiles.size() - 1) {
                                auto mapping = std::find_if(profile->mapping.begin(), profile->mapping.end(), [&button = button](auto elem) {
                                    return elem.button == button;
                                });
                                mapping->onPress.emplace_back();
                                mapping->onRelease.emplace_back();
                            }
                            if (selectedTileIndex % 2 == 0) {
                                UIScreen::pushChild(std::make_shared<EditComboScreen>(display, profile, button, selectedTileIndex/2));
                            }
                        default:
                            break;
                    }
            }
        }

        void EditSequenceScreen::updateNavbar() {
            if (selectedTileIndex == tiles.size() - 1) {
                setNavbarItems({NavbarItem{
                        NavbarItem::D_PAD,
                        "Navigate",
                        {true, true, false, false}
                }, NavbarItem{
                        NavbarItem::GAMEPAD,
                        "Create Combo",
                        {false, true, false, false}
                }});
                return;
            }
            if (selectedTileIndex % 2 == 0) {
                setNavbarItems({NavbarItem{
                        NavbarItem::D_PAD,
                        "Navigate",
                        {true, true, false, false}
                }, NavbarItem{
                        NavbarItem::GAMEPAD,
                        "Delete",
                        {false, false, false, true}
                }, NavbarItem{
                        NavbarItem::GAMEPAD,
                        "Back",
                        {false, false, true, false}
                }, NavbarItem{
                        NavbarItem::GAMEPAD,
                        "Edit",
                        {false, true, false, false}
                }});
            }
            else {
                setNavbarItems({NavbarItem{
                        NavbarItem::D_PAD,
                        "Navigate",
                        {true, true, false, false}
                }, NavbarItem{
                        NavbarItem::D_PAD,
                        "Decrease/Increase",
                        {false, false, true, true}
                }});
            }
        }



        void SequenceElementComboTile::userDraw(uint16_t x, uint16_t y, bool selected) {
            COMPONENT::Tile::userDraw(x, y, selected);

            StatePack hatStates {};
            StatePack gamepadFaceStates {};
            StatePack triggerShoulderStates {};
            StatePack joystickStates {};
            for (auto event : combo) {
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
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }

            int sequenceWidth = 32 * (
                    gamepadFaceStates +
                    hatStates +
                    triggerShoulderStates +
                    joystickStates
            );

            int xoff = (width - sequenceWidth)/2;
            if (hatStates) {
                UIScreen::drawDPad(display, xoff + 16, y + 20, 12, hatStates);
                xoff += 32;
            }
            if (gamepadFaceStates) {
                UIScreen::drawGamepad(display, xoff + 16, y + 20, 12, gamepadFaceStates);
                xoff += 32;
            }
            if (triggerShoulderStates) {
                UIScreen::drawTopButtons(display, xoff + 16, y + 20, 12, triggerShoulderStates);
                xoff += 32;
            }
            if (joystickStates) {
                UIScreen::drawJoysticks(display, xoff + 16, y + 20, 12, joystickStates);
                xoff += 32;
            }
        }

        void SequenceElementComboTile::clear() {
            COMPONENT::Tile::clear();
        }

        SequenceElementComboTile::SequenceElementComboTile(ILI9341_TFT &display, ProfileManagement::EventCombo combo)
                : COMPONENT::Tile(display, 320, 40), combo{std::move(combo)} {}

        void SequenceElementDelayTile::userDraw(uint16_t x, uint16_t y, bool selected) {
            std::string text = "- " + std::to_string(delayMs) + " +";
            auto colour = selected ? ILI9341_TFT::C_RED : 0x6000;
            int textWidth = text.length() * 8;
            int sepWidth = (width - textWidth) / 2;
            display.drawFastHLine(x + 10, y + height/2, sepWidth - 20, colour);
            display.drawFastHLine(x + sepWidth + textWidth + 10, y + height/2, sepWidth - 20, colour);
            display.setFont(font_default);
            display.setCursor(x + sepWidth, y + (height - 8)/2),
            display.print(text);
        }

        void SequenceElementDelayTile::clear() {
            std::string text = "+ " + std::to_string(delayMs) + " -";
            auto colour = ILI9341_TFT::C_BLACK;
            int textWidth = text.length() * 8;
            int sepWidth = (width - textWidth) / 2;
            display.drawFastHLine(lastX + 10, lastY + height/2, sepWidth - 20, colour);
            display.drawFastHLine(lastX + sepWidth + textWidth + 10, lastY + height/2, sepWidth - 20, colour);
            display.setFont(font_default);
            display.setCursor(lastX + sepWidth, lastY + (height - 8)/2),
                    display.print(text);
        }

        SequenceElementDelayTile::SequenceElementDelayTile(ILI9341_TFT &display, uint16_t delayMs)
                : Tile(display, 320, 16), delayMs{delayMs} {}

        void SequenceElementDelayTile::updateDelay(uint16_t newDelay) {
            clear();
            delayMs = newDelay;
        }
    } // UI
} // TCFW