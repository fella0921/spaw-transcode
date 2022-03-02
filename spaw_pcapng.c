

#include "spaw_pcapng.h"
#include "spaw_comm.h"
#include "spaw_pcma.h"
#include <arpa/inet.h>
#include <unistd.h>

#include "spaw_comm.h"
#include "spaw_amr.h"
#include "spaw_amrwb.h"

// 0x00000000	Reserved ???
// 0x00000001	Interface Description Block
// 0x00000002	Packet Block
// 0x00000003	Simple Packet Block
// 0x00000004	Name Resolution Block
// 0x00000005	Interface Statistics Block
// 0x00000006	Enhanced Packet Block
// 0x00000007	IRIG Timestamp Block (requested by Gianluca Varenni <gianluca.varenni@cacetech.com>, CACE Technologies LLC)
// 0x00000008	Arinc 429 in AFDX Encapsulation Information Block (requested by Gianluca Varenni <gianluca.varenni@cacetech.com>, CACE Technologies LLC)
// 0x0A0D0D0A	Section Header Block
// 0x0A0D0A00-0x0A0D0AFF	Reserved. Used to detect trace files corrupted because of file transfers using the HTTP protocol in text mode.
// 0x000A0D0A-0xFF0A0D0A	Reserved. Used to detect trace files corrupted because of file transfers using the HTTP protocol in text mode.
// 0x000A0D0D-0xFF0A0D0D	Reserved. Used to detect trace files corrupted because of file transfers using the HTTP protocol in text mode.
// 0x0D0D0A00-0x0D0D0AFF	Reserved. Used to detect trace files corrupted because of file transfers using the FTP protocol in text mode.

#define INTERFACE_DESCRIPTION_BLOCK_TYPE 0x00000001
#define PACKET_BLOCK_TYPE 0x00000002
#define SIMPLE_PACKET_BLOCK_TYPE 0x00000003
#define NAME_RESOLUTION_BLOCK_TYPE 0x00000004
#define INTERFACE_STATISTICS_BLOCK_TYPE 0x00000005
#define ENHANCED_PACKET_BLOCK_TYPE 0x00000006
#define IRIG_TIMESTAMP_BLOCK_TYPE 0x00000007
#define ENCAPSULATION_INFORMATION_BLOCK_TYPE 0x00000008
#define SECTION_HEADER_BLOCK_TYPE 0x0A0D0D0A

int spaw_pcapng_amr2linear(ST_SPAW_CODEC_CONTEXT *pstCodecContext, unsigned char *frame, int frame_len, FILE *fp, uint8_t flagOA)
{
    short out_speech0[160];
    int slLoop = 0;

    uint32_t decoded_data_len = 0;

    int nOffset = 0;
    unsigned char *buffer = NULL;
    int nLoop = 0;

    //�������;���offset
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

    spaw_amr_decode((void *)(pstCodecContext->decoder_state), &frame[nOffset], (frame_len - nOffset), flagOA, (void *)out_speech0, &decoded_data_len);

    fwrite(out_speech0, 1, sizeof(out_speech0), fp);

    return 0;
}

int spaw_pcapng_amrwb2linear(ST_SPAW_CODEC_CONTEXT *pstCodecContext, unsigned char *frame, int frame_len, FILE *fp, uint8_t flagOA)
{
    short *psOutSpeech = NULL;
    short out_speech0[320];
    int slLoop = 0;

    uint32_t decoded_data_len = 0;

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

    if (0 != spaw_amr_wb_decode((void *)(pstCodecContext->amr_wb_decoder_state), &frame[nOffset],
                                (frame_len - nOffset), flagOA, (void *)out_speech0, &decoded_data_len))
    {

        printf("spaw_amr_wb_decode failed.\n");
        return -1;
    }

    fwrite(out_speech0, 1, decoded_data_len, fp);

    return 0;
}

int spaw_pcapng_pcm2linear(unsigned char *frame, int frame_len, FILE *fp)
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

#if 0
   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +---------------------------------------------------------------+
 0 |                    Block Type = 0x00000006                    |
   +---------------------------------------------------------------+
 4 |                      Block Total Length                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 8 |                         Interface ID                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
12 |                        Timestamp (High)                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
16 |                        Timestamp (Low)                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
20 |                         Captured Len                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
24 |                          Packet Len                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
28 /                                                               /
   /                          Packet Data                          /
   /          /* variable length, aligned to 32 bits */            /
   /                                                               /
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   /                                                               /
   /                      Options (variable)                       /
   /                                                               /
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      Block Total Length

#endif

