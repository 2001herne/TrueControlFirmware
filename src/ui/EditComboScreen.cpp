//
// Created by benjaminherne on 4/02/26.
//

#include <iostream>
#include <algorithm>
#include "ui/EditComboScreen.h"

namespace TCFW {
    namespace UI {

        EditComboScreen::EditComboScreen(ILI9341_TFT &targetDisplay,
                                         std::shared_ptr<ProfileManagement::Profile> profile,
                                         ProfileManagement::Button button,
                                         size_t comboIndex)
                :UIScreen(targetDisplay),  profile {profile}, button {button}, comboIndex {comboIndex} {
            setPrimaryHeader({
                EditComboScreen::profile->name
            });
            setSecondaryHeader({"Edit " + std::to_string(comboIndex)});

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

            setNavbarItems({NavbarItem{
                NavbarItem::D_PAD,
                "Navigate",
                {true, true, true, true}
            },NavbarItem{
              NavbarItem::GAMEPAD,
              "Back",
              {false, false, true, false}
            }, NavbarItem{
                NavbarItem::GAMEPAD,
                "Toggle",
                {false, true, false, false}
            }});

            tiles.emplace_back(display, GAMEPAD_BUTTON_NORTH);
            tiles.emplace_back(display, GAMEPAD_BUTTON_SOUTH);
            tiles.emplace_back(display, GAMEPAD_BUTTON_EAST);
            tiles.emplace_back(display, GAMEPAD_BUTTON_WEST);

            tiles.emplace_back(display, GAMEPAD_HAT_UP);
            tiles.emplace_back(display, GAMEPAD_HAT_DOWN);
            tiles.emplace_back(display, GAMEPAD_HAT_LEFT);
            tiles.emplace_back(display, GAMEPAD_HAT_RIGHT);

            tiles.emplace_back(display, GAMEPAD_BUTTON_TL);
            tiles.emplace_back(display, GAMEPAD_BUTTON_TR);
            tiles.emplace_back(display, GAMEPAD_BUTTON_TL2);
            tiles.emplace_back(display, GAMEPAD_BUTTON_TR2);

            tiles.emplace_back(display, GAMEPAD_BUTTON_THUMBL);
            tiles.emplace_back(display, GAMEPAD_BUTTON_THUMBR);

        }

        void EditComboScreen::paintContent() {
            needsRepaint = false;
            std::cout << "Repainting" << std::endl;
            int xoff = 0;
            int yoff = 0;
            for (int i = 0; i < tiles.size(); i++) {
                auto& tile = tiles[i];
                auto mapping = std::find_if(profile->mapping.begin(), profile->mapping.end(), [&button = button](auto elem) {
                    return elem.button == button;
                });
                auto combo = mapping->onPress[comboIndex].combo;
                bool isInCombo = std::find_if(combo.begin(), combo.end(), [&tile](std::shared_ptr<ProfileManagement::Event> elem) {
                    if (elem->eventType == ProfileManagement::Event::BUTTON_PRESS || elem->eventType == ProfileManagement::Event::BUTTON_RELEASE) {
                        auto buttonEvent = std::reinterpret_pointer_cast<ProfileManagement::ButtonEvent>(elem);
                        return buttonEvent->button == tile.button;
                    }
                    return false;
                }) != combo.end();
                std::cout << static_cast<std::string>(tile.button) << " is " << (isInCombo ? "" : "not") << " in the combo." << std::endl;
                tile.active = isInCombo;
                tile.draw(xoff, 41 + yoff, selectedTileIndex == i);
                xoff += tile.width;
                if (xoff + tile.width > 320) {
                    xoff = 0;
                    yoff += tile.height;
                }
            }
        }

        bool EditComboScreen::shouldRepaintContent() {
            return needsRepaint;
        }

