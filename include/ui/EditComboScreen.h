//
// Created by benjaminherne on 4/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_EDITCOMBOSCREEN_H
#define TRUECONTROLUSBDESCRIPTORS_EDITCOMBOSCREEN_H

#include "ui/Screen.h"
#include "ui/component/Tile.h"

namespace TCFW {
    namespace UI {
        class ButtonToggleTile: public COMPONENT::Tile {
        public:
            ButtonToggleTile(ILI9341_TFT &display, ProfileManagement::Button button);
            void userDraw(uint16_t x, uint16_t y, bool selected) override;

            bool active = false;

            void clear() override;

            ProfileManagement::Button button;
        };

        class EditComboScreen : public UIScreen {
        protected:
        public:
            explicit EditComboScreen(ILI9341_TFT &targetDisplay, std::shared_ptr<ProfileManagement::Profile> profile, ProfileManagement::Button button, size_t comboIndex);

        protected:
            void paintContent() override;

            bool shouldRepaintContent() override;

            void onEvent(ProfileManagement::Button event) override;
        private:
            std::shared_ptr<ProfileManagement::Profile> profile;
            ProfileManagement::Button button;
            size_t comboIndex;

            bool needsRepaint = true;
            std::vector<ButtonToggleTile> tiles {};
            int selectedTileIndex = 0 ;
        };

    } // UI
} // TCFW

#endif //TRUECONTROLUSBDESCRIPTORS_EDITCOMBOSCREEN_H
