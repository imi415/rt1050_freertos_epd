#ifndef USER_TASKS_H
#define USER_TASKS_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern TaskHandle_t user_task_lvgl_handle;
void user_task_lvgl(void *pvParameters);

#endif