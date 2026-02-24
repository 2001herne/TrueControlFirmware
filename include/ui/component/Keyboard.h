//
// Created by benjaminherne on 3/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_KEYBOARD_H
#define TRUECONTROLUSBDESCRIPTORS_KEYBOARD_H

#include <string>
#include <memory>
#include <functional>
#include "Tile.h"

namespace TCFW {
    namespace UI {
        namespace COMPONENT {

            class LetterDisplayTile: public Tile{
            public:
                explicit LetterDisplayTile(ILI9341_TFT &display, const std::string& content, size_t index);

                void userDraw(uint16_t x, uint16_t y, bool selected) override;

                void clear() override;
            private:
                const std::string& content;
                const size_t index;
            };

            class KeyboardKeyTile: public Tile {
            public:
                KeyboardKeyTile(ILI9341_TFT& display, const uint16_t keyWidth, std::function<void()> action);

                const std::function<void()> action;
            };

            class KeyboardLetterTile: public KeyboardKeyTile {
            public:
                KeyboardLetterTile(ILI9341_TFT &display, bool& capslock, const char normalChar, const char capsChar, std::function<void()> action);

                void userDraw(uint16_t x, uint16_t y, bool selected) override;

                void clear() override;
            private:
                char normal;
                char caps;
                bool& capslock;
            };

            class KeyboardSymbolTile: public KeyboardKeyTile{
            public:
                KeyboardSymbolTile(ILI9341_TFT &display,
                                   const uint16_t keyWidth,
                                   const std::function<void()> action,
                                   std::function<void(ILI9341_TFT&, int, int, int, int, bool)> draw);

                void userDraw(uint16_t x, uint16_t y, bool selected) override;

                void clear() override;
            private:
                std::function<void(ILI9341_TFT&, int, int, int, int, bool)> drawFunction;
            };

            class Keyboard : public Tile{
            public:
                Keyboard(ILI9341_TFT &display, std::function<void(std::string)> onComplete);
                Keyboard(ILI9341_TFT &display, std::function<void(std::string)> onComplete, std::string content);
                void userDraw(uint16_t x, uint16_t y, bool selected) override;

                void clear() override;

                void up();
                void down();
                void left();
                void right();
                void action();


            private:
                std::string content;
                std::vector<LetterDisplayTile> output;
                std::array<std::vector<std::shared_ptr<KeyboardKeyTile>>, 5> board;
                bool capslock = false;
                int selectedRow = 0;
                int selectedColumn = 0;
            };

        } // COMPONENT
    } // UI
} // TCFS

#endif //TRUECONTROLUSBDESCRIPTORS_KEYBOARD_H
