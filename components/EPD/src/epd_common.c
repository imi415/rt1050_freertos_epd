#include "epd_common.h"

/**
 * @brief Execute command sequence.
 * Sequence format: 1 byte parameter length, 1 byte command, [length] bytes params.
 * Parameter length does not include command itself.
 * @param cb epd_cb_t callback
 * @param user_data user pointer
 * @param seq sequence array
 * @param seq_len sequence length
 * @return epd_ret_t 
 */
epd_ret_t epd_common_execute_sequence(epd_cb_t *cb, void *user_data, uint8_t *seq, uint32_t seq_len) {
    EPD_ASSERT(cb->write_command_cb != NULL);

    uint32_t i = 0;
    while(i < seq_len) {
        EPD_ERROR_CHECK(cb->write_command_cb(user_data, &seq[i + 1], seq[i] + 1));
        i += seq[i] + 2;
    }

    return EPD_OK;
}