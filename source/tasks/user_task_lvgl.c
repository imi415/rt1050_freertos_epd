#include "epd_wfh0420cz35.h"
#include "lvgl.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "user_tasks.h"

#include "epd_test_image_1.h"

#define LVGL_HANDLER_MS 100
#define EPD_BUF_SIZE    (400 * 300 / 8)

// Static functions
static epd_ret_t user_epd_delay_cb(void *handle, uint32_t msec);
static epd_ret_t user_epd_reset_cb(void *handle);
static epd_ret_t user_epd_poll_busy_cb(void *handle);
static epd_ret_t user_epd_write_command_cb(void *handle, uint8_t *command, uint32_t len);
static epd_ret_t user_epd_write_data_cb(void *handle, uint8_t *data, uint32_t len);

// Static and global variables
static epd_wfh0420_t s_epd = {.cb        = {.delay_cb         = user_epd_delay_cb,
                                            .reset_cb         = user_epd_reset_cb,
                                            .poll_busy_cb     = user_epd_poll_busy_cb,
                                            .write_command_cb = user_epd_write_command_cb,
                                            .write_data_cb    = user_epd_write_data_cb},
                              .user_data = NULL};

static uint8_t bw_buf[EPD_BUF_SIZE];
static uint8_t red_buf[EPD_BUF_SIZE];

SemaphoreHandle_t user_task_lvgl_busy_mutex;

TaskHandle_t user_task_lvgl_handle = NULL;

void user_task_lvgl(void *pvParameters) {
    epd_coord_t coord = {
        .x_start = 0,
        .x_end   = 399,
        .y_start = 0,
        .y_end   = 299,
    };

    // Flush EPD to red.
    memset(bw_buf, 0xFF, EPD_BUF_SIZE);
    memset(red_buf, 0x00, EPD_BUF_SIZE);

    user_task_lvgl_busy_mutex = xSemaphoreCreateBinary();
    if(user_task_lvgl_busy_mutex == NULL) {
        for(;;) {
            //
        }
    }

    epd_wfh0420_upload(&s_epd, &coord, epd_bitmap_57852730_p0_4indexed, bw_buf);

    coord.x_start = 0;
    coord.x_end = 239;
    coord.y_start = 0;
    coord.y_end = 239;

    vTaskDelay(pdMS_TO_TICKS(60000));

    //epd_wfh0420_upload(&s_epd, &coord, epd_bitmap_79157131_p0, bw_buf);

    // lv_init();
    for (;;) {
        // lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(LVGL_HANDLER_MS));
    }
}

static epd_ret_t user_epd_delay_cb(void *handle, uint32_t msec) {
    vTaskDelay(pdMS_TO_TICKS(msec));
    return EPD_OK;
}

static epd_ret_t user_epd_reset_cb(void *handle) {
    // Toggle Reset GPIO.
    vTaskDelay(pdMS_TO_TICKS(10));
    GPIO_WritePinOutput(BOARD_INITPINS_EPD_RES_GPIO, BOARD_INITPINS_EPD_RES_GPIO_PIN, 0x00);
    vTaskDelay(pdMS_TO_TICKS(10));
    GPIO_WritePinOutput(BOARD_INITPINS_EPD_RES_GPIO, BOARD_INITPINS_EPD_RES_GPIO_PIN, 0x01);
    vTaskDelay(pdMS_TO_TICKS(10));
    GPIO_WritePinOutput(BOARD_INITPINS_EPD_RES_GPIO, BOARD_INITPINS_EPD_RES_GPIO_PIN, 0x00);
    vTaskDelay(pdMS_TO_TICKS(10));
    GPIO_WritePinOutput(BOARD_INITPINS_EPD_RES_GPIO, BOARD_INITPINS_EPD_RES_GPIO_PIN, 0x01);
    vTaskDelay(pdMS_TO_TICKS(10));
    return EPD_OK;
}

static epd_ret_t user_epd_poll_busy_cb(void *handle) {
    // if busy high then return, else wait for GPIO rising edge interrupt.
    if(GPIO_ReadPinInput(BOARD_INITPINS_EPD_BUSY_GPIO, BOARD_INITPINS_EPD_BUSY_GPIO_PIN)) {
        return EPD_OK;
    }

    xSemaphoreTake(user_task_lvgl_busy_mutex, portMAX_DELAY);

    return EPD_OK;
}

static epd_ret_t user_epd_write_command_cb(void *handle, uint8_t *command, uint32_t len) {
    // SPI write first byte as command (D/C low), rest bytes as data (D/C high)
    lpspi_transfer_t xfer = {
        .dataSize = 0x01,
        .txData = command,
        .rxData = NULL,
    };

    GPIO_WritePinOutput(BOARD_INITPINS_EPD_DC_GPIO, BOARD_INITPINS_EPD_DC_GPIO_PIN, 0x00);
    if(LPSPI_MasterTransferBlocking(LPSPI1, &xfer) != kStatus_Success) {
        return EPD_FAIL;
    }

    if(len < 2) return EPD_OK; 

    xfer.txData = &command[1];
    xfer.dataSize = len - 1;

    GPIO_WritePinOutput(BOARD_INITPINS_EPD_DC_GPIO, BOARD_INITPINS_EPD_DC_GPIO_PIN, 0x01);
    if(LPSPI_MasterTransferBlocking(LPSPI1, &xfer) != kStatus_Success) {
        return EPD_FAIL;
    }

    return EPD_OK;
}

static epd_ret_t user_epd_write_data_cb(void *handle, uint8_t *data, uint32_t len) {
    // SPI write data (D/C high)
    lpspi_transfer_t xfer = {
        .dataSize = len,
        .txData = data,
        .rxData = NULL,
    };

    GPIO_WritePinOutput(BOARD_INITPINS_EPD_DC_GPIO, BOARD_INITPINS_EPD_DC_GPIO_PIN, 0x01);
    if(LPSPI_MasterTransferBlocking(LPSPI1, &xfer) != kStatus_Success) {
        return EPD_FAIL;
    }

    return EPD_OK;
}

/* GPIO3_Combined_16_31_IRQn interrupt handler */
void GPIO3_GPIO_COMB_16_31_IRQHANDLER(void) {

    uint32_t pins_flags = GPIO_GetPinsInterruptFlags(GPIO3); 
    GPIO_ClearPinsInterruptFlags(GPIO3, pins_flags); 

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(user_task_lvgl_busy_mutex, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}