#include <stdint.h>

#include "board.h"
#include "pin_mux.h"
#include "peripherals.h"

int main(void) {
    BOARD_BootClockRUN();
    BOARD_InitBootPins();
    BOARD_InitBootPeripherals();

    BOARD_ConfigMPU();

    for(;;)  {
        //
    }

    return 0;
}