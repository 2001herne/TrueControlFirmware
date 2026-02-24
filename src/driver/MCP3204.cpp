//
// Created by benjaminherne on 5/02/26.
//

#include "driver/MCP3204.h"

namespace TCFW {
    MCP3204::MCP3204(spi_inst_t* spiInst): spiInst{spiInst} {
        spi_init(spiInst, 20000);
        gpio_set_function(10, GPIO_FUNC_SPI);
        gpio_set_function(11, GPIO_FUNC_SPI);
        gpio_set_function(12, GPIO_FUNC_SPI);
        gpio_set_function(13, GPIO_FUNC_SIO);

//        gpio_set_function(10, GPIO_FUNC_SIO);
//        gpio_set_function(11, GPIO_FUNC_SIO);
//        gpio_set_function(12, GPIO_FUNC_SIO);
//        gpio_set_function(13, GPIO_FUNC_SIO);

//        gpio_init(10);
//        gpio_init(11);
//        gpio_init(12);
        gpio_init(13);

//        gpio_set_dir(10, GPIO_OUT);
//        gpio_set_dir(11, GPIO_OUT);
//        gpio_set_dir(12, GPIO_OUT);
        gpio_set_dir(13, GPIO_OUT);

//        gpio_put(10, true);
//        gpio_put(11, true);
//        gpio_put(12, true);
        gpio_put(13, true);


    }


} // TCFW