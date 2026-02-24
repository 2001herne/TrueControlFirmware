//
// Created by benjaminherne on 1/02/26.
//

#include "ui/Screen.h"

#include <algorithm>
#include <numeric>
#include <iostream>

namespace TCFW {
    namespace UI {
        std::stack<std::shared_ptr<UIScreen>> UIScreen::uiStack {};

        void UIScreen::reset() {
            uiStack = {};
        }

        void UIScreen::task() {
            if (!uiStack.empty()) {
                if (uiStack.top()->shouldClose) {
                    uiStack.pop();
                    uiStack.top()->needsFullClear = true;
                    uiStack.top()->repaintHeader = true;
                    uiStack.top()->repaintNavbar = true;
                }
                else {
                    uiStack.top()->paintHeader();
                    if (uiStack.top()->shouldRepaintContent() || uiStack.top()->needsFullClear) {
                        if (uiStack.top()->needsFullClear) {
                            uiStack.top()->display.fillRect(0, 41, 320, 240-80, ILI9341_TFT::C_BLACK);
                            uiStack.top()->paintContent();
                            uiStack.top()->needsFullClear = false;
                        } else {
                            uiStack.top()->paintContent();
                        }
                        uiStack.top()->repaintNavbar = true;
                    }
                    uiStack.top()->paintNavbar();
                }
            }
        }

        void UIScreen::paintLogo() {
            display.drawBitmap(0, 0, 40, 40, ILI9341_TFT::C_RED, ILI9341_TFT::C_BLACK, UIScreen::logo_bitmap);
        }

        void UIScreen::close() {
            shouldClose = true;
        }

        UIScreen::UIScreen(ILI9341_TFT &targetDisplay): display {targetDisplay} {}

        void UIScreen::pushChild(const std::shared_ptr<UIScreen>& childScreen) {
            uiStack.push(childScreen);
        }

        void UIScreen::paintHeader() {
            if (repaintHeader) {
                paintLogo();
                uint primaryChars = primaryHeader.content.length();
                uint secondaryChars = secondaryHeader.content.length();
                auto primaryWidth = [&primaryChars]() { return primaryChars * 16; };
                auto secondaryWidth = [&secondaryChars]() { return secondaryChars * 16; };
                auto totalWidth = [&primaryWidth, &secondaryWidth, &headerSymbol = headerSymbol]() {
                    std::cout << headerSymbol.width << " " << primaryWidth() + secondaryWidth() + 16 + headerSymbol.width << std::endl;
                    return primaryWidth() + secondaryWidth() + 16 + headerSymbol.width;
                };
                uint headerWidth = 320 - 40;
                while (totalWidth() > headerWidth && primaryChars > 3) {
                    primaryChars--;
                }

                while (totalWidth() > headerWidth && secondaryChars > 3) {
                    secondaryChars--;
                }

                display.fillRect(40, 0, headerWidth, 40, ILI9341_TFT::C_BLACK);
                display.setFont(font_orla);
                if (!secondaryHeader.content.empty()) {
                    display.fillRect(42 + primaryWidth(), 0, 278 - primaryWidth(), 40, secondaryHeader.bgColour);
                    display.setCursor(56 + primaryWidth(), 8);
                    display.setTextColor(secondaryHeader.fgColour, secondaryHeader.bgColour);
                    display.print(secondaryHeader.content.substr(0, secondaryChars));
                }
                display.fillRect(40, 0, primaryWidth(), 40, primaryHeader.bgColour);
                display.setCursor(40, 8);
                display.setTextColor(primaryHeader.fgColour, primaryHeader.bgColour);
                display.print(primaryHeader.content.substr(0, primaryChars));

                if (!secondaryHeader.content.empty()) {
                    display.fillTriangle(42 + primaryWidth(), 0, 54 + primaryWidth(), 20, 42 + primaryWidth(), 40,
                                          primaryHeader.bgColour);
                    constexpr int8_t sepWidth = 2;
                    display.fillQuadrilateral(42 + primaryWidth()           , 0,
                                               42 + primaryWidth() + sepWidth, 0,
                                               54 + primaryWidth()           , 20,
                                               54 + primaryWidth() - sepWidth, 20,
                                               ILI9341_TFT::C_RED);
                    display.fillQuadrilateral(42 + primaryWidth()           , 40,
                                               42 + primaryWidth() + sepWidth, 40,
                                               54 + primaryWidth()           , 20,
                                               54 + primaryWidth() - sepWidth, 20,
                                               ILI9341_TFT::C_RED);
                }

                if (headerSymbol.symbolFunc) {
                    headerSymbol.symbolFunc(display, 320 -  (headerSymbol.width/2 + 8), 20,
                                            secondaryHeader.fgColour,
                                            secondaryHeader.bgColour);
                }

                display.drawFastHLine(0, 40, 320, ILI9341_TFT::C_RED);
                display.drawFastVLine(40, 0, 40, ILI9341_TFT::C_RED);

                repaintHeader = false;
            }
        }

