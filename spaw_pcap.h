

#ifndef __SPAW_PCAP_H_
#define __SPAW_PCAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "spaw_comm.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define PCAP_MAGIC 0xa1b2c3d4
#define PCAP_SWAPPED_MAGIC 0xd4c3b2a1

typedef struct pcap_file_hdr
{
        uint32_t magic;         /* magic number */
        uint16_t version_major; /* major version number */
        uint16_t version_minor; /* minor version number */
        int32_t thiszone;       /* GMT to local correction */
        uint32_t sigfigs;       /* accuracy of timestamps */
        uint32_t snaplen;       /* max length of captured packets, in octets */
        uint32_t network;       /* data link type */
} pcap_file_hdr;

/* "libpcap" record header. */
typedef struct pcap_rec_hdr
{
        uint32_t ts_sec;   /* timestamp seconds */
        uint32_t ts_usec;  /* timestamp microseconds (nsecs for PCAP_NSEC_MAGIC) */
        uint32_t incl_len; /* number of octets of packet saved in file */
        uint32_t orig_len; /* actual length of packet */
} pcap_rec_hdr;

int spaw_pcap_open(char *in_filename, char *out_filename, void *data);

#endif
