//
// Created by benjaminherne on 21/01/26.
//

#include <iostream>
#include <bsp/board_api.h>
#include <algorithm>
#include <utility>
#include <complex>
#include <numeric>
#include "ProfileManagement/ProfileExecutor.h"
#include "usb_descriptors.h"

namespace TCFW::ProfileManagement {
    void InputGamepad::registerStateSource(hid_gamepad_button_bm_t gamepadButton,
                                           StateSourceFn source) {
        gamepadStateSources[gamepadButton] = source;
    }

    void InputGamepad::registerHatSource(hid_gamepad_hat_t hatButton,
                                         InputGamepad::StateSourceFn source) {
        hatSources[hatButton] = source;
    }

    void InputGamepad::task() {
        for (auto stateSource : gamepadStateSources) {
            bool oldState = currentState & stateSource.first;
            bool newState = stateSource.second();
            if (newState != oldState) {
                eventQueue.emplace(
                    newState ? Event::BUTTON_PRESS : ButtonEvent::BUTTON_RELEASE,
                    Button(stateSource.first)
                );
                currentState ^= stateSource.first;
            }
        }
        for (auto hatSource : hatSources) {
            bool oldState = hatState.contains(hatSource.first) ? hatState.at(hatSource.first) : false;
            bool newState = hatSource.second();
            if (newState != oldState) {
                eventQueue.emplace(
                    newState ? Event::BUTTON_PRESS : ButtonEvent::BUTTON_RELEASE,
                    Button(hatSource.first)
                );
                hatState[hatSource.first] = newState;
            }
        }
    }

    std::optional<ButtonEvent> InputGamepad::getNextEvent(){
        if (eventQueue.empty()) {
            return {};
        }
        auto retval = eventQueue.front();
        eventQueue.pop();
        return retval;
    }

    ProfileExecutor::ProfileExecutor(InputGamepad & inputGamepad, OutputGamepad& outputGamepad):
        input {inputGamepad},
        output {outputGamepad} {}

    void ProfileExecutor::task() {
        processInputs();
        processOutputs();
    }

    void ProfileExecutor::processInputs() {
        if (loadedProfile) {
            std::optional<ButtonEvent> inputEvent;
            while ((inputEvent = input.getNextEvent())) {
                std::cout << "Got event \""
                          << (inputEvent.value().eventType == Event::BUTTON_PRESS ? "press" : "release")
                          << "\" on button \""
                          << (std::string) inputEvent.value().button
                          << "\""
                          << std::endl;
                auto eventSequenceIterator = std::find_if(loadedProfile->mapping.begin(), loadedProfile->mapping.end(),
                                                          [inputEvent](const ButtonMapping &element) -> bool {
                                                              return element.button == inputEvent->button;
                                                          });
                if (eventSequenceIterator == loadedProfile->mapping.end()) {
                    std::cout << "Profile does not map this button" << std::endl;
                    continue;
                }
                auto eventScheduleTime = board_millis();
                for (auto sequenceElement: eventSequenceIterator->on(inputEvent->eventType)) {
                    for (auto event: sequenceElement.combo) {
                        std::shared_ptr<ButtonEvent> outputButtonEvent {};
                        switch(event->eventType) {
                            case Event::BUTTON_PRESS:
                            case Event::BUTTON_RELEASE:
                                outputButtonEvent = std::reinterpret_pointer_cast<ButtonEvent>(event);
                                std::cout << "Scheduling "
                                          << (event->eventType == Event::BUTTON_PRESS ? "press" : "release")
                                          << "\" on button \""
                                          << (std::string) outputButtonEvent->button
                                          << "\" at "
                                          << eventScheduleTime
                                          << std::endl;
                                break;
                            default:
                                break;
                        }
                        eventQueue.emplace(
                            eventScheduleTime,
                            event
                        );
                    }
                    eventScheduleTime += sequenceElement.delayMs;
                }
            }
        }
    }

    void ProfileExecutor::processOutputs() {
        auto now = board_millis();
        while (!eventQueue.empty() && eventQueue.top().first <= now) {
            auto event = eventQueue.top().second;
            std::shared_ptr<ButtonEvent> outputButtonEvent;
            switch(event->eventType) {
                case Event::BUTTON_PRESS:
                case Event::BUTTON_RELEASE:
                    outputButtonEvent = std::reinterpret_pointer_cast<ButtonEvent>(event);
                    std::cout << "Processing event \""
                              << (outputButtonEvent->eventType == Event::BUTTON_PRESS ? "press" : "release")
                              << (std::string)outputButtonEvent->button
                              << "\", scheduled for "
                              << eventQueue.top().first
                              << " at "
                              << now
                              << std::endl;
                    output.onEvent(outputButtonEvent);
                    break;
                case Event::ACTION:
                    std::reinterpret_pointer_cast<ActionEvent>(event)->callback();
                    break;
                default:
                    std::cout << "\tUnhandled Event Type" << std::endl;
            }
            eventQueue.pop();
        }
    }

