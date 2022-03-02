
#include <stdio.h>
#include <arpa/inet.h>
#include "spaw_pcap.h"
#include "spaw_pcapng.h"
#include <string.h>
#include <stdlib.h>
#include "spaw_comm.h"
#include "spaw_amr.h"
#include "spaw_amrwb.h"

typedef int (*open_routine_func)(char *in_filename, char *out_filename, void *data);

typedef struct tag_ST_OPEN_INFO
{
    const char *name;
    const char *suffix;
    open_routine_func function;
    void *data; /*  */
} ST_OPEN_INFO;

#define SPAW_MAX_SUPPORT_FILE_TYPE 8

typedef struct ST_SPAW_SUPPORT_FILE_TYPE
{
    int procedure_counts;

    ST_OPEN_INFO procedures[SPAW_MAX_SUPPORT_FILE_TYPE];
} ST_SPAW_SUPPORT_FILE_TYPE;

ST_SPAW_SUPPORT_FILE_TYPE g_stSpawSupportFileType;

ST_SPAW_CODEC_CONTEXT m_stSpawCodecContext;

static int spaw_register_func(open_routine_func function_pointer,
                              char *name,
                              char *suffix,
                              void *data)
{
    int nCount = 0;

    //
    if (!(function_pointer) || !(name))
    {
        return -1;
    }

    if (g_stSpawSupportFileType.procedure_counts >= SPAW_MAX_SUPPORT_FILE_TYPE)
    {
        return -1;
    }

    nCount = g_stSpawSupportFileType.procedure_counts;

    g_stSpawSupportFileType.procedures[nCount].function = function_pointer;
    g_stSpawSupportFileType.procedures[nCount].name = name;
    g_stSpawSupportFileType.procedures[nCount].suffix = suffix;

    g_stSpawSupportFileType.procedures[nCount].data = data;

    g_stSpawSupportFileType.procedure_counts++;

    return 0;
}

int spaw_init_file_type(void)
{

    g_stSpawSupportFileType.procedure_counts = 0;

    memset(g_stSpawSupportFileType.procedures, 0x00, sizeof(g_stSpawSupportFileType.procedures));

    return 0;
}

int spaw_init_register(void *data)
{
    int nRet = 0;

    spaw_init_file_type();

    spaw_register_func(spaw_pcap_open, "spaw_pcap_open", ".pcap", data);

    spaw_register_func(spaw_pcapng_open, "spaw_pcapng_open", ".pcapng", data);

    return nRet;
}

int spaw_get_file_type(char *file_name, char *out_suffix)
{
    int nLoop = 0;
    int file_name_len = strlen(file_name);
    int end_pos = file_name_len - 1;
    int start_pos = -1;
    int current_pos = end_pos;

    while (current_pos >= 0)
    {
        if ('.' == file_name[current_pos])
        {
            start_pos = current_pos;
            break;
        }

        current_pos--;
    }

    if (start_pos == -1)
    {
        return -1;
    }

    memcpy(out_suffix, &(file_name[start_pos]), end_pos - start_pos + 1);
    out_suffix[end_pos - start_pos + 1] = '\0';

    return 0;
}

int spaw_func_call_back(char *input_name, char *output_name, void *data)
{
    int nLoop = 0;
    int nFuncIndex = 0;
    int nRet = 0;
    unsigned char aucSuffix[128] = {0};

    if (0 != spaw_get_file_type(input_name, aucSuffix))
    {
        return -1;
    }

    for (nLoop = 0; nLoop < g_stSpawSupportFileType.procedure_counts; nLoop++)
    {
        nRet = strncmp(aucSuffix, g_stSpawSupportFileType.procedures[nLoop].suffix, strlen(aucSuffix));

        if (0 == nRet)
        {
            // find
            nFuncIndex = nLoop;
            break;
        }
    }

    if (nLoop >= g_stSpawSupportFileType.procedure_counts)
    {
        // not find
        return -1;
    }

    nRet = g_stSpawSupportFileType.procedures[nFuncIndex].function(input_name, output_name, data);

    return nRet;
}

EN_SPAW_ALGORITHM_TYPE spaw_get_algorithm_type(char *algo_name)
{
    if (strlen("pcma") == strlen(algo_name) &&
        0 == strncmp("pcma", algo_name, strlen(algo_name)))
    {
        return SDP_ALGO_8_PCMA;
    }
    else if (strlen("amr_wb_ba") == strlen(algo_name) &&
             (0 == strncmp("amr_wb_ba", algo_name, strlen(algo_name))))
    {
        return SDP_ALGO_DYN_AMR_WB_BA;
    }
    else if (strlen("amr_wb") == strlen(algo_name) &&
             0 == strncmp("amr_wb", algo_name, strlen(algo_name)))
    {
        return SDP_ALGO_DYN_AMR_WB;
    }
    else if (strlen("amr") == strlen(algo_name) &&
             0 == strncmp("amr", algo_name, strlen(algo_name)))
    {
        return SDP_ALGO_DYN_AMR;
    }
    else if (strlen("amr_ba") == strlen(algo_name) &&
             0 == strncmp("amr_ba", algo_name, strlen(algo_name)))
    {
        return SDP_ALGO_DYN_AMR;
    }
    else
    {
        printf("Unsupported algo_name:%s", algo_name);
    }

    return SDP_ALGO_NULL;
}

int main(int argc, char *argv[])
{
    EN_SPAW_ALGORITHM_TYPE enAlgoType = SDP_ALGO_NULL;

    if (argc != 4)
    {
        printf("argc is Invalid. argc:%d\n", argc);
        return -1;
    }

    enAlgoType = spaw_get_algorithm_type(argv[3]);

    // get algo type
    if (SDP_ALGO_NULL == enAlgoType)
    {
        return -1;
    }

    m_stSpawCodecContext.decoder_state = spaw_decoder_amr_interface_init();
    m_stSpawCodecContext.amr_wb_decoder_state = spaw_amr_wb_interface_init();

    m_stSpawCodecContext.enAlgoType = enAlgoType;

    spaw_init_register(&m_stSpawCodecContext);


    // argv[1]
 
    // argv[2]
    // spaw_pcap_open(argv[1], argv[2]);
    spaw_func_call_back(argv[1], argv[2], &m_stSpawCodecContext);

    spaw_decoder_amr_interface_exit(m_stSpawCodecContext.decoder_state);
    spaw_amr_wb_interface_exit(m_stSpawCodecContext.amr_wb_decoder_state);

    return 0;
}
