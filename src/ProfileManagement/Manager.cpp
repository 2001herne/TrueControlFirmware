//
// Created by benjaminherne on 21/01/26.
//

#include <algorithm>
#include "ProfileManagement/Manager.h"
#include <optional>
#include <iostream>

namespace TCFW::ProfileManagement {


    void ProfileManagerExecutable::execute(
            const std::vector<std::string> &arguments,
            std::vector<std::string> &output) {
        if (arguments.size() >= 2) {
            if (arguments[1] == "create" ) {
                createProfile(arguments, output);
            } else if (arguments[1] == "list") {
                listProfiles(output);
            } else if (arguments[1] == "print") {
                if (arguments.size() == 3) {
                    printProfileOverview(arguments, output);
                } else if (arguments.size() >= 4) {
                    printProfileDetail(arguments, output);
                } else {
                    output.emplace_back("Not enough arguments.");
                    output.emplace_back("Usage: profile print <profile-id> [mapping]");
                }
            } else if (arguments[1] == "set") {
                if (arguments.size() >= 5) {
                    setProfileMapping(arguments, output);
                } else {
                    output.emplace_back("Not enough arguments.");
                    output.emplace_back("Usage: profile set <profile-id> <button-name> <event-type>");
                }
            } else if (arguments[1] == "load") {
                output.emplace_back("Loading Profile");
                loadProfile(arguments, output);
            }
        }
    }

    void ProfileManagerExecutable::printProfileOverview(const std::vector<std::string> &arguments,
                                                        std::vector<std::string> &output) {
        auto profiles = profileManager.getProfiles();
        if (profiles.contains(std::stoul(arguments[2]))) {
            auto profile = *profiles[std::stoul(arguments[2])].lock();
            output.emplace_back("Profile \"" + profile.name + "\" has mappings for the following:");
            for (const auto& button : profile.mapping) {
                output.emplace_back("- " + (std::string)(button.button));
            }
        } else {
            output.emplace_back("No such profile id.");
        }
    }

    void ProfileManagerExecutable::listProfiles(std::vector<std::string> &output) {
        output.emplace_back("Profiles:");
        for (const auto& pair : profileManager.getProfiles()) {
            auto profile = pair.second.lock();
            output.emplace_back("- " + std::to_string(profile->id) + ": " + profile->name);
        }
    }

    void ProfileManagerExecutable::createProfile(const std::vector<std::string> &arguments,
                                                 std::vector<std::string> &output) {
        if (arguments.size() >= 3) {
            auto newProfile = profileManager.createProfile(arguments[2]);
            output.emplace_back("Created Profile: " + newProfile.lock()->name);
        } else {
            output.emplace_back("Not enough arguments.");
            output.emplace_back("Usage: profile create <profile-name>");
        }
    }

    ProfileManagerExecutable::ProfileManagerExecutable(Manager &manager): profileManager {manager} {}

    void ProfileManagerExecutable::printProfileDetail(const std::vector<std::string> &arguments,
                                                      std::vector<std::string> &output) {
        auto profiles = profileManager.getProfiles();
        if (profiles.contains(std::stoul(arguments[2]))) {
            auto profile = *profiles[std::stoul(arguments[2])].lock();
            auto mapping = std::find_if(profile.mapping.begin(), profile.mapping.end(), [arguments](const ButtonMapping& mapping) -> bool {
                return arguments[3] == (std::string)mapping.button;
            });

            if (mapping != profile.mapping.end()) {
                output.emplace_back("Profile \"" + profile.name + "\" " + (std::string)mapping->button + ":");
                output.emplace_back("-- onPress:");
                for (auto combo : mapping->onPress) {
                    output.emplace_back("--- " + (std::string)combo);
                }
                output.emplace_back("-- onRelease:");
                for (auto combo : mapping->onRelease) {
                    output.emplace_back("--- " + (std::string)combo);
                }
            } else {
                output.emplace_back("No such mapping on profile \"" + profile.name + "\"");
            }
        } else {
            output.emplace_back("No such profile id.");
        }
    }