        void UIScreen::setPrimaryHeader(const HeaderElement &primaryHeader) {
            repaintHeader = true;
            UIScreen::primaryHeader = primaryHeader;
        }

        void UIScreen::setSecondaryHeader(const HeaderElement &secondaryHeader) {
            repaintHeader = true;
            UIScreen::secondaryHeader = secondaryHeader;
        }

        void UIScreen::setNavbarItems(const std::vector<NavbarItem> &navbarItems) {
            if (navbarItems != UIScreen::navbarItems) {
                repaintNavbar = true;
                UIScreen::navbarItems = navbarItems;
            }
        }

        void UIScreen::paintNavbar() {
            if (repaintNavbar) {
                display.fillRect(0, 202, 320, 40, ILI9341_TFT::C_BLACK);

                std::vector<uint8_t> displayLengths(navbarItems.size());
                std::transform(navbarItems.begin(), navbarItems.end(), displayLengths.begin(), [](const NavbarItem& elem)->uint8_t {
                    return elem.text.length();
                });

                auto totalWidth = [&displayLengths]() {
                    return std::accumulate(displayLengths.begin(), displayLengths.end(), 4 * displayLengths.size()) * 8;
                };

                while (totalWidth() > 320) {
                    auto maxElement = std::max_element(displayLengths.begin(), displayLengths.end());
                    (*maxElement)--;
                }

                int spacing = (320 - totalWidth()) / (float)(displayLengths.size() + 1);

                display.setFont(font_retro);
                display.setTextColor(ILI9341_TFT::C_WHITE, ILI9341_TFT::C_BLACK);
                int offset = spacing;
                for (size_t i = 0; i < displayLengths.size(); i++) {
                    const NavbarItem& item = navbarItems[i];
                    switch (item.inputType) {
                        case NavbarItem::GAMEPAD:
                            UIScreen::drawGamepad(display,offset + 12, 220, 12, item.states);
                            break;
                        case NavbarItem::D_PAD:
                            UIScreen::drawDPad(display, offset + 12, 220, 12, item.states);
                            break;
                    }
                    display.setCursor(offset + 28, 212);
                    display.print(item.text.substr(0, displayLengths[i]));
                    offset += spacing + (displayLengths[i] + 4) * 8;
                }

                display.drawFastHLine(0, 201, 320, ILI9341_TFT::C_RED);

                repaintNavbar = false;
            }
        }

        void UIScreen::notifyEvent(ProfileManagement::Button event) {
            uiStack.top()->onEvent(event);
        }

        void UIScreen::drawDPad(ILI9341_TFT &display, uint16_t x, uint16_t y, uint16_t r, StatePack states,
                                displaylib_16_graphics::pixel_color565_e fg,
                                displaylib_16_graphics::pixel_color565_e bg) {
            display.drawRectWH(x - r/4.f, y-r, r/2.f, 2 * r, fg);
            display.drawRectWH(x - r, y-r/4.f, (2 * r), r/2.f, fg);
            display.drawRectWH(x-r/4.f, y-r/4.f, r/2.f, r/2.f, bg);

            if (states.north) {
                display.fillRect(x - r/4.f, y-r, r/2.f, (3 * r)/4.f, fg);
            }
            if (states.south) {
                display.fillRect(x - r/4.f, y + r/4.f, r/2.f, (3 * r)/4.f, fg);
            }
            if (states.east) {
                display.fillRect(x + r/4.f, y-r/4.f, (r*3)/4.f, r/2.f, fg);
            }
            if (states.west) {
                display.fillRect(x-r, y-r/4.f, (r*3)/4.f, r/2.f, fg);
            }
        }

