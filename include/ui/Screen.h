//
// Created by benjaminherne on 1/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_SCREEN_H
#define TRUECONTROLUSBDESCRIPTORS_SCREEN_H

#include <bitset>
#include <memory>
#include <stack>
#include <queue>
#include "displaylib_16/ili9341.hpp"
#include "displaylib_16/displaylib_16_graphics.hpp"
#include "ProfileManagement/Profile.h"

namespace TCFW {
    namespace UI {
        struct Rect {
            uint16_t x;
            uint16_t y;
            uint16_t width;
            uint16_t height;
        };

        struct HeaderElement {
            std::string content;
            displaylib_16_graphics::pixel_color565_e fgColour = ILI9341_TFT::C_WHITE;
            displaylib_16_graphics::pixel_color565_e bgColour = ILI9341_TFT::C_BLACK;
        };

        struct HeaderSymbol {
            uint16_t width = 0;
            std::function<void(ILI9341_TFT &, uint16_t, uint16_t,
                               displaylib_16_graphics::pixel_color565_e,
                               displaylib_16_graphics::pixel_color565_e)> symbolFunc {};
        };

        struct StatePack {
            union {
                bool north;
                bool left_trigger;
                bool left_stick;
            };
            union {
                bool south;
                bool right_trigger;
                bool right_stick;
            };
            union {
                bool east;
                bool left_shoulder;
            };
            union {
                bool west;
                bool right_shoulder;
            };

            bool operator==(const StatePack &rhs) const;

            bool operator!=(const StatePack &rhs) const;

            operator bool() const;
        };

        struct NavbarItem {
            enum inputType {
                GAMEPAD,
                D_PAD
            } inputType;
            std::string text;
            StatePack states;

            bool operator==(const NavbarItem &rhs) const;

            bool operator!=(const NavbarItem &rhs) const;
        };

        class UIScreen {
        public:
            explicit UIScreen(ILI9341_TFT &targetDisplay);
            static void task();
            static void reset();

            static void pushChild(const std::shared_ptr<UIScreen>& childScreen);
            void close();

            static void notifyEvent(ProfileManagement::Button event);

            static void drawDPad(ILI9341_TFT& display, uint16_t x, uint16_t y, uint16_t r, StatePack states,
                                 displaylib_16_graphics::pixel_color565_e fg = ILI9341_TFT::C_RED,
                                 displaylib_16_graphics::pixel_color565_e bg = ILI9341_TFT::C_BLACK);
            static void drawGamepad(ILI9341_TFT& display, uint16_t x, uint16_t y, uint16_t r, StatePack states,
                                 displaylib_16_graphics::pixel_color565_e fg = ILI9341_TFT::C_RED,
                                 displaylib_16_graphics::pixel_color565_e bg = ILI9341_TFT::C_BLACK);
            static void drawTopButtons(ILI9341_TFT& display, uint16_t x, uint16_t y, uint16_t r, StatePack states,
                                 displaylib_16_graphics::pixel_color565_e fg = ILI9341_TFT::C_RED,
                                 displaylib_16_graphics::pixel_color565_e bg = ILI9341_TFT::C_BLACK);
            static void drawJoysticks(ILI9341_TFT& display, uint16_t x, uint16_t y, uint16_t r, StatePack states,
                                 displaylib_16_graphics::pixel_color565_e fg = ILI9341_TFT::C_RED,
                                 displaylib_16_graphics::pixel_color565_e bg = ILI9341_TFT::C_BLACK);

        protected:
            virtual void paintContent() = 0;
            void setPrimaryHeader(const HeaderElement &primaryHeader);
            void setSecondaryHeader(const HeaderElement &secondaryHeader);
            void setHeaderSymbol(uint16_t width, std::function<void(
                    ILI9341_TFT& display,
                    uint16_t x,
                    uint16_t y,
                    displaylib_16_graphics::pixel_color565_e fg,
                    displaylib_16_graphics::pixel_color565_e bg
            )>);
            void setNavbarItems(const std::vector<NavbarItem> &navbarItems);
            virtual bool shouldRepaintContent() = 0;
            virtual void onEvent(ProfileManagement::Button event) = 0;

            ILI9341_TFT &display;
            bool needsFullClear = true;

            static constexpr uint8_t logo_bitmap[] = {
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xf0, 0x03, 0xff,
                    0xff, 0xff, 0xf0, 0x07, 0xff, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xe0, 0x0f, 0xff, 0xff,
                    0xff, 0xc0, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x1f, 0xf0, 0x00, 0x01, 0xf0, 0x3f,
                    0xf0, 0x00, 0x01, 0xf0, 0x7f, 0xf0, 0x00, 0x03, 0xe0, 0xff, 0xf0, 0x00, 0x03, 0xe1, 0xc0, 0xf0,
                    0x00, 0x07, 0xc3, 0x80, 0xf0, 0x00, 0x07, 0xc7, 0x00, 0xf0, 0x00, 0x0f, 0x8e, 0x00, 0xf0, 0x00,
                    0x0f, 0x9c, 0x00, 0xf0, 0x00, 0x1f, 0x38, 0x00, 0x00, 0x00, 0x1f, 0x78, 0x00, 0x00, 0x00, 0x3e,
                    0xf0, 0x00, 0x00, 0x00, 0x3e, 0xf0, 0x00, 0x00, 0x00, 0x7d, 0xe0, 0x00, 0x00, 0x00, 0x7d, 0xe0,
                    0x00, 0x00, 0x00, 0xf9, 0xe0, 0x07, 0x80, 0x00, 0xf9, 0xe0, 0x0f, 0x00, 0x01, 0xf1, 0xe0, 0x1e,
                    0x00, 0x01, 0xf1, 0xe0, 0x3c, 0x00, 0x03, 0xe1, 0xe0, 0x78, 0x00, 0x03, 0xe1, 0xff, 0xf0, 0x00,
                    0x07, 0xc1, 0xff, 0xe0, 0x00, 0x07, 0xc1, 0xff, 0xc0, 0x00, 0x0f, 0x81, 0xff, 0x80, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            };

            bool shouldClose = false;
        private:
            static std::stack<std::shared_ptr<UIScreen>> uiStack;

            void paintLogo();
            void paintHeader();
            void paintNavbar();
            bool repaintHeader = true;
            bool repaintNavbar = true;

            HeaderElement primaryHeader;
            HeaderElement secondaryHeader;
            HeaderSymbol headerSymbol;
            std::vector<NavbarItem> navbarItems;

        };

    } // UI
} // TCFW

#endif //TRUECONTROLUSBDESCRIPTORS_SCREEN_H
