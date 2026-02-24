//
// Created by benjaminherne on 4/02/26.
//

#include "ui/component/CreateTile.h"

namespace TCFW::UI::COMPONENT {

    void CreateTile::userDraw(uint16_t x, uint16_t y, bool selected) {
        Tile::userDraw(x, y, selected);
        for (int i = 0; i < 360; i += 30) {
            display.drawArc(x + width/2, y + height/2, (height - 10)/2, 1, i, i + 15, display.C_WHITE);
        }

        display.drawFastVLine(x + width/2, y + (2 * height)/6.f, height/3.f + 1, display.C_WHITE);
        display.drawFastHLine(x + width/2 - height/6.f, y + height/2, height/3.f + 1, display.C_WHITE);
    }

    void CreateTile::clear() {
        Tile::clear();

        for (int i = 0; i < 360; i += 30) {
            display.drawArc(lastX + width/2, lastY + height/2, (height - 10)/2, 1, i, i + 15, display.C_BLACK);
        }

        display.drawFastVLine(lastX + width/2, lastY + (2 * height)/6.f, height/3.f + 1, display.C_BLACK);
        display.drawFastHLine(lastX + width/2 - height/6.f, lastY + height/2, height/3.f + 1, display.C_BLACK);
    }

    CreateTile::CreateTile(ILI9341_TFT &display, uint16_t width, uint16_t height) : Tile(display, width, height) {}
}
