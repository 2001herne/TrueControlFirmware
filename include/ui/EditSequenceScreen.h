//
// Created by benjaminherne on 4/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_EDITSEQUENCESCREEN_H
#define TRUECONTROLUSBDESCRIPTORS_EDITSEQUENCESCREEN_H

#include "Screen.h"
#include "ui/component/Tile.h"

namespace TCFW {
    namespace UI {

        class SequenceElementComboTile: public COMPONENT::Tile {
        public:
            SequenceElementComboTile(ILI9341_TFT &display, ProfileManagement::EventCombo combo);

            void userDraw(uint16_t x, uint16_t y, bool selected) override;

            void clear() override;
        private:
            ProfileManagement::EventCombo combo;
        };

        class SequenceElementDelayTile: public COMPONENT::Tile {
        public:
            SequenceElementDelayTile(ILI9341_TFT &display, uint16_t delayMs);

            void userDraw(uint16_t x, uint16_t y, bool selected) override;

            void clear() override;

            void updateDelay(uint16_t newDelay);
        private:
            uint16_t delayMs;
        };

        class EditSequenceScreen: public UIScreen {
        public:
            explicit EditSequenceScreen(ILI9341_TFT &targetDisplay, std::shared_ptr<ProfileManagement::Profile> profile, ProfileManagement::Button button);

        private:
            void paintContent() override;

            bool shouldRepaintContent() override;

            void onEvent(ProfileManagement::Button event) override;

            std::shared_ptr<ProfileManagement::Profile> profile;
            ProfileManagement::Button button;

            std::vector<std::shared_ptr<COMPONENT::Tile>> tiles;

            bool needsRepaint = true;

            void updateNavbar();

            int selectedTileIndex = 0;
            int oldSelectedTileIndex = 0;
        };

    } // UI
} // TCFW

#endif //TRUECONTROLUSBDESCRIPTORS_EDITSEQUENCESCREEN_H
