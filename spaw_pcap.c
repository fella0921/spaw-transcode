

#include "spaw_pcap.h"
#include "spaw_pcma.h"
#include <arpa/inet.h>
#include <unistd.h>
#include "spaw_amr.h"
#include "spaw_amrwb.h"

int spaw_pcap_file_header(pcap_file_hdr *pfhdr)
{
    if (NULL == pfhdr)
    {
        return -1;
    }

    printf("=====================\n"
           "magic:0x%0x\n"
           "version_major:0x%0x\n"
           "version_minor:0x%0x\n"
           "thiszone:0x%0x\n"
           "sigfigs:0x%0x\n"
           "snaplen:0x%0x\n"
           "network:0x%0x\n"
           "=====================\n",
           pfhdr->magic,
           pfhdr->version_major,
           pfhdr->version_minor,
           pfhdr->thiszone,
           pfhdr->sigfigs,
           pfhdr->snaplen,
           pfhdr->network);

    return 0;
}

int spaw_pcap_header(pcap_rec_hdr *prhdr)
{
    if (NULL == prhdr)
    {
        return -1;
    }

    printf("=====================\n"
           "ts.timestamp_s:0x%0x\n"
           "ts.timestamp_ms:0x%0x\n"
           "capture_len:0x%0x\n"
           "len:0x%0x\n"
           "=====================\n",
           prhdr->ts_sec,
           prhdr->ts_usec,
           prhdr->incl_len,
           prhdr->orig_len);
}

