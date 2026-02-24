//
// Created by benjaminherne on 4/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_CREATETILE_H
#define TRUECONTROLUSBDESCRIPTORS_CREATETILE_H

#include "Tile.h"

namespace TCFW::UI::COMPONENT {
    class CreateTile: public Tile {
    public:
        CreateTile(ILI9341_TFT &display, uint16_t width, uint16_t height);

        void userDraw(uint16_t x, uint16_t y, bool selected) override;

        void clear() override;
    };
}


#endif //TRUECONTROLUSBDESCRIPTORS_CREATETILE_H
