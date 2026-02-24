//
// Created by benjaminherne on 2/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_PROFILESELECTIONSCREEN_H
#define TRUECONTROLUSBDESCRIPTORS_PROFILESELECTIONSCREEN_H

#include "ProfileManagement/Manager.h"
#include "Screen.h"
#include "component/Tile.h"

namespace TCFW {
    namespace UI {

        struct ConcreteProfileSelectionTile : public COMPONENT::Tile {
            ConcreteProfileSelectionTile(ILI9341_TFT &display,
                                         const std::weak_ptr<ProfileManagement::Profile> &profile);

            std::weak_ptr<ProfileManagement::Profile> profile;

            void userDraw(uint16_t x, uint16_t y, bool selected) override;

            void clear() override;
        };

        class ProfileSelectScreen: public UIScreen {
        public:
            ProfileSelectScreen(ILI9341_TFT &targetDisplay, ProfileManagement::Manager &profileManager);

            void paintContent() override;

        protected:
            bool shouldRepaintContent() override;

            void onEvent(ProfileManagement::Button event) override;

        private:
            ProfileManagement::Manager& profileManager;
            std::vector<std::shared_ptr<COMPONENT::Tile>> tiles;

            size_t selectedTileIndex = 0;
            size_t oldSelected = 0;

            bool needsRepaint = true;

            void updateNavbar();
        };
    } // UI
} // TCFW

#endif //TRUECONTROLUSBDESCRIPTORS_PROFILESELECTIONSCREEN_H