        void UIScreen::drawGamepad(ILI9341_TFT &display, uint16_t x, uint16_t y, uint16_t r, StatePack states,
                                   displaylib_16_graphics::pixel_color565_e fg,
                                   displaylib_16_graphics::pixel_color565_e bg) {
            if (states.north) display.fillCircle(x, y - (2*r)/3.f, r/3.f, fg);
            else display.drawCircle(x, y - (2*r)/3.f, r/3.f, fg);

            if (states.south) display.fillCircle(x, y + (2*r)/3.f, r/3.f, fg);
            else display.drawCircle(x, y + (2*r)/3.f, r/3.f, fg);

            if (states.east) display.fillCircle(x + (2*r)/3.f, y, r/3.f, fg);
            else display.drawCircle(x + (2*r)/3.f, y, r/3.f, fg);

            if (states.west) display.fillCircle(x - (2*r)/3.f, y, r/3.f, fg);
            else display.drawCircle(x - (2*r)/3.f, y, r/3.f, fg);
        }

        void UIScreen::drawTopButtons(ILI9341_TFT &display, uint16_t x, uint16_t y, uint16_t r, StatePack states,
                                      displaylib_16_graphics::pixel_color565_e fg,
                                      displaylib_16_graphics::pixel_color565_e bg) {
            y += (3 * r)/16;
            if (states.left_shoulder) display.fillRect(x - r, y + r/8, (7*r)/8, r/2, fg);
            else display.drawRectWH(x - r, y + r/8, (7*r)/8, r/2, fg);

            if (states.right_shoulder) display.fillRect(x + r/8, y + r/8, (7 * r)/8, r/2, fg);
            else display.drawRectWH(x + r/8, y + r/8, (7 * r)/8, r/2, fg);

            if (states.left_trigger) display.fillRect(x - (5 * r)/8, y - r, r/2, (7 * r)/8, fg);
            else display.drawRectWH(x - (5 * r)/8, y - r, r/2, (7 * r)/8, fg);

            if (states.right_trigger) display.fillRect(x + r/8, y-r, r/2, (7*r)/8, fg);
            else display.drawRectWH(x + r/8, y-r, r/2, (7*r)/8, fg);
        }

        void UIScreen::drawJoysticks(ILI9341_TFT &display, uint16_t x, uint16_t y, uint16_t r, StatePack states,
                                     displaylib_16_graphics::pixel_color565_e fg,
                                     displaylib_16_graphics::pixel_color565_e bg) {
            uint16_t base = y + r;
            auto drawStick = [&display, r, y, base, fg](bool state, uint16_t x) {
                if (state) {
                    display.fillQuadrilateral(x - r/2, base,
                                              x - r/8, base - r/4,
                                              x + r/8, base - r/4,
                                              x + r/2, base,
                                              fg);
                    display.fillRect(x - r/8, y, r/4, (3 * r)/4, fg);
                    display.fillCircle(x, y - r/4, r/4, fg);
                } else {
                    display.drawLine(x - r / 2, base, x - r / 8, base - r / 4, fg);
                    display.drawFastVLine(x - r / 8, y - r/2, (5 * r)/4, fg);
                    display.drawFastVLine(x + r / 8, y - r/2, (5 * r)/4, fg);
                    display.drawLine(x + r / 2, base, x + r / 8, base - r / 4, fg);
                    display.drawCircle(x, y - (3 * r)/4, r/4, fg);
                }
            };
            drawStick(states.left_stick, x - r/2);
            drawStick(states.right_stick, x + r/2);
        }

        void UIScreen::setHeaderSymbol(uint16_t width,
                                       std::function<void(ILI9341_TFT &, uint16_t, uint16_t,
                                                          displaylib_16_graphics::pixel_color565_e,
                                                          displaylib_16_graphics::pixel_color565_e)> drawFunc) {
            headerSymbol = {
                    width,
                    drawFunc
            };
        }

        bool NavbarItem::operator==(const NavbarItem &rhs) const {
            return inputType == rhs.inputType &&
                   text == rhs.text &&
                   states == rhs.states;
        }

        bool NavbarItem::operator!=(const NavbarItem &rhs) const {
            return !(rhs == *this);
        }

        bool StatePack::operator==(const StatePack &rhs) const {
            return north == rhs.north &&
                   south == rhs.south &&
                   east == rhs.east &&
                   west == rhs.west;
        }

        bool StatePack::operator!=(const StatePack &rhs) const {
            return !(rhs == *this);
        }

        StatePack::operator bool() const {
            return north || south || east || west;
        }
    } // UI
} // TCFW