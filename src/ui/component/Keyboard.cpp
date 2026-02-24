//
// Created by benjaminherne on 3/02/26.
//

#include <numeric>
#include <iostream>
#include <utility>
#include "ui/component/Keyboard.h"

namespace TCFW::UI::COMPONENT {
    Keyboard::Keyboard(ILI9341_TFT &display, std::function<void(std::string)> onComplete)
            :Tile(display, 320, 160) {
        int outputTiles = 320/12;
        output.reserve(outputTiles);
        for (auto i = 0; i < outputTiles; i++) {
            output.emplace_back(display, content, i);
        }

        for (int i = 0; i < 12; i++) {
            std::string normal = "1234567890-=";
            std::string caps = "!@#$%^&*()_+";
            auto action = [&capslock = capslock, &content = content, normal = normal[i], caps = caps[i]]() {
                content.push_back(capslock ? caps : normal);
            };
            board[0].emplace_back(std::make_shared<KeyboardLetterTile>(display, capslock, normal[i], caps[i], action));
        }


        for (char c : std::string("qwertyuiop")) {
            auto action = [&capslock = capslock, &content = content, c]() {
                content.push_back(capslock ? std::toupper(c) : c);
            };
            board[1].emplace_back(std::make_shared<KeyboardLetterTile>(display,capslock, c, std::toupper(c), action));
        }

        for (char c : std::string("asdfghjkl")) {
            auto action = [&capslock = capslock, &content = content, c]() {
                content.push_back(capslock ? std::toupper(c) : c);
            };
            board[2].emplace_back(std::make_shared<KeyboardLetterTile>(display, capslock, c, std::toupper(c), action));
        }

        for (char c : std::string("zxcvbnm")) {
            auto action = [&capslock = capslock, &content = content, c]() {
                content.push_back(capslock ? std::toupper(c) : c);
            };
            board[3].emplace_back(std::make_shared<KeyboardLetterTile>(display, capslock, c, std::toupper(c), action));
        }

        auto capslockAction = [&capslock = capslock]() {
            capslock = !capslock;
        };
        auto capslockDraw = [&capslock = capslock](ILI9341_TFT& display, int x, int y, int width, int height, bool selected) {
            display.fillRect(x + width/2 - 8, y + 5, 16, height - 10, ILI9341_TFT::C_BLACK);
            auto colour = capslock ? ILI9341_TFT::C_RED : ILI9341_TFT::C_WHITE;
            display.fillTriangle(x + width/2, y + 5, x + width/2 + 7, y + height/2, x + width/2 - 8, y + height/2, colour);
            display.fillRect(x + width/2 - 2, y + height/2, 4, height/2 - 5, colour);
        };
        board[4].emplace_back(std::make_shared<KeyboardSymbolTile>(display, 2, capslockAction, capslockDraw));

        auto backspaceAction = [&content = content]() {
            if (!content.empty()) content.pop_back();
        };
        auto backspaceDraw = [](ILI9341_TFT& display, int x, int y, int width, int height, bool selected) {
            height = height - 10;
            y += 5;
            int left = x + width/4;
            int right = x + (3 * width)/4;
            int center = x + width/2;
            int top = y + height/6;
            int bottom = y + (5 * height)/6;
            int middle = y + height/2;
            display.drawLine(left, middle, center, top, ILI9341_TFT::C_WHITE);
            display.drawLine(left, middle, center, bottom, ILI9341_TFT::C_WHITE);
            display.drawLine(center, top, right, top, ILI9341_TFT::C_WHITE);
            display.drawLine(center, bottom, right, bottom, ILI9341_TFT::C_WHITE);
            display.drawLine(right, top, right, bottom, ILI9341_TFT::C_WHITE);
            display.drawLine(center, top, right, bottom, ILI9341_TFT::C_WHITE);
            display.drawLine(center, bottom, right, top, ILI9341_TFT::C_WHITE);
        };
        board[4].emplace_back(std::make_shared<KeyboardSymbolTile>(display, 2, backspaceAction, backspaceDraw));

        auto spacebarAction = [&content = content]() {
            content.push_back(' ');
        };
        auto spacebarDraw = [](ILI9341_TFT& display, int x, int y, int width, int height, bool selected) {
            display.drawFastHLine(x + width/10, y + height - 8, (4 * width)/5, ILI9341_TFT::C_WHITE);
        };
        board[4].emplace_back(std::make_shared<KeyboardSymbolTile>(display, 5, spacebarAction, spacebarDraw));

        auto completeAction = [&content = content, onComplete]() {
            onComplete(content);
        };
        auto confirmDraw = [](ILI9341_TFT& display, int x, int y, int width, int height, bool selected) {
            std::string text = "OK";
            int textWidth = text.length() * 8;
            display.setCursor(x + (width - textWidth)/2, y + (height - 16)/2);
            display.setFont(font_retro);
            display.setTextColor(ILI9341_TFT::C_WHITE, ILI9341_TFT::C_BLACK);
            display.print(text);
        };
        board[4].emplace_back(std::make_shared<KeyboardSymbolTile>(display, 2, completeAction, confirmDraw));
    }