    void ProfileExecutor::loadProfile(std::weak_ptr<Profile> profile) {
        std::cout << "Profile is expired? " << profile.expired() << std::endl;
        loadedProfile = profile.lock();
    }

    OutputGamepad::OutputGamepad(MCP3204 &sticks): stickSource{sticks} {}

    void OutputGamepad::task() {
        if (tud_suspended()) tud_remote_wakeup();
        if (!tud_hid_ready()) return;
        currentState.hat = hat;

        auto floor_mod = [](double dividend, double divisor) {
            return dividend - divisor * std::floor(dividend / divisor);
        };

        auto loopingAngle = [&floor_mod](int x, int y) {
            return floor_mod(std::atan2(y, x) - std::numbers::pi / 4, std::numbers::pi / 2) - std::numbers::pi / 4;
        };

        auto scalingFactor = [&loopingAngle](int x, int y) {
            auto angle = loopingAngle(x, y);
            return 1 / std::sqrt(1 + std::pow(std::tan(angle), 2));
        };

        auto lx = (stickSource.read<MCP3204::CH1>() - 2048);
        auto ly = (stickSource.read<MCP3204::CH0>() - 2048);
        auto lsf = scalingFactor(lx, ly);
        currentState.x = static_cast<int8_t>(-std::trunc(lx * lsf / 16));
        currentState.y = static_cast<int8_t>(std::trunc(ly * lsf / 16));

        int rx = stickSource.read<MCP3204::CH3>() - 2048;
        int ry = stickSource.read<MCP3204::CH2>() - 2048;
        auto rsf = scalingFactor(rx, ry);
        currentState.rx = static_cast<int8_t>(std::trunc(rx * rsf / 16));
        currentState.ry = static_cast<int8_t>(-std::trunc(ry * rsf / 16));

        tud_hid_report(REPORT_ID_GAMEPAD, &currentState, sizeof(currentState));
    }

    void OutputGamepad::onEvent(const std::shared_ptr<ButtonEvent>& event) {
        switch (event->button.buttonType) {
            case Button::GAMEPAD:
                if (event->eventType == Event::BUTTON_PRESS) {
                    currentState.buttons |= event->button.gamepadButton;
                    if (event->button.gamepadButton == GAMEPAD_BUTTON_TL2) {
                        currentState.z = 0b1111111;
                    }
                    if (event->button.gamepadButton == GAMEPAD_BUTTON_TR2) {
                        currentState.rz = 0b1111111;
                    }
                }
                if (event->eventType == ButtonEvent::BUTTON_RELEASE) {
                    currentState.buttons &= ~event->button.gamepadButton;
                    if (event->button.gamepadButton == GAMEPAD_BUTTON_TL2) {
                        currentState.z = 0;
                    }
                    if (event->button.gamepadButton == GAMEPAD_BUTTON_TR2) {
                        currentState.rz = 0;
                    }
                }
                break;
            case Button::HAT:
                switch(event->button.hatButton) {
                    case GAMEPAD_HAT_UP:
                        if (event->eventType == Event::BUTTON_PRESS) {
                            hat.up();
                        } else {
                            hat.down();
                        }
                        break;
                    case GAMEPAD_HAT_RIGHT:
                        if (event->eventType == Event::BUTTON_PRESS) {
                            hat.right();
                        } else {
                            hat.left();
                        }
                        break;
                    case GAMEPAD_HAT_DOWN:
                        if (event->eventType == Event::BUTTON_PRESS) {
                            hat.down();
                        } else {
                            hat.up();
                        }
                        break;
                    case GAMEPAD_HAT_LEFT:
                        if (event->eventType == Event::BUTTON_PRESS) {
                            hat.left();
                        } else {
                            hat.right();
                        }
                        break;
                    default:
                        break;
                }
        }
    }

    void SimpleHat::up() {
        ypos = (int8_t)std::clamp(ypos - 1, 0, 2);
    }

    void SimpleHat::down() {
        ypos = (int8_t)std::clamp(ypos + 1, 0, 2);
    }

    void SimpleHat::left() {
        xpos = (int8_t)std::clamp(xpos - 1, 0, 2);
    }

    void SimpleHat::right() {
        xpos = (int8_t)std::clamp(xpos + 1, 0, 2);
    }

    SimpleHat::operator hid_gamepad_hat_t() {
        return SimpleHat::hatLUT[ypos][xpos];
    }
}