int spaw_pcap_pcm2linear(unsigned char *frame, int frame_len, FILE *fp)
{
    short *psOutSpeech = NULL;

    short in_speech0[160];
    int slLoop = 0;

    int nOffset = 0;
    unsigned char *buffer = NULL;
    int nLoop = 0;
    // skip Linux cooked capture
    for (nLoop = 0; nLoop < NETWORK_LEN; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");

    nOffset += NETWORK_LEN;
    for (nLoop = 0; nLoop < 20; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");
    // skip ip
    nOffset += 20;
    for (nLoop = 0; nLoop < 8; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");
    // skip udp
    nOffset += 8;
    for (nLoop = 0; nLoop < 12; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");
    // skip rtp
    nOffset += 12;

    printf("payloadLen:%d\n", frame_len - nOffset);
    for (nLoop = 0; nLoop < 160 / 16; nLoop++)
    {
        printf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x,\n",
               frame[16 * nLoop + 0 + nOffset],
               frame[16 * nLoop + 1 + nOffset],
               frame[16 * nLoop + 2 + nOffset],
               frame[16 * nLoop + 3 + nOffset],
               frame[16 * nLoop + 4 + nOffset],
               frame[16 * nLoop + 5 + nOffset],
               frame[16 * nLoop + 6 + nOffset],
               frame[16 * nLoop + 7 + nOffset],
               frame[16 * nLoop + 8 + nOffset],
               frame[16 * nLoop + 9 + nOffset],
               frame[16 * nLoop + 10 + nOffset],
               frame[16 * nLoop + 11 + nOffset],
               frame[16 * nLoop + 12 + nOffset],
               frame[16 * nLoop + 13 + nOffset],
               frame[16 * nLoop + 14 + nOffset],
               frame[16 * nLoop + 15 + nOffset]);
    }

    psOutSpeech = (short *)malloc((frame_len - nOffset) * sizeof(short));
    // proc payload
    for (nLoop = 0; nLoop < (frame_len - nOffset); nLoop++)
    {
        psOutSpeech[nLoop] = SPAW_Alaw2Linear(frame[nOffset + nLoop]);
    }

    fwrite(psOutSpeech, 1, (frame_len - nOffset) * sizeof(short), fp);

    if (NULL != psOutSpeech)
    {
        free(psOutSpeech);
        psOutSpeech = NULL;
    }

    return 0;
}

int spaw_pcap_amr2linear(ST_SPAW_CODEC_CONTEXT *pstCodecContext, unsigned char *frame, int frame_len, FILE *fp, uint8_t flagOA)
{
    short out_speech0[160];
    int slLoop = 0;

    int nOffset = 0;
    unsigned char *buffer = NULL;
    int nLoop = 0;
    uint32_t decoded_data_len = 0;

    // skip Linux cooked capture
    for (nLoop = 0; nLoop < NETWORK_LEN; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");

    nOffset += NETWORK_LEN;

    for (nLoop = 0; nLoop < 20; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");
    // skip ip
    nOffset += 20;
    for (nLoop = 0; nLoop < 8; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");
    // skip udp
    nOffset += 8;
    for (nLoop = 0; nLoop < 12; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");
    // skip rtp
    nOffset += 12;

    spaw_amr_decode((void *)pstCodecContext->decoder_state, (void *)&(frame[nOffset]), (frame_len - nOffset), flagOA, (void *)out_speech0, &decoded_data_len);

    fwrite(out_speech0, 1, sizeof(out_speech0), fp);

    return 0;
}

int spaw_pcap_amrwb2linear(ST_SPAW_CODEC_CONTEXT *pstCodecContext, unsigned char *frame, int frame_len, FILE *fp, uint8_t flagOA)
{
    short out_speech0[320];
    int slLoop = 0;
    int count = 0;

    int nOffset = 0;
    unsigned char *buffer = NULL;
    int nLoop = 0;
    uint32_t decoded_data_len = 0;

    // skip Linux cooked capture
    for (nLoop = 0; nLoop < NETWORK_LEN; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");
    nOffset += NETWORK_LEN;

    for (nLoop = 0; nLoop < 20; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");
    // skip ip
    nOffset += 20;
    for (nLoop = 0; nLoop < 8; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");
    // skip udp
    nOffset += 8;
    for (nLoop = 0; nLoop < 12; nLoop++)
    {
        printf("0x%02x, ", frame[nLoop + nOffset]);
    }
    printf("\n");
    // skip rtp
    nOffset += 12;

    if (-1 == spaw_amr_wb_decode((void *)(pstCodecContext->amr_wb_decoder_state), (void *)&(frame[nOffset]),
                                 (frame_len - nOffset), flagOA, (void *)out_speech0, &decoded_data_len))
    {
        printf("spaw_amr_wb_decode failed.\n");
        return -1;
    }

    count = fwrite(out_speech0, 1, decoded_data_len, fp);
    printf("count:%d\n", count);

    return 0;
}

int spaw_pcap_open(char *in_filename, char *out_filename, void *data)
{
    pcap_file_hdr *pstfilehdr = NULL;
    pcap_rec_hdr *prhdr = NULL;
    unsigned char *buffer = NULL;
    unsigned int len = 0;
    unsigned int size = 0;
    int byte_swapped = FALSE;
    int nPacketLen = 0;
    int nReadCount = 0;
    int nOffset = 0;
    FILE *payload_linear_pcm = NULL;
    int nPacketStat = 1;
    ST_SPAW_CODEC_CONTEXT *pstCodecContext = (ST_SPAW_CODEC_CONTEXT *)data;
    unsigned int network = 0;

    FILE *fp_in = fopen(in_filename, "rb");
    if (NULL == fp_in)
    {
        printf("fopen in_filename[%s]\n", in_filename);
        return -1;
    }

    if ((payload_linear_pcm = fopen(out_filename, "wb")) == NULL)
    {
        printf("fopen out_filename[%s]\n", out_filename);
        return -1;
    }

    fseek(fp_in, 0, SEEK_END);
    size = ftell(fp_in);
    rewind(fp_in);

    buffer = (unsigned char *)malloc(sizeof(unsigned char) * size);
    if (NULL == buffer)
    {
        printf("malloc failed.\n");
        return -1;
    }

    fread(buffer, 1, size, fp_in);

    // file header.
    pstfilehdr = (pcap_file_hdr *)(buffer + len);
    spaw_pcap_file_header(pstfilehdr);
    len += sizeof(pcap_file_hdr);

    switch (pstfilehdr->magic)
    {
    case PCAP_MAGIC:
        byte_swapped = FALSE;
        break;
    case PCAP_SWAPPED_MAGIC:
        byte_swapped = TRUE;
        break;
    default:
        printf("unsupported magic:0x%x", pstfilehdr->magic);
        break;
    }

    while (len < size)
    {
        prhdr = (pcap_rec_hdr *)(buffer + len);
        spaw_pcap_header(prhdr);
        len += sizeof(pcap_rec_hdr);

        nPacketLen = prhdr->orig_len;
        network = pstfilehdr->network;
        if (byte_swapped)
        {
            nPacketLen = ntohl(nPacketLen);
            network = ntohl(pstfilehdr->network);
        }

        switch (network)
        {
        case WTAP_ENCAP_ETHERNET:
            NETWORK_LEN = 14;
            break;
        case WTAP_ENCAP_SLL:
            NETWORK_LEN = 16;
            break;

        default:
            printf("unsupported network:0x%x", network);
            return -1;
        }

        switch (pstCodecContext->enAlgoType)
        {
        case SDP_ALGO_0_PCMU:
            break;
        case SDP_ALGO_8_PCMA:
            printf("====================================================nPacketStat:%d===============================\n", nPacketStat++);
            spaw_pcap_pcm2linear(buffer + len, nPacketLen, payload_linear_pcm);
            len += nPacketLen;
            break;
        case SDP_ALGO_DYN_AMR_WB_BA:
            printf("====================================================nPacketStat:%d===============================\n", nPacketStat++);
            if (-1 == spaw_pcap_amrwb2linear(pstCodecContext, buffer + len, nPacketLen, payload_linear_pcm, 1))
            {
                goto end_proc;
                return -1;
            }
            len += nPacketLen;
            break;
        case SDP_ALGO_DYN_AMR_WB:
            printf("====================================================nPacketStat:%d===============================\n", nPacketStat++);
            if (-1 == spaw_pcap_amrwb2linear(pstCodecContext, buffer + len, nPacketLen, payload_linear_pcm, 0))
            {
                goto end_proc;
                return -1;
            }
            len += nPacketLen;
            break;
        case SDP_ALGO_DYN_AMR:

            printf("====================================================nPacketStat:%d===============================\n", nPacketStat++);
            spaw_pcap_amr2linear(pstCodecContext, buffer + len, nPacketLen, payload_linear_pcm, 0);
            len += nPacketLen;
            break;
        case SDP_ALGO_DYN_AMR_BA:

            printf("====================================================nPacketStat:%d===============================\n", nPacketStat++);

            spaw_pcap_amr2linear(pstCodecContext, buffer + len, nPacketLen, payload_linear_pcm, 1);
            len += nPacketLen;
            break;

        default:
            goto end_proc;
            return -1;
        }
    }

end_proc:

    if (NULL != buffer)
    {
        free(buffer);
        buffer = NULL;
    }

    if (NULL != payload_linear_pcm)
    {
        fclose(payload_linear_pcm);
        payload_linear_pcm = NULL;
    }

    if (NULL != fp_in)
    {
        fclose(fp_in);
        fp_in = NULL;
    }

    return 0;
}
