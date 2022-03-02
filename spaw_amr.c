
#include "spaw_amr.h"
#include "interf_dec.h"
#include "interf_enc.h"
#include <stdlib.h>
#include <string.h>
#include "spaw_comm.h"

const int spaw_amr_frame_sizes[] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 1};

#define SPAW_AMR_OUT_MAX_SIZE 32
#define SPAW_AMR_MODES 9 /* plus SID */

void *spaw_decoder_amr_interface_init(void)
{
	return Decoder_Interface_init();
}

void spaw_decoder_amr_interface_decode(void *state, const unsigned char *in, short *out, int bfi)
{
	Decoder_Interface_Decode(state, in, out, bfi);

	return;
}

void spaw_decoder_amr_interface_exit(void *state)
{
	Decoder_Interface_exit(state);

	return;
}

static int spaw_amr_unpack_oa(unsigned char *buf, uint8_t *tmp, int encoded_data_len)
{
	uint8_t *tocs;
	int index;
	int framesz;

	if (!buf)
	{
		return -1;
	}
	buf++; /* CMR skip */
	tocs = buf;
	index = ((tocs[0] >> 3) & 0xf);
	buf++; /* point to voice payload */
	if (index > SPAW_AMR_MODES && index != 0xf)
	{
		printf("AMR decoder (OA): Invalid Table Of Contents (TOC): 0x%x\n", index);
		return -1;
	}
	framesz = spaw_amr_frame_sizes[index];
	if (framesz > encoded_data_len - 1)
	{
		printf("AMR decoder (OA): Invalid frame size: %d\n", framesz);
		return -1;
	}
	tmp[0] = tocs[0];
	memcpy(&tmp[1], buf, framesz);

	return 0;
}

int spaw_amr_unpack_be(unsigned char *encoded_buf, uint8_t *tmp, int encoded_len)
{
	int framesz, index;
	uint8_t shift_tocs[2] = {0x00, 0x00};
	uint8_t *shift_buf;

	memcpy(shift_tocs, encoded_buf, 2);
	/* shift for BE */
	spaw_array_lshift(4, shift_tocs, 2);
	shift_buf = encoded_buf + 1; /* skip CMR */
	/* shift for BE */
	spaw_array_lshift(2, shift_buf, encoded_len - 1);
	/* get frame size */
	index = ((shift_tocs[0] >> 3) & 0x0f);
	if (index > 9 && index != 0xf)
	{
		return -1;
	}
	framesz = spaw_amr_frame_sizes[index];
	tmp[0] = shift_tocs[0]; /* save TOC */
	memcpy(&tmp[1], shift_buf, framesz);

	return 0;
}

int spaw_amr_decode(void *decoder_state,
					void *encoded_data,
					uint32_t encoded_data_len,
					uint8_t flagOA,
					void *decoded_data,
					uint32_t *decoded_data_len)
{
	uint8_t *buf = encoded_data;
	uint8_t tmp[SPAW_AMR_OUT_MAX_SIZE];

	if (flagOA)
	{
		/* Octed Aligned */
		if (-1 == spaw_amr_unpack_oa(buf, tmp, encoded_data_len))
		{
			printf("spaw_amr_unpack_oa failed.");
			return -1;
		}
	}
	else
	{
		/* Bandwidth Efficient */
		if (-1 == spaw_amr_unpack_be(buf, tmp, encoded_data_len))
		{
			printf("spaw_amr_unpack_be failed.");
		}
	}

	spaw_decoder_amr_interface_decode(decoder_state, tmp, (short *)decoded_data, 0);

	*decoded_data_len = 320;

	return 0;
}
