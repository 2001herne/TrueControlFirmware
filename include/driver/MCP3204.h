//
// Created by benjaminherne on 5/02/26.
//

#ifndef TRUECONTROLUSBDESCRIPTORS_MCP3204_H
#define TRUECONTROLUSBDESCRIPTORS_MCP3204_H

#include <cstdint>
#include <hardware/spi.h>
#include <hardware/gpio.h>
#include <iostream>
#include <bitset>

#define MODE(mode_bits_4) {(uint8_t) 1 << 2 | ((uint8_t) mode_bits_4) >> 2, (uint8_t)( (uint8_t)mode_bits_4 << 6), 0}

namespace TCFW {

    class MCP3204 {
    public:
        MCP3204(spi_inst_t *spiInst);

        enum MCP3204_Mode: uint8_t {
            CH0 = 0b1000,
            CH1 = 0b1001,
            CH2 = 0b1010,
            CH3 = 0b1011,
            CH0_CH1 = 0b0000,
            CH1_CH0 = 0b0001,
            CH2_CH3 = 0b0010,
            CH3_CH2 = 0b0011,
        };

        template <MCP3204_Mode mode>
        int read() {
            gpio_put(13, false);
            uint8_t result[3] = {0, 0, 0};
            uint8_t src[3] = MODE(mode);
            spi_write_read_blocking(spiInst, src, result, 3);
//            std::cout << '_';
//            if (mode == CH3) {
//                std::cout << std::bitset<8>(result[0]) << ' ' << std::bitset<8>(result[1]) << ' '
//                          << std::bitset<8>(result[2]);
//            }
            gpio_put(13, true);
            return (uint16_t)(result[1] & 0x0F) << 8 | result[2];
//            return 0;
        }



    private:
        spi_inst_t *spiInst;
    };

} // TCFW

#endif //TRUECONTROLUSBDESCRIPTORS_MCP3204_H
