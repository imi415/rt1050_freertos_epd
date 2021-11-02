#ifndef EPD_PRIVATE_H
#define EPD_PRIVATE_H

#include "epd_common.h"

epd_ret_t epd_common_execute_sequence(epd_cb_t *cb, void *user_data, uint8_t *seq, uint32_t seq_len);

#endif