

#ifndef __SPAW_AMR_WB_H_
#define __SPAW_AMR_WB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

    void *spaw_amr_wb_interface_init(void);
    void spaw_amr_wb_interface_exit(void *state);

    int spaw_amr_wb_decode(void *decoder_state,
                           void *encoded_data,
                           uint32_t encoded_data_len,
                           uint8_t flagOA,
                           void *decoded_data,
                           uint32_t *decoded_data_len);

#ifdef __cplusplus
}
#endif

#endif