    void Keyboard::userDraw(uint16_t x, uint16_t y, bool selected) {
        std::cout << capslock;
        for (int i = 0; i < output.size(); i++) {
            auto &tile = output[i];
            tile.draw(x + i * tile.width, y, i == content.length());
        }
        display.drawFastHLine(0, y + 24, 320, ILI9341_TFT::C_RED);
        for (int i = 0; i < board.size(); i++) {
            auto& row = board[i];
            auto rowWidth = std::accumulate(row.begin(), row.end(), 0, [](auto left, auto right) {
                return left + right->width;
            });
            int offset = std::max(0, width - rowWidth) / 2;
            for (int j = 0; j < row.size(); j++) {
                auto tile = row[j];
                tile->draw(x + offset, y + 24 + i * tile->height, i == selectedRow && j == selectedColumn);
                offset += tile->width;
            }
        }
    }

    void Keyboard::clear() {

    }

    void Keyboard::up() {
        selectedRow = std::max(0, selectedRow - 1);
    }

    void Keyboard::down() {
        selectedRow = std::min((int)board.size() - 1, selectedRow + 1);
    }

    void Keyboard::left() {
        selectedColumn = std::max(0, selectedColumn - 1);
    }

    void Keyboard::right() {
        selectedColumn = std::min((int)board[selectedRow].size(), selectedColumn + 1);
    }

    void Keyboard::action() {
        board[selectedRow][selectedColumn]->action();
    }

    Keyboard::Keyboard(ILI9341_TFT &display, std::function<void(std::string)> onComplete, std::string content):
            Keyboard(display, onComplete){
        Keyboard::content = content;
    }

    LetterDisplayTile::LetterDisplayTile(ILI9341_TFT &display, const std::string &content, size_t index)
        : Tile(display, 12, 24), content(content), index(index) {}

    void LetterDisplayTile::userDraw(uint16_t x, uint16_t y, bool selected) {
        auto colour = selected ? ILI9341_TFT::C_RED : 0x6000;
        display.drawFastHLine(x+2, y + 20, 8, colour);
        if (index < content.size()) {
            display.setFont(font_retro);
            display.setCursor(x + 2, y + 4);
            display.setTextColor(ILI9341_TFT::C_WHITE, ILI9341_TFT::C_BLACK);
            display.print(content[index]);
        } else {
            display.fillRect(x + 2, y + 4, 8, 16, ILI9341_TFT::C_BLACK);
        }
    }

    void LetterDisplayTile::clear() {
        Tile::clear();
    }

    KeyboardKeyTile::KeyboardKeyTile(ILI9341_TFT &display, const uint16_t keyWidth, std::function<void()> action)
        : Tile(display, keyWidth * 24, 27), action{action}{}


    void KeyboardLetterTile::userDraw(uint16_t x, uint16_t y, bool selected) {
        KeyboardKeyTile::userDraw(x, y, selected);
        display.setCursor(x + 8, y + (height - 16)/2);
        display.setFont(font_retro);
        display.setTextColor(ILI9341_TFT::C_WHITE, ILI9341_TFT::C_BLACK);
        display.print(capslock ? caps : normal);
    }

    void KeyboardLetterTile::clear() {
        KeyboardKeyTile::clear();
    }

    KeyboardLetterTile::KeyboardLetterTile(ILI9341_TFT &display, bool& capslock, const char normalChar, const char capsChar, std::function<void()> action)
        :KeyboardKeyTile(display, 1, action), capslock{capslock}, normal{normalChar}, caps{capsChar} {}

    KeyboardSymbolTile::KeyboardSymbolTile(ILI9341_TFT &display,
                                           const uint16_t keyWidth,
                                           const std::function<void()> action,
                                           std::function<void(ILI9341_TFT&, int, int, int, int, bool)> draw)
        :KeyboardKeyTile(display, keyWidth, action),
        drawFunction{draw}{}

    void KeyboardSymbolTile::userDraw(uint16_t x, uint16_t y, bool selected) {
        KeyboardKeyTile::userDraw(x, y, selected);
        drawFunction(display, x, y, width, height, selected);
    }

    void KeyboardSymbolTile::clear() {
        KeyboardKeyTile::clear();
        display.fillRect(lastX + 5, lastY + 5, width - 10, height - 10, ILI9341_TFT::C_BLACK);
    }
} // TCFW::UI::COMPONENT