int spaw_enhanced_packet_block_proc(ST_PCAPNG *pstPcapng, FILE *fp, int *pOutBlockTotalLen,
                                    ST_SPAW_CODEC_CONTEXT *pstCodecContext)
{
    int nOffset = 0;
    int nBlockTotalLen = 0;
    int nCapturedLen = 0;
    int nPacketLen = 0;
    unsigned char *frame = NULL;

    frame = (unsigned char *)(pstPcapng->buffer + pstPcapng->current_pos);
    // skip Block Type
    nOffset += 4;

    nBlockTotalLen = *(int *)(frame + nOffset);
    if (pstPcapng->byte_swapped)
    {
        nBlockTotalLen = ntohl(nBlockTotalLen);
    }
    // skip Block Total Length

    nOffset += 4;
    // skip Interface ID
    nOffset += 4;
    // skip Timestamp (High)
    nOffset += 4;
    // skip  Timestamp (Low)
    nOffset += 4;

    nCapturedLen = *(int *)(frame + nOffset);
    if (pstPcapng->byte_swapped)
    {
        nCapturedLen = ntohl(nCapturedLen);
    }

    // skip Captured Len
    nOffset += 4;

    // skip Packet Len

    nPacketLen = *(int *)(frame + nOffset);
    if (pstPcapng->byte_swapped)
    {
        nPacketLen = ntohl(nPacketLen);
    }
    nOffset += 4;

    switch (pstCodecContext->enAlgoType)
    {
    case SDP_ALGO_0_PCMU:
        break;
    case SDP_ALGO_8_PCMA:
        spaw_pcapng_pcm2linear((frame + nOffset), nPacketLen, fp);
        break;
    case SDP_ALGO_DYN_AMR_WB_BA:
        if (0 != spaw_pcapng_amrwb2linear(pstCodecContext, (frame + nOffset), nPacketLen, fp, 1))
        {
            return -1;
        }
        break;
    case SDP_ALGO_DYN_AMR_WB:
        if (0 != spaw_pcapng_amrwb2linear(pstCodecContext, (frame + nOffset), nPacketLen, fp, 0))
        {
            return -1;
        }
        break;
    case SDP_ALGO_DYN_AMR:
        spaw_pcapng_amr2linear(pstCodecContext, (frame + nOffset), nPacketLen, fp, 0);
        break;
    case SDP_ALGO_DYN_AMR_BA:

        spaw_pcapng_amr2linear(pstCodecContext, (frame + nOffset), nPacketLen, fp, 1);
        break;

    default:

        *pOutBlockTotalLen = nBlockTotalLen;
        return -1;
    }

    *pOutBlockTotalLen = nBlockTotalLen;

    return 0;
}

#if 0
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +---------------------------------------------------------------+
 0 |                    Block Type = 0x00000001                    |
   +---------------------------------------------------------------+
 4 |                      Block Total Length                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 8 |           LinkType            |           Reserved            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
12 |                            SnapLen                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
16 /                                                               /
   /                      Options (variable)                       /
   /                                                               /
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      Block Total Length                       |
   +---------------------------------------------------------------+
#endif

int spaw_interface_description_block_proc(ST_PCAPNG *pstPcapng, int *pOutBlockTotalLen)
{
    unsigned char *frame = NULL;
    int nOffset = 0;
    unsigned int magic;
    unsigned short link_type = 0;

    int nBlockTotalLen = 0;

    frame = (unsigned char *)(pstPcapng->buffer + pstPcapng->current_pos);

    // skip Block Type
    nOffset += 4;
    nBlockTotalLen = *(int *)(frame + nOffset);

    if (pstPcapng->byte_swapped)
    {
        nBlockTotalLen = ntohl(nBlockTotalLen);
    }

    *pOutBlockTotalLen = nBlockTotalLen;

    // skip Block Total Length
    nOffset += 4;

    link_type = *(unsigned short *)(frame + nOffset);

    if (pstPcapng->byte_swapped)
    {
        link_type = ntohl(link_type);
    }

    switch (link_type)
    {
    case WTAP_ENCAP_ETHERNET:
        NETWORK_LEN = 14;
        break;
    case WTAP_ENCAP_SLL:
        NETWORK_LEN = 16;
        break;

    default:
        printf("link_type: 0x%04x", link_type);
        break;
    }

    return 0;
}

#if 0

   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +---------------------------------------------------------------+
 0 |                   Block Type = 0x0A0D0D0A                     |
   +---------------------------------------------------------------+
 4 |                      Block Total Length                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 8 |                      Byte-Order Magic                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
12 |          Major Version        |         Minor Version         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
16 |                                                               |
   |                          Section Length                       |
   |                                                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
24 /                                                               /
   /                      Options (variable)                       /
   /                                                               /
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      Block Total Length

