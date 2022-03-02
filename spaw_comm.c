
#include "spaw_comm.h"
#include <string.h>

int NETWORK_LEN = 14;

int spaw_array_lshift(uint8_t lshift, uint8_t *buf, int a_len)
{
	int i = 0;
	uint8_t first_byte;
	uint8_t second_byte;

	if (!buf || !a_len)
		return (-1);

	if ((lshift < 0) || (lshift > 8))
		return (-1);

	first_byte = 0xFF >> lshift;
	second_byte = ~first_byte;

	for (i = 1; i < a_len; i++)
	{
		buf[i - 1] = ((buf[i - 1] & first_byte) << lshift) |
					 ((buf[i] & second_byte) >> (8 - lshift));
	}
	i--;
	buf[i] = ((buf[i] & first_byte) << lshift);
	return (0);
}
