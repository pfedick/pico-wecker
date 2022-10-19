// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ------ //
// tm1637 //
// ------ //

#define tm1637_wrap_target 0
#define tm1637_wrap 30

static const uint16_t tm1637_program_instructions[] = {
            //     .wrap_target
    0xf801, //  0: set    pins, 1         side 1     
    0x80a0, //  1: pull   block                      
    0xe027, //  2: set    x, 7                       
    0xff01, //  3: set    pins, 1         side 1 [7] 
    0xe000, //  4: set    pins, 0                    
    0xf000, //  5: set    pins, 0         side 0     
    0x7001, //  6: out    pins, 1         side 0     
    0xb842, //  7: nop                    side 1     
    0x1046, //  8: jmp    x--, 6          side 0     
    0xf000, //  9: set    pins, 0         side 0     
    0xb842, // 10: nop                    side 1     
    0xb142, // 11: nop                    side 0 [1] 
    0xf800, // 12: set    pins, 0         side 1     
    0xf801, // 13: set    pins, 1         side 1     
    0x00f0, // 14: jmp    !osre, 16                  
    0x0001, // 15: jmp    1                          
    0xff01, // 16: set    pins, 1         side 1 [7] 
    0xe000, // 17: set    pins, 0                    
    0xf000, // 18: set    pins, 0         side 0     
    0xe027, // 19: set    x, 7                       
    0x7001, // 20: out    pins, 1         side 0     
    0xb842, // 21: nop                    side 1     
    0x1054, // 22: jmp    x--, 20         side 0     
    0xf000, // 23: set    pins, 0         side 0     
    0xf841, // 24: set    y, 1            side 1     
    0xb142, // 25: nop                    side 0 [1] 
    0x00f3, // 26: jmp    !osre, 19                  
    0xf800, // 27: set    pins, 0         side 1     
    0xf801, // 28: set    pins, 1         side 1     
    0x4040, // 29: in     y, 32                      
    0x8000, // 30: push   noblock                    
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program tm1637_program = {
    .instructions = tm1637_program_instructions,
    .length = 31,
    .origin = -1,
};

static inline pio_sm_config tm1637_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + tm1637_wrap_target, offset + tm1637_wrap);
    sm_config_set_sideset(&c, 2, true, false);
    return c;
}
#endif
