//
// Created by benjaminherne on 20/01/26.
//

#include <vector>
#include <string>

#ifndef TRUECONTROLUSBDESCRIPTORS_EXECUTABLE_H
#define TRUECONTROLUSBDESCRIPTORS_EXECUTABLE_H

namespace TCFW {
    class Executable {
    protected:
        std::vector<std::string> arguments;
    public:
        virtual void execute(const std::vector<std::string> &arguments, std::vector<std::string> &output) = 0;
    };

    class RebootExecutable: public Executable{
    public:
        void execute(const std::vector<std::string> &arguments, std::vector<std::string> &output) override;
    };
}


#endif //TRUECONTROLUSBDESCRIPTORS_EXECUTABLE_H
