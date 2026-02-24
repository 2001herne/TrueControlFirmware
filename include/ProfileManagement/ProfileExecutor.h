//
// Created by benjaminherne on 21/01/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_PROFILEEXECUTOR_H
#define TRUECONTROLUSBDESCRIPTORS_PROFILEEXECUTOR_H

#include <class/hid/hid.h>
#include <bsp/board_api.h>
#include <map>
#include <string>
#include <queue>
#include <optional>
#include <memory>
#include "Profile.h"
#include "driver/MCP3204.h"

namespace TCFW::ProfileManagement {
    class SimpleHat {
        constexpr static hid_gamepad_hat_t hatLUT[3][3] = {
                {GAMEPAD_HAT_UP_LEFT, GAMEPAD_HAT_UP, GAMEPAD_HAT_UP_RIGHT},
                {GAMEPAD_HAT_LEFT, GAMEPAD_HAT_CENTERED, GAMEPAD_HAT_RIGHT},
                {GAMEPAD_HAT_DOWN_LEFT, GAMEPAD_HAT_DOWN, GAMEPAD_HAT_DOWN_RIGHT}
        };
        int8_t xpos = 1;
        int8_t ypos = 1;
    public:

        void up();
        void down();
        void left();
        void right();

        operator hid_gamepad_hat_t();
    };

    class InputGamepad {
    public:
        using HatSourceFunc = hid_gamepad_hat_t (*) ();
    private:
        using StateSourceFn = bool (*) ();
        std::map<hid_gamepad_button_bm_t, StateSourceFn> gamepadStateSources {};
        std::map<hid_gamepad_hat_t, StateSourceFn> hatSources {};
        std::map<hid_gamepad_hat_t, bool> hatState {};
        std::queue<ButtonEvent> eventQueue {};
        uint32_t currentState = 0;


    public:
        void task();
        void registerStateSource(hid_gamepad_button_bm_t, StateSourceFn);
        void registerHatSource(hid_gamepad_hat_t, StateSourceFn);
        std::optional<ButtonEvent> getNextEvent();
    };

    class OutputGamepad {
    private:
        hid_gamepad_report_t currentState {
            .x = 0,
            .y = 0,
            .z = 0,
            .rz = 0,
            .rx = 0,
            .ry = 0,
            .hat = GAMEPAD_HAT_CENTERED,
            .buttons = 0
        };
        SimpleHat hat;
        MCP3204 stickSource;



    public:
        OutputGamepad(MCP3204& sticks);

        void task();
        void onEvent(const std::shared_ptr<ButtonEvent>& event);
    };

    class ProfileExecutor {
        InputGamepad& input;
        OutputGamepad& output;
        std::priority_queue<
            std::pair<typeof(board_millis()), std::shared_ptr<Event>>,
            std::vector<std::pair<typeof(board_millis()), std::shared_ptr<Event>>>,
            std::greater<>
        > eventQueue {};
        std::shared_ptr<Profile> loadedProfile;

        void processInputs();
        void processOutputs();
    public:
        ProfileExecutor(InputGamepad&, OutputGamepad&);
        void loadProfile(std::weak_ptr<Profile> profile);
        void task();

    };
}


#endif //TRUECONTROLUSBDESCRIPTORS_PROFILEEXECUTOR_H
