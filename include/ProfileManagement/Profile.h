//
// Created by benjaminherne on 21/01/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_PROFILE_H
#define TRUECONTROLUSBDESCRIPTORS_PROFILE_H

#include <string>
#include <class/hid/hid.h>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace TCFW::ProfileManagement {

    extern const std::map<std::string, hid_gamepad_button_bm_t> gamepadButtons;
    extern const std::map<std::string, hid_gamepad_hat_t> hatButtons;

    struct Button {
        enum ButtonType {
            GAMEPAD,
            HAT
        } buttonType;
        union {
            hid_gamepad_button_bm_t gamepadButton;
            hid_gamepad_hat_t hatButton;
        };
        operator std::string() const;
        static bool isValidButton(std::string button);
        Button(const std::string& buttonName);
        Button(hid_gamepad_button_bm_t gamepadButton);
        Button(hid_gamepad_hat_t hatButton);

        bool operator==(const Button& other) const;

        bool operator<(const Button &rhs) const;
    };

    struct Event {
        enum EventType {
            BUTTON_PRESS, BUTTON_RELEASE, ACTION, NONE
        } eventType;
    };

    struct ButtonEvent: public Event{
        ButtonEvent(EventType eventType, Button button);
        Button button;
        bool operator<(const ButtonEvent &rhs) const;
    };

    struct ActionEvent: public Event {
        const std::function<void()> callback;
        explicit ActionEvent(std::function<void()>  callback);
    };

    typedef std::vector<std::shared_ptr<Event>> EventCombo;

    struct EventSequenceElement {
        EventCombo combo;
        int delayMs = 0;

        operator std::string();
    };

    typedef std::vector<EventSequenceElement> EventSequence;

    struct ButtonMapping {
        explicit ButtonMapping(hid_gamepad_button_bm_t button);
        explicit ButtonMapping(hid_gamepad_hat_t button);

        EventSequence on(ButtonEvent::EventType eventType) const {
            switch (eventType) {
                case ButtonEvent::BUTTON_PRESS:
                    return onPress;
                case ButtonEvent::BUTTON_RELEASE:
                    return onRelease;
                default:
                    return {};
            }
        }

        Button button;
        EventSequence onPress;
        EventSequence onRelease;
    };

    struct Profile {
        Profile(std::string profileName);
        const uint32_t id;
        std::string name;
        std::vector<ButtonMapping> mapping;
    private:
        static uint32_t nextId;
        static std::vector<uint32_t> reservedIds;
        static uint32_t getNextId();
    };
}


#endif //TRUECONTROLUSBDESCRIPTORS_PROFILE_H
