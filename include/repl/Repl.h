//
// Created by benjaminherne on 20/01/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_REPL_H
#define TRUECONTROLUSBDESCRIPTORS_REPL_H

#include <vector>
#include <memory>
#include <map>
#include <queue>
#include "Executable.h"

namespace TCFW {
    enum Keyname : char {
        NEWLINE     = 10,
        RETURN      = 13,
        BACKSPACE   = 127
    };

    struct Repl {
    private:
        enum STATE {
            READ,
            TOKENISE,
            EXECUTE,
            PRINT
        } state = READ;
        std::string inputBuffer;
        std::vector<std::string> commandBuffer;
        std::shared_ptr<std::map<std::string, std::shared_ptr<Executable>>> commands;
        std::vector<std::string> outputBuffer;

        void subtaskHandleInput();
        void subtaskTokeniseInput();
        void subtaskExecute();
        void subtaskPrint();


    public:
        Repl();

        void task();
        void registerExecutable(const std::string& name, std::shared_ptr<Executable> executable);

    };

    class HelpExecutable: public Executable {
    private:
        std::weak_ptr<const std::map<std::string, std::shared_ptr<Executable>>> executables;
    public:
        HelpExecutable(std::weak_ptr<const std::map<std::string, std::shared_ptr<Executable>>> validExecutables);
        void execute(const std::vector<std::string> &arguments, std::vector<std::string> &output) override;
    };

    class FallbackExecutable: public Executable {
    public:
        void execute(const std::vector<std::string> &arguments, std::vector<std::string> &output) override;
    };
}

#endif //TRUECONTROLUSBDESCRIPTORS_REPL_H