    void ProfileManagerExecutable::setProfileMapping(const std::vector<std::string> &arguments,
                                                     std::vector<std::string> &output) {
        auto profiles = profileManager.getProfiles();
        if (profiles.contains(std::stoul(arguments[2]))) {
            auto profile = profiles[std::stoul(arguments[2])].lock();
            auto mapping = std::find_if(profile->mapping.begin(), profile->mapping.end(), [arguments](const ButtonMapping& mapping) -> bool {
                if (gamepadButtons.contains(arguments[3])) {
                    return gamepadButtons.at(arguments[3]) == mapping.button.gamepadButton;
                } else if (hatButtons.contains(arguments[3])) {
                    return hatButtons.at(arguments[3]) == mapping.button.hatButton;
                }
                return false;

            });

            if (mapping != profile->mapping.end()) {
                if (arguments[4] == "press") {
                    std::optional<EventSequence> newSequence = createSequenceFromArgs(arguments.begin() + 5, arguments.end(), output);
                    if (newSequence) {
                        mapping->onPress = newSequence.value();
                    } else {
                        output.emplace_back("Unable to construct sequence");
                    }
                } else if (arguments[4] == "release") {
                    std::optional<EventSequence> newSequence = createSequenceFromArgs(arguments.begin() + 5, arguments.end(), output);
                    if (newSequence) {
                        mapping->onRelease = newSequence.value();
                    } else {
                        output.emplace_back("Unable to construct sequence");
                    }
                } else {
                    output.emplace_back(R"(Invalid event specified. Must be one of "press" or "release")");
                }
            } else {
                output.emplace_back("No such mapping on profile \"" + profile->name + "\"");
                if (gamepadButtons.contains(arguments[3])) {
                    output.emplace_back("Creating...");
                    profile->mapping.emplace_back(gamepadButtons.at(arguments[3]));
                    setProfileMapping(arguments, output);
                } else if (hatButtons.contains(arguments[3])) {
                    output.emplace_back("Creating...");
                    profile->mapping.emplace_back(hatButtons.at(arguments[3]));
                    setProfileMapping(arguments, output);
                } else {
                    output.emplace_back("Button does not appear to exist. Not creating.");
                }
            }
        } else {
            output.emplace_back("No such profile id.");
        }
    }

    std::optional<EventSequence>
    ProfileManagerExecutable::createSequenceFromArgs(std::vector<std::string>::const_iterator begin,
                                                     std::vector<std::string>::const_iterator end,
                                                     std::vector<std::string> &output) {
        EventSequence retSeq {};
        while (begin != end) {
            auto comboEnd = std::find_if(begin, end, [](std::string value) -> bool {
                char* endptr;
                strtol(value.c_str(), &endptr, 10);
                return endptr != value.c_str() && *endptr == '\0';
            });
            if (comboEnd != end) {
                EventCombo newCombo;
                for (auto button = begin; button != comboEnd; button++) {
                    switch ((*button)[0]) {
                        case 'v':
                        case '^':
                            if (Button::isValidButton(button->substr(1))) {
                                newCombo.emplace_back(std::make_shared<ButtonEvent>(
                                        (*button)[0] == 'v' ? Event::BUTTON_PRESS : Event::BUTTON_RELEASE,
                                        Button(button->substr(1))
                                ));
                                break;
                            }
                        default:
                            output.emplace_back(R"(Invalid button name specifier: Name not prefixed or invalid name)");
                            return {};
                    }
                }
                EventSequenceElement elem = {
                        .combo = newCombo,
                        .delayMs = std::stoi(*comboEnd)
                };
                retSeq.push_back(elem);
                begin = comboEnd + 1;
            } else {
                output.emplace_back("Failed to consume all input. Malformed specifier for sequence");
                return {};
            }
        }
        return retSeq;
    }

    void
    ProfileManagerExecutable::loadProfile(const std::vector<std::string> &arguments, std::vector<std::string> &output) {
        if (arguments.size() >= 3) {
            output.emplace_back("Loading Profile " + arguments[2]);
            profileManager.loadProfile(std::stoi(arguments[2]));
        }
    }

    std::weak_ptr<Profile> Manager::createProfile(std::string profileName) {
        auto profile = std::make_shared<Profile>(profileName);
        profiles[profile->id] = profile;
        return profile;
    }

    std::map<uint32_t, std::weak_ptr<Profile>> Manager::getProfiles() {
        std::map<uint32_t , std::weak_ptr<Profile>> retmap {};
        for (const auto& pair : profiles) {
            retmap[pair.first] = pair.second;
        }
        return retmap;
    }

    void Manager::loadProfile(int i) {
        if (this->profiles.contains(i)) {
            std::cout << "Found profile to load... Loading" << std::endl;
            executor.loadProfile(this->profiles[i]);
        }
    }

    Manager::Manager(ProfileExecutor &executor): executor{executor} {
    }

    void Manager::deleteProfile(const uint32_t i) {
        if (profiles.contains(i)) {
            profiles.erase(i);
        }
    }
}
