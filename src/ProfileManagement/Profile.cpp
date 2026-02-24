//
// Created by benjaminherne on 21/01/26.
//

#include "ProfileManagement/Profile.h"

#include <utility>
#include <algorithm>

#define returnCase(s) case(s): return #s

namespace TCFW::ProfileManagement {
    ButtonMapping::ButtonMapping(hid_gamepad_button_bm_t button):
        button {button}, onPress {
        {EventCombo{std::make_shared<ButtonEvent>(ButtonEvent::EventType::BUTTON_PRESS, this->button)}}
    }, onRelease {
        {EventCombo{std::make_shared<ButtonEvent>(ButtonEvent::EventType::BUTTON_RELEASE, this->button)}}
    } {}

    ButtonMapping::ButtonMapping(hid_gamepad_hat_t button):
        button {button}, onPress {
            {EventCombo{std::make_shared<ButtonEvent>(ButtonEvent::EventType::BUTTON_PRESS, this->button)}}
    }, onRelease {
            {EventCombo{std::make_shared<ButtonEvent>(ButtonEvent::EventType::BUTTON_RELEASE, this->button)}}
    } {}


    Profile::Profile(std::string profileName): id {getNextId()}, name {std::move(profileName)} {
        mapping.emplace_back(GAMEPAD_BUTTON_NORTH);
        mapping.emplace_back(GAMEPAD_BUTTON_EAST);
        mapping.emplace_back(GAMEPAD_BUTTON_SOUTH);
        mapping.emplace_back(GAMEPAD_BUTTON_WEST);
        mapping.emplace_back(GAMEPAD_HAT_UP);
        mapping.emplace_back(GAMEPAD_HAT_RIGHT);
        mapping.emplace_back(GAMEPAD_HAT_DOWN);
        mapping.emplace_back(GAMEPAD_HAT_LEFT);
        mapping.emplace_back(GAMEPAD_BUTTON_TL);
        mapping.emplace_back(GAMEPAD_BUTTON_TL2);
        mapping.emplace_back(GAMEPAD_BUTTON_TR);
        mapping.emplace_back(GAMEPAD_BUTTON_TR2);
        mapping.emplace_back(GAMEPAD_BUTTON_THUMBL);
        mapping.emplace_back(GAMEPAD_BUTTON_THUMBR);
    }

    uint32_t Profile::getNextId() {
        while (std::find(reservedIds.begin(), reservedIds.end(), nextId) != reservedIds.end()) {
            nextId++;
        }
        return nextId++;
    }

    uint32_t Profile::nextId = 0;
    std::vector<uint32_t> Profile::reservedIds {};


    Button::operator std::string() const {
        switch(buttonType) {
            case GAMEPAD:
                switch (gamepadButton) {
                    returnCase(GAMEPAD_BUTTON_NORTH);
                    returnCase(GAMEPAD_BUTTON_EAST);
                    returnCase(GAMEPAD_BUTTON_SOUTH);
                    returnCase(GAMEPAD_BUTTON_WEST);
                    default:
                        return "GAMEPAD_BUTTON " + std::to_string(std::countr_zero((uint32_t)gamepadButton));
                }
            case HAT:
                switch(hatButton) {
                    returnCase(GAMEPAD_HAT_CENTERED);
                    returnCase(GAMEPAD_HAT_UP);
                    returnCase(GAMEPAD_HAT_UP_RIGHT);
                    returnCase(GAMEPAD_HAT_RIGHT);
                    returnCase(GAMEPAD_HAT_DOWN_RIGHT);
                    returnCase(GAMEPAD_HAT_DOWN);
                    returnCase(GAMEPAD_HAT_DOWN_LEFT);
                    returnCase(GAMEPAD_HAT_LEFT);
                    returnCase(GAMEPAD_HAT_UP_LEFT);
                }
        }
        return "";
    }

    #define gpButton(name) {"GAMEPAD_BUTTON_" #name, GAMEPAD_BUTTON_ ## name}

