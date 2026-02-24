//
// Created by benjaminherne on 3/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_PROFILEEDITSCREEN_H
#define TRUECONTROLUSBDESCRIPTORS_PROFILEEDITSCREEN_H

#include "ui/Screen.h"
#include "ui/component/Tile.h"

namespace TCFW {
    namespace UI {
        class ProfileMappingTile : public COMPONENT::Tile {
        public:
            explicit ProfileMappingTile(ILI9341_TFT &display, ProfileManagement::ButtonMapping mapping);

            void userDraw(uint16_t x, uint16_t y, bool selected) override;

            void clear() override;

            ProfileManagement::ButtonMapping mapping;
        };

        class ProfileEditScreen : public UIScreen {
        protected:
        public:
            explicit ProfileEditScreen(ILI9341_TFT &targetDisplay, std::shared_ptr<ProfileManagement::Profile> profile);

        protected:
            void paintContent() override;

            bool shouldRepaintContent() override;

            void onEvent(ProfileManagement::Button event) override;

        private:
            std::shared_ptr<ProfileManagement::Profile> profile;
            std::vector<ProfileMappingTile> tiles;

            void updateNavbar();

            bool needsRepaint = true;

            int selectedTileIndex = 0;
            int oldSelected = 0;
        };

    } // UI
} // TCFW

#endif //TRUECONTROLUSBDESCRIPTORS_PROFILEEDITSCREEN_H
