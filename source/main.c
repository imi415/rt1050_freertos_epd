#include <stdint.h>

#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "user_tasks.h"

static void create_tasks(void) {
    if (xTaskCreate(user_task_lvgl, "LVGL", 4096, NULL, 1, &user_task_lvgl_handle) != pdPASS) goto dead_loop;

    return;

dead_loop:
    for (;;) {
        //
    }
}

int main(void) {
    BOARD_BootClockRUN();
    BOARD_InitBootPins();
    BOARD_InitBootPeripherals();

    BOARD_ConfigMPU();

    create_tasks();

    vTaskStartScheduler();

    for (;;) {
        //
    }

    return 0;
}