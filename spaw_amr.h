

#ifndef __SPAW_AMR_H_
#define __SPAW_AMR_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "interf_dec.h"
#include "interf_enc.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

    void *spaw_decoder_amr_interface_init(void);
    void spaw_decoder_amr_interface_decode(void *state, const unsigned char *in, short *out, int bfi);
    void spaw_decoder_amr_interface_exit(void *state);

    int spaw_amr_decode(void *decoder_state,
                        void *encoded_data,
                        uint32_t encoded_data_len,
                        uint8_t flagOA,
                        void *decoded_data,
                        uint32_t *decoded_data_len);

#ifdef __cplusplus
}
#endif

#endif
