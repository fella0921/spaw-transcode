

#ifndef __SPAW_PCMA_H_
#define __SPAW_PCMU_H_

#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

short SPAW_Alaw2Linear(unsigned char alawbyte);

unsigned char SPAW_Linear2Alaw(short sample, int full_coding);


#endif