#endif
int spaw_section_header_block_proc(ST_PCAPNG *pstPcapng, int *pOutBlockTotalLen)
{
    unsigned char *frame = NULL;
    int nOffset = 0;
    unsigned int magic;

    int nBlockTotalLen = 0;

    frame = (unsigned char *)(pstPcapng->buffer + pstPcapng->current_pos);

    // skip Block Type
    nOffset += 4;
    nBlockTotalLen = *(int *)(frame + nOffset);
    // skip Block Total Length
    nOffset += 4;
    //
    magic = *(unsigned int *)(frame + nOffset);
    switch (magic)
    {
    case (0x1A2B3C4D):
        pstPcapng->byte_swapped = FALSE;
        break;
    case (0x4D3C2B1A):
        pstPcapng->byte_swapped = TRUE;
        nBlockTotalLen = ntohl(nBlockTotalLen);
        break;

    default:
        printf("magic: 0x%08x", magic);
        return -1;
        break;
    }

    *pOutBlockTotalLen = nBlockTotalLen;

    return 0;
}

int spaw_block_default_proc(ST_PCAPNG *pstPcapng, int *pOutBlockTotalLen)
{
    unsigned char *frame = NULL;
    int nOffset = 0;
    unsigned int magic;

    int nBlockTotalLen = 0;

    frame = (unsigned char *)(pstPcapng->buffer + pstPcapng->current_pos);

    // skip Block Type
    nOffset += 4;
    nBlockTotalLen = *(int *)(frame + nOffset);

    if (pstPcapng->byte_swapped)
    {
        nBlockTotalLen = ntohl(nBlockTotalLen);
    }

    *pOutBlockTotalLen = nBlockTotalLen;

    return 0;
}

int spaw_pcapng_open(char *in_filename, char *out_filename, void *data)
{
    int iRet = 0;
    ST_PCAPNG stPcapng;
    unsigned char *buffer = NULL;
    unsigned int len = 0;
    unsigned int size = 0;
    FILE *payload_linear_pcm = NULL;
    unsigned int ulBlockType = 0;
    int iBlockLen = 0;

    ST_SPAW_CODEC_CONTEXT *pstCodecContext = (ST_SPAW_CODEC_CONTEXT *)data;

    memset(&stPcapng, 0x00, sizeof(ST_PCAPNG));

    FILE *fp_in = fopen(in_filename, "rb");
    if (NULL == fp_in)
    {
        return -1;
    }

    if ((payload_linear_pcm = fopen(out_filename, "wb")) == NULL)
    {
        return -1;
    }

    fseek(fp_in, 0, SEEK_END);
    size = ftell(fp_in);
    rewind(fp_in);

    stPcapng.buffer = (unsigned char *)malloc(sizeof(unsigned char) * size);

    fread(stPcapng.buffer, 1, size, fp_in);

    while (len < size)
    {
        ulBlockType = *(unsigned int *)(stPcapng.buffer + len);

        switch (ulBlockType)
        {
        case INTERFACE_DESCRIPTION_BLOCK_TYPE:
            spaw_interface_description_block_proc(&stPcapng, &iBlockLen);
            break;
        case PACKET_BLOCK_TYPE:
            spaw_block_default_proc(&stPcapng, &iBlockLen);
            break;
        case SIMPLE_PACKET_BLOCK_TYPE:
            spaw_block_default_proc(&stPcapng, &iBlockLen);
            break;
        case NAME_RESOLUTION_BLOCK_TYPE:
            spaw_block_default_proc(&stPcapng, &iBlockLen);
            break;
        case INTERFACE_STATISTICS_BLOCK_TYPE:
            spaw_block_default_proc(&stPcapng, &iBlockLen);
            break;
        case ENHANCED_PACKET_BLOCK_TYPE:

            if (0 != spaw_enhanced_packet_block_proc(&stPcapng, payload_linear_pcm, &iBlockLen, pstCodecContext))
            {
                printf("spaw_enhanced_packet_block_proc failed.\n");
                goto end_proc;
            }
            break;
        case IRIG_TIMESTAMP_BLOCK_TYPE:
            spaw_block_default_proc(&stPcapng, &iBlockLen);
            break;
        case ENCAPSULATION_INFORMATION_BLOCK_TYPE:
            spaw_block_default_proc(&stPcapng, &iBlockLen);
            break;
        case SECTION_HEADER_BLOCK_TYPE:
            if (0 != spaw_section_header_block_proc(&stPcapng, &iBlockLen))
            {
                printf("spaw_section_header_block_proc failed.");
                goto end_proc;
            }

            break;

        default:
            printf("ulBlockType:0x%x\n", ulBlockType);
            goto end_proc;
        }

        len += iBlockLen;
        stPcapng.current_pos = len;
    }

end_proc:

    if (NULL != stPcapng.buffer)
    {
        free(stPcapng.buffer);
        stPcapng.buffer = NULL;
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
