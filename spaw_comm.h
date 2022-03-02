

#ifndef __SPAW_COMM_H_
#define __SPAW_COMM_H_

#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define WTAP_ENCAP_ETHERNET 1
#define WTAP_ENCAP_SLL 113

    typedef enum tag_EN_SPAW_ALGORITHM_TYPE
    {
        SDP_ALGO_0_PCMU = 0,
        /*PCMU/G711Mu*/
        SDP_ALGO_8_PCMA = 8,

        SDP_ALGO_DYN_AMR_WB_BA = 96, /* AMR-WB-BA  16k sample rate  */
        SDP_ALGO_DYN_AMR_WB = 97,    /* AMR-WB-BWE 16k sample rate */
        SDP_ALGO_DYN_AMR = 99,       /* AMR-NB-BWE 8k sample rate */
        SDP_ALGO_DYN_AMR_BA = 119,   /* AMR-NB-BA  8k sample rate */

        SDP_ALGO_NULL = 128,
        SDP_ALGO_DYN_END = 128,
    } EN_SPAW_ALGORITHM_TYPE;

    typedef struct tag_ST_SPAW_CODEC_CONTEXT
    {
        EN_SPAW_ALGORITHM_TYPE enAlgoType;
        void *decoder_state;
        void *amr_wb_decoder_state;

    } ST_SPAW_CODEC_CONTEXT;

    short SATM_Alaw2Linear(unsigned char alawbyte);

    unsigned char SATM_Linear2Alaw(short sample, int full_coding);

    int spaw_array_lshift(uint8_t lshift, uint8_t *buf, int a_len);

    extern ST_SPAW_CODEC_CONTEXT m_stSpawCodecContext;

    extern int NETWORK_LEN;

#ifdef __cplusplus
}
#endif

#endif
