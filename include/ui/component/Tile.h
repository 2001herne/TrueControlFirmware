//
// Created by benjaminherne on 3/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_TILE_H
#define TRUECONTROLUSBDESCRIPTORS_TILE_H

#include "displaylib_16/ili9341.hpp"

namespace TCFW {
    namespace UI {
        namespace COMPONENT {

            struct Tile {
                Tile(ILI9341_TFT& display, uint16_t width, uint16_t height);
                ILI9341_TFT& display;
                const uint16_t width;
                const uint16_t height;

                virtual void userDraw(uint16_t x, uint16_t y, bool selected) = 0;
                virtual void clear() = 0;

                void draw(uint16_t x, uint16_t y, bool selected);
                int16_t lastX = -1;
                int16_t lastY = -1;
            };

        } // COMPONENT
    } // UI
} // TCFW

#endif //TRUECONTROLUSBDESCRIPTORS_TILE_H
