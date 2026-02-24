//
// Created by benjaminherne on 6/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_LAPTIMER_H
#define TRUECONTROLUSBDESCRIPTORS_LAPTIMER_H

#include <vector>
#include <bsp/board_api.h>
#include <string>

#ifndef NDEBUG

extern std::vector<std::pair<std::string,uint32_t>> lapTimerSteps;

#define LAP_TIMER_CLEAR() lapTimerSteps.clear(); lapTimerSteps.emplace_back("", board_millis())
#define LAP_TIMER_CHECKPOINT(name) lapTimerSteps.emplace_back(name, board_millis())
#define LAP_TIMER_METRICS() printf("\n");                                                        \
for (auto timestamp = lapTimerSteps.begin() + 1; timestamp < lapTimerSteps.end(); timestamp++) { \
    auto delta = timestamp->second - (timestamp - 1)->second;                                    \
    printf("%s: %lu, ", timestamp->first.c_str(), delta);                                        \
}                                                                                                \
printf("\n")

#endif

#endif //TRUECONTROLUSBDESCRIPTORS_LAPTIMER_H
