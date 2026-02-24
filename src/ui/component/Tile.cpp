//
// Created by benjaminherne on 3/02/26.
//

#include "ui/component/Tile.h"

namespace TCFW {
    namespace UI {
        namespace COMPONENT {
            void Tile::userDraw(uint16_t x, uint16_t y, bool selected) {
                auto colour = selected ? ILI9341_TFT::C_RED : 0x6000;

                int bracketLength = std::min(width, height) / 4.0f;
                display.fillRect(x + 3, y + 3, bracketLength, 2, colour);
                display.fillRect(x + width - 3 - bracketLength, y + 3, bracketLength, 2, colour);
                display.fillRect(x + 3, y + 3, 2, bracketLength, colour);
                display.fillRect(x + width - 5, y + 3, 2, bracketLength, colour);
                display.fillRect(x + 3, y + height - 3 - bracketLength, 2, bracketLength, colour);
                display.fillRect(x + width - 5, y + height - 3 - bracketLength, 2, bracketLength, colour);
                display.fillRect(x + 3, y + height - 5, bracketLength, 2, colour);
                display.fillRect(x + width - 3 - bracketLength, y + height - 5, bracketLength, 2, colour);
            }

            void Tile::clear() {
                int x = lastX;
                int y = lastY;
                auto colour = ILI9341_TFT::C_BLACK;

                int bracketLength = std::min(width, height) / 4.0f;
                display.fillRect(x + 3, y + 3, bracketLength, 2, colour);
                display.fillRect(x + width - 3 - bracketLength, y + 3, bracketLength, 2, colour);
                display.fillRect(x + 3, y + 3, 2, bracketLength, colour);
                display.fillRect(x + width - 5, y + 3, 2, bracketLength, colour);
                display.fillRect(x + 3, y + height - 3 - bracketLength, 2, bracketLength, colour);
                display.fillRect(x + width - 5, y + height - 3 - bracketLength, 2, bracketLength, colour);
                display.fillRect(x + 3, y + height - 5, bracketLength, 2, colour);
                display.fillRect(x + width - 3 - bracketLength, y + height - 5, bracketLength, 2, colour);
            }

            Tile::Tile(ILI9341_TFT &display, uint16_t width, uint16_t height):
                    display{display}, width{width}, height{height} {}

            void Tile::draw(uint16_t x, uint16_t y, bool selected) {
                lastX = x;
                lastY = y;
                userDraw(x, y, selected);
            }
        } // COMPONENT
    } // UI
} // TCFW