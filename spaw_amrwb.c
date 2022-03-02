

#include "spaw_amrwb.h"
#include "dec_if.h"
#include <string.h>
#include "spaw_comm.h"

#define SPAW_AMRWB_OUT_MAX_SIZE 61
#define SPAW_AMRWB_MODES 10 /* Silence Indicator (SID) included */

#define INVALID_FRAME_TYPE(index) ((index) > SPAW_AMRWB_MODES && (index) != 0xe && (index) != 0xf) /* include SPEECH_LOST and NO_DATA*/

const int spaw_amrwb_frame_sizes[] = {17, 23, 32, 36, 40, 46, 50, 58, 60, 5, 0, 0, 0, 0, 1, 1};

void *spaw_amr_wb_interface_init(void)
{
	return D_IF_init();
}

void spaw_amr_wb_interface_decode(void *state, const unsigned char *bits, short *synth, int bfi)
{
	D_IF_decode(state, bits, synth, bfi);
	return;
}

void spaw_amr_wb_interface_exit(void *state)
{
	D_IF_exit(state);
	return;
}

static int spaw_amrwb_unpack_oa(unsigned char *buf, uint8_t *tmp, int encoded_data_len)
{
	uint8_t *tocs;
	int index;
	int framesz;

	buf++; /* CMR skip */
	tocs = buf;
	index = ((tocs[0] >> 3) & 0xf);
	buf++; /* point to voice payload */

	if (INVALID_FRAME_TYPE(index))
	{
		printf("AMRWB decoder (OA): Invalid TOC: 0x%x", index);
		return -1;
	}
	framesz = spaw_amrwb_frame_sizes[index];
	if (framesz > encoded_data_len - 1)
	{
		printf("AMRWB decoder (OA): Invalid frame size: %d\n", framesz);
		return -1;
	}
	tmp[0] = tocs[0];
	memcpy(&tmp[1], buf, framesz);

	return 0;
}

int spaw_amrwb_unpack_be(unsigned char *encoded_buf, uint8_t *tmp, int encoded_len)
{
	int framesz, index, ft;
	uint8_t shift_tocs[2] = {0x00, 0x00};
	uint8_t *shift_buf;

	memcpy(shift_tocs, encoded_buf, 2);
	/* shift for BE */
	spaw_array_lshift(4, shift_tocs, 2);
	ft = shift_tocs[0] >> 3;
	ft &= ~(1 << 5);			 /* Frame Type*/
	shift_buf = encoded_buf + 1; /* skip CMR */
	/* shift for BE */
	spaw_array_lshift(2, shift_buf, encoded_len - 1);
	/* get frame size */
	index = ((shift_tocs[0] >> 3) & 0x0f);
	if (index > 10 && index != 0xe && index != 0xf)
	{
		printf("index:%d\n", index);
		return -1;
	}
	framesz = spaw_amrwb_frame_sizes[index];
	tmp[0] = shift_tocs[0]; /* save TOC */
	memcpy(&tmp[1], shift_buf, framesz);

	return 0;
}

int spaw_amr_wb_decode(void *decoder_state,
					   void *encoded_data,
					   uint32_t encoded_data_len,
					   uint8_t flagOA,
					   void *decoded_data,
					   uint32_t *decoded_data_len)
{
	unsigned char *buf = encoded_data;
	uint8_t tmp[SPAW_AMRWB_OUT_MAX_SIZE] = {0};
	int iRet = 0;

	if (flagOA)
	{
		iRet = spaw_amrwb_unpack_oa(buf, tmp, encoded_data_len);
		if (-1 == iRet)
		{
			printf("spaw_amrwb_unpack_oa failed.\n");
			return -1;
		}
	}
	else
	{
		iRet = spaw_amrwb_unpack_be(buf, tmp, encoded_data_len);
		if (-1 == iRet)
		{
			printf("spaw_amrwb_unpack_be failed.\n");
			return -1;
		}
	}

	spaw_amr_wb_interface_decode(decoder_state, tmp, (short *)decoded_data, 0);

	// default: 320
	*decoded_data_len = 640;

	return 0;
}
