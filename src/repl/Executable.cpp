//
// Created by benjaminherne on 20/01/26.
//

#include <pico/bootrom.h>
#include <hardware/watchdog.h>
#include "repl/Executable.h"

namespace TCFW {
    void RebootExecutable::execute(const std::vector<std::string> &arguments, std::vector<std::string> &output) {
        if (arguments.size() >= 2 && arguments[1] == "bootsel") {
            reset_usb_boot(0, 0);
        } else {
            watchdog_reboot(0, 0, 0);
        }
    }
}