    const std::map<std::string, hid_gamepad_button_bm_t> gamepadButtons = {
            gpButton(0),
            gpButton(1),
            gpButton(2),
            gpButton(3),
            gpButton(4),
            gpButton(5),
            gpButton(6),
            gpButton(7),
            gpButton(8),
            gpButton(9),
            gpButton(10),
            gpButton(11),
            gpButton(12),
            gpButton(13),
            gpButton(14),
            gpButton(15),
            gpButton(16),
            gpButton(17),
            gpButton(18),
            gpButton(19),
            gpButton(20),
            gpButton(21),
            gpButton(22),
            gpButton(23),
            gpButton(24),
            gpButton(25),
            gpButton(26),
            gpButton(27),
            gpButton(28),
            gpButton(29),
            gpButton(30),
            gpButton(31),
            gpButton(NORTH),
            gpButton(SOUTH),
            gpButton(EAST),
            gpButton(WEST),
            gpButton(TL),
            gpButton(TL2),
            gpButton(TR),
            gpButton(TR2),
            gpButton(THUMBL),
            gpButton(THUMBR)
    };
    #undef gpButton

    const std::map<std::string, hid_gamepad_hat_t> hatButtons= {
            {"GAMEPAD_HAT_CENTERED", GAMEPAD_HAT_CENTERED},
            {"GAMEPAD_HAT_UP", GAMEPAD_HAT_UP},
            {"GAMEPAD_HAT_UP_RIGHT", GAMEPAD_HAT_UP_RIGHT},
            {"GAMEPAD_HAT_RIGHT", GAMEPAD_HAT_RIGHT},
            {"GAMEPAD_HAT_DOWN_RIGHT", GAMEPAD_HAT_DOWN_RIGHT},
            {"GAMEPAD_HAT_DOWN", GAMEPAD_HAT_DOWN},
            {"GAMEPAD_HAT_DOWN_LEFT", GAMEPAD_HAT_DOWN_LEFT},
            {"GAMEPAD_HAT_LEFT", GAMEPAD_HAT_LEFT},
            {"GAMEPAD_HAT_UP_LEFT", GAMEPAD_HAT_UP_LEFT},
    };

    bool Button::isValidButton(std::string button) {
        return gamepadButtons.contains(button) || hatButtons.contains(button);
    }



    Button::Button(const std::string& buttonName) {
        if (buttonName.starts_with("GAMEPAD_BUTTON")) {
            buttonType = GAMEPAD;
            gamepadButton = gamepadButtons.at(buttonName);
        } else if (buttonName.starts_with("GAMEPAD_HAT")) {
            buttonType = HAT;
            hatButton = hatButtons.at(buttonName);
        }
    }

    Button::Button(hid_gamepad_button_bm_t gamepadButton): buttonType {GAMEPAD}, gamepadButton {gamepadButton} {}

    Button::Button(hid_gamepad_hat_t hatButton): buttonType{HAT}, hatButton{hatButton} {}

    bool Button::operator==(const Button &other) const {
        if (buttonType == other.buttonType) {
            switch(buttonType) {
                case GAMEPAD:
                    return gamepadButton == other.gamepadButton;
                case HAT:
                    return hatButton == other.hatButton;
            }
        }
        return false;
    }

    bool Button::operator<(const Button &rhs) const {
        if (buttonType < rhs.buttonType) {
            return true;
        }
        if (rhs.buttonType < buttonType) {
            return false;
        }
        switch(buttonType) {
            case GAMEPAD:
                return gamepadButton < rhs.gamepadButton;
            case HAT:
                return hatButton < rhs.hatButton;
            default:
                return false;
        }
    }

    EventSequenceElement::operator std::string() {
        std::string output {};
        for (const auto& event : combo) {
            switch(event->eventType) {
                case Event::BUTTON_PRESS:
                case Event::BUTTON_RELEASE:
                    output += event->eventType == Event::BUTTON_PRESS ? "v" : "^";
                    output += std::reinterpret_pointer_cast<ButtonEvent>(event)->button;
                    output += " | ";
                    break;
                default:
                    output += "Unhandled event type";
            }
        }
        output += "Delay: " + std::to_string(delayMs) + "ms" ;
        return output;
    }

    bool ButtonEvent::operator<(const ButtonEvent &rhs) const {
        if (eventType < rhs.eventType)
            return true;
        if (rhs.eventType < eventType)
            return false;
        return button < rhs.button;
    }

    ButtonEvent::ButtonEvent(Event::EventType eventType, Button button)
        : Event(eventType), button(button) {}

    ActionEvent::ActionEvent(std::function<void()> callback): Event(ACTION), callback(std::move(callback)) {}
}