        void EditComboScreen::onEvent(ProfileManagement::Button event) {
            switch (event.buttonType) {
                case ProfileManagement::Button::HAT:
                    needsRepaint = true;
                    switch (event.hatButton) {
                        case GAMEPAD_HAT_UP:
                            if (selectedTileIndex < 8) needsRepaint = false;
                            else selectedTileIndex -= 8;
                            break;
                        case GAMEPAD_HAT_DOWN:
                            if (selectedTileIndex + 8 > tiles.size() - 1 && selectedTileIndex >= (tiles.size() / 8) * 8) {
                                needsRepaint = false;
                            } else {
                                selectedTileIndex = std::min((int)tiles.size() - 1, selectedTileIndex + 8);
                            }
                            break;
                        case GAMEPAD_HAT_LEFT:
                            selectedTileIndex = std::max((selectedTileIndex/8) * 8, selectedTileIndex - 1);
                            break;
                        case GAMEPAD_HAT_RIGHT:
                            selectedTileIndex = std::min(std::min((selectedTileIndex / 8 + 1) * 8 - 1, selectedTileIndex + 1), (int)(tiles.size() - 1));
                            break;
                        default:
                            break;
                    }
                    break;
                case ProfileManagement::Button::GAMEPAD:
                    if (event.gamepadButton == GAMEPAD_BUTTON_EAST) {
                        close();
                    }
                    if (event.gamepadButton == GAMEPAD_BUTTON_SOUTH) {
                        needsRepaint = true;
                        auto mapping = std::find_if(profile->mapping.begin(), profile->mapping.end(), [&button = button](auto elem) {
                            return elem.button == button;
                        });
                        if (tiles[selectedTileIndex].active) {
                            std::erase_if(mapping->onPress[comboIndex].combo,
                                          [this](const auto& elem) {
                                  if (elem->eventType == ProfileManagement::Event::BUTTON_PRESS || elem->eventType == ProfileManagement::Event::BUTTON_RELEASE) {
                                      auto buttonEvent = std::reinterpret_pointer_cast<ProfileManagement::ButtonEvent>(elem);
                                      return buttonEvent->button == tiles[selectedTileIndex].button;
                                  }
                                  return false;
                            });
                            std::erase_if(mapping->onRelease[comboIndex].combo,
                                          [this](const auto& elem) {
                                              if (elem->eventType == ProfileManagement::Event::BUTTON_PRESS || elem->eventType == ProfileManagement::Event::BUTTON_RELEASE) {
                                                  auto buttonEvent = std::reinterpret_pointer_cast<ProfileManagement::ButtonEvent>(elem);
                                                  return buttonEvent->button == tiles[selectedTileIndex].button;
                                              }
                                              return false;
                                          });
                        } else {
                            mapping->onPress[comboIndex].combo.push_back(
                                    std::make_shared<ProfileManagement::ButtonEvent>(
                                            ProfileManagement::Event::BUTTON_PRESS,
                                            tiles[selectedTileIndex].button
                                    )
                            );
                            mapping->onRelease[comboIndex].combo.push_back(
                                    std::make_shared<ProfileManagement::ButtonEvent>(
                                            ProfileManagement::Event::BUTTON_PRESS,
                                            tiles[selectedTileIndex].button
                                    )
                            );
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        void ButtonToggleTile::userDraw(uint16_t x, uint16_t y, bool selected) {
            COMPONENT::Tile::userDraw(x, y, selected);
            auto colour = active ? ILI9341_TFT::C_WHITE :
                                        selected ? ILI9341_TFT::C_RED : (displaylib_16_graphics::pixel_color565_e) 0x6000;
            switch (button.buttonType) {
                case ProfileManagement::Button::HAT:
                    UIScreen::drawDPad(display, x + width/2, y + width/2, 12, {
                        button.hatButton == GAMEPAD_HAT_UP,
                        button.hatButton == GAMEPAD_HAT_DOWN,
                        button.hatButton == GAMEPAD_HAT_RIGHT,
                        button.hatButton == GAMEPAD_HAT_LEFT,
                    }, colour);
                    break;
                case ProfileManagement::Button::GAMEPAD:
                    switch (button.gamepadButton) {
                        case GAMEPAD_BUTTON_NORTH:
                        case GAMEPAD_BUTTON_SOUTH:
                        case GAMEPAD_BUTTON_EAST:
                        case GAMEPAD_BUTTON_WEST:
                            UIScreen::drawGamepad(display, x + width/2, y + width/2, 12, {
                                button.gamepadButton == GAMEPAD_BUTTON_NORTH,
                                button.gamepadButton == GAMEPAD_BUTTON_SOUTH,
                                button.gamepadButton == GAMEPAD_BUTTON_EAST,
                                button.gamepadButton == GAMEPAD_BUTTON_WEST,

                            }, colour);
                            break;
                        case GAMEPAD_BUTTON_TL:
                        case GAMEPAD_BUTTON_TL2:
                        case GAMEPAD_BUTTON_TR:
                        case GAMEPAD_BUTTON_TR2:
                            UIScreen::drawTopButtons(display, x + width/2, y + width/2, 12, {
                                .left_trigger = button.gamepadButton == GAMEPAD_BUTTON_TL2,
                                .right_trigger = button.gamepadButton == GAMEPAD_BUTTON_TR2,
                                .left_shoulder = button.gamepadButton == GAMEPAD_BUTTON_TL,
                                .right_shoulder = button.gamepadButton == GAMEPAD_BUTTON_TR,
                            }, colour);
                            break;
                        case GAMEPAD_BUTTON_THUMBR:
                        case GAMEPAD_BUTTON_THUMBL:
                            UIScreen::drawJoysticks(display, x + width/2, y + width/2, 12, {
                                .left_stick = button.gamepadButton == GAMEPAD_BUTTON_THUMBL,
                                .right_stick = button.gamepadButton == GAMEPAD_BUTTON_THUMBR,
                            }, colour);
                            break;
                        default:
                            break;
                    }
            }
        }

        void ButtonToggleTile::clear() {

        }

        ButtonToggleTile::ButtonToggleTile(ILI9341_TFT &display, ProfileManagement::Button button)
            : Tile(display, 40, 40), button {button} {}
    } // UI
} // TCFW