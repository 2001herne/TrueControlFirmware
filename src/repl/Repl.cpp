//
// Created by benjaminherne on 20/01/26.
//

#include <array>
#include <iostream>
#include <utility>
#include "repl/Repl.h"
#include "tusb.h"

namespace TCFW {
    Repl::Repl(): commands{std::make_shared<typeof(*commands)>()}{

        registerExecutable("help", std::make_shared<HelpExecutable>(commands));
        registerExecutable("reboot", std::make_shared<RebootExecutable>());
    }

    void Repl::task() {
        switch(state) {
            case READ:
                subtaskHandleInput();
                break;
            case TOKENISE:
                subtaskTokeniseInput();
                break;
            case EXECUTE:
                subtaskExecute();
                break;
            case PRINT:
                subtaskPrint();
                break;
        }
    }

    void Repl::subtaskHandleInput() {
        char inputChar = '\0';
        uint32_t bytesRead = 0;
        do {
            bytesRead = tud_cdc_read(reinterpret_cast<void*>(&inputChar), 1);
            if (bytesRead > 0) {
                switch (inputChar) {
                    case NEWLINE:
                    case RETURN:
                        std::cout << std::endl;
                        state = TOKENISE;
                        return;
                    case BACKSPACE:
                        std::cout << "\r> " << std::string(inputBuffer.length(), ' ');
                        if (!inputBuffer.empty()) {
                            inputBuffer.pop_back();
                        }
                        break;
                    default:
                        if (std::isprint(inputChar)) {
                            inputBuffer.push_back(inputChar);
                        }
                }
            }
        } while (bytesRead > 0);
        std::cout << "\r> " << inputBuffer << std::flush;
    }

    // Simplified Quoting and escape rules:
    // 1. Strings can be opened and closed by " or '. The closing char must be the same as the opening char.
    // 2. '\' is the next char literal operator. What this means is that the char following '\' is included exactly in the current token, ignoring splitting and quoting.
    // 3. ' ' splits a token when not in string or literal mode.
    void Repl::subtaskTokeniseInput() {
        bool isQuoted = false;
        bool isLiteral = false;
        char quoteChar = '\0';
        std::string token;
        for (char c : inputBuffer) {
            if (isLiteral) {
                token.push_back(c);
                isLiteral = false;
                continue;
            }
            if (isQuoted) {
                if (c == quoteChar) {
                    isQuoted = false;
                    continue;
                }
                if (c == '\\') {
                    isLiteral = true;
                    continue;
                }
                token.push_back(c);
                continue;
            }
            if (c == '\\') {
                isLiteral = true;
                continue;
            }
            if (c == '"' || c == '\'') {
                quoteChar = c;
                isQuoted = true;
                continue;
            }
            if (c == ' ') {
                commandBuffer.push_back(token);
                token.clear();
                continue;
            }
            token.push_back(c);
        }
        commandBuffer.push_back(token);
        inputBuffer.clear();
        state = EXECUTE;
    }

    void Repl::subtaskExecute() {
        outputBuffer.clear();
        if (commands->contains(commandBuffer[0])) {
            commands->operator[](commandBuffer[0])->execute(commandBuffer, outputBuffer);
        } else {
            FallbackExecutable().execute(commandBuffer, outputBuffer);
        }
        commandBuffer.clear();
        state = PRINT;
    }

    void Repl::subtaskPrint() {
        for (const auto& line : outputBuffer) {
            std::cout << ">> " << line << std::endl;
        }
        state = READ;
    }

    void Repl::registerExecutable(const std::string &name, std::shared_ptr<Executable> executable) {
        commands->operator[](name) = std::move(executable);
    }

    void FallbackExecutable::execute(const std::vector<std::string> &arguments, std::vector<std::string> &output) {
        output.emplace_back("No such executable. Check the command again, or type 'help' to see available commands.");
        output.insert(output.end(), arguments.begin(), arguments.end());
    }

    HelpExecutable::HelpExecutable(
            std::weak_ptr<const std::map<std::string, std::shared_ptr<Executable>>> validExecutables):
            executables{std::move(validExecutables)}{}

    void HelpExecutable::execute(const std::vector<std::string> &arguments, std::vector<std::string> &output) {
        output.emplace_back("Valid executables include:");
        for (const auto& executable : *executables.lock()) {
            output.push_back(std::string("- ") + executable.first);
        }
    }
}
