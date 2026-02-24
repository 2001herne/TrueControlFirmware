//
// Created by benjaminherne on 21/01/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_MANAGER_H
#define TRUECONTROLUSBDESCRIPTORS_MANAGER_H

#include <map>
#include <memory>
#include <optional>
#include "repl/Executable.h"
#include "Profile.h"
#include "ProfileExecutor.h"

namespace TCFW::ProfileManagement {
    class Manager {
    private:
        std::map<uint32_t, std::shared_ptr<Profile>> profiles;
        ProfileExecutor& executor;
    public:
        Manager(ProfileExecutor& executor);
        std::weak_ptr<Profile> createProfile(std::string profileName);
        std::map<uint32_t, std::weak_ptr<Profile>> getProfiles();

        void loadProfile(int i);

        void deleteProfile(const uint32_t i);
    };

    class ProfileManagerExecutable: public TCFW::Executable {
        Manager& profileManager;
        void createProfile(const std::vector<std::string> &arguments, std::vector<std::string> &output);
        void listProfiles(std::vector<std::string> &output);
        void printProfileOverview(const std::vector<std::string> &arguments, std::vector<std::string> &output);
        void printProfileDetail(const std::vector<std::string> &arguments, std::vector<std::string> &output);
        void loadProfile(const std::vector<std::string> &arguments, std::vector<std::string>& output);
    public:
        explicit ProfileManagerExecutable(Manager& manager);

        void execute(const std::vector<std::string> &arguments, std::vector<std::string> &output) override;

        void setProfileMapping(const std::vector<std::string> &arguments, std::vector<std::string> &output);

        std::optional<EventSequence>
        createSequenceFromArgs(std::vector<std::string>::const_iterator begin,
                               std::vector<std::string>::const_iterator end,
                               std::vector<std::string> &output);
    };
}


#endif //TRUECONTROLUSBDESCRIPTORS_MANAGER_H
