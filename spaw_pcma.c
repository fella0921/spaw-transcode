

#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define AST_ALAW_AMI_MASK 0x55
#define AST_ALAW_SIGN_BIT 0x80

#define SIGN_BIT (0x80) /* A-law ����λ  */

static void
ast_alaw_get_sign_mag(short sample, unsigned *sign, unsigned *mag)
{
   /* It may look illogical to retrive the sign this way in both cases,
    * but this helps gcc eliminate the branch below and produces
    * faster code */
   *sign = ((unsigned short)sample >> 8) & AST_ALAW_SIGN_BIT;
   //#if defined(G711_REDUCED_BRANCHING)
   {
      unsigned dual_mag = (-sample << 16) | (unsigned short)sample;
      *mag = (dual_mag >> (*sign >> 3)) & 0xffffU;
   }
   //#else
   //   if (sample < 0)
   //      *mag = -sample;
   //   else
   //      *mag = sample;
   //#endif /* G711_REDUCED_BRANCHING */
   *sign ^= AST_ALAW_SIGN_BIT;
}

static short alaw2linear(unsigned char alawbyte)
{
   unsigned exponent, mantissa;
   short sample;

   alawbyte ^= AST_ALAW_AMI_MASK;
   exponent = (alawbyte & 0x70) >> 4;
   mantissa = alawbyte & 0x0f;
   sample = (mantissa << 4) + 8 /* rounding error */;
   if (exponent)
      sample = (sample + 0x100) << (exponent - 1);
   if (!(alawbyte & 0x80))
      sample = -sample;
   return sample;
}

static unsigned char linear2alaw(short sample, int full_coding)
{
   static const unsigned exp_lut[128] = {
       1, 1, 2, 2, 3, 3, 3, 3,
       4, 4, 4, 4, 4, 4, 4, 4,
       5, 5, 5, 5, 5, 5, 5, 5,
       5, 5, 5, 5, 5, 5, 5, 5,
       6, 6, 6, 6, 6, 6, 6, 6,
       6, 6, 6, 6, 6, 6, 6, 6,
       6, 6, 6, 6, 6, 6, 6, 6,
       6, 6, 6, 6, 6, 6, 6, 6,
       7, 7, 7, 7, 7, 7, 7, 7,
       7, 7, 7, 7, 7, 7, 7, 7,
       7, 7, 7, 7, 7, 7, 7, 7,
       7, 7, 7, 7, 7, 7, 7, 7,
       7, 7, 7, 7, 7, 7, 7, 7,
       7, 7, 7, 7, 7, 7, 7, 7,
       7, 7, 7, 7, 7, 7, 7, 7,
       7, 7, 7, 7, 7, 7, 7, 7};
   unsigned sign, exponent, mantissa, mag;
   unsigned char alawbyte;

   ast_alaw_get_sign_mag(sample, &sign, &mag);
   if (mag > 32767)
      mag = 32767; /* clip the magnitude for -32768 */

   exponent = exp_lut[(mag >> 8) & 0x7f];
   mantissa = (mag >> (exponent + 3)) & 0x0f;
   if (mag < 0x100)
      exponent = 0;

   if (full_coding)
   {
      /* full encoding, with sign and xform */
      alawbyte = (unsigned char)(sign | (exponent << 4) | mantissa);
      alawbyte ^= AST_ALAW_AMI_MASK;
   }
   else
   {
      /* half-cooked coding -- mantissa+exponent only (for lookup tab) */
      alawbyte = (exponent << 4) | mantissa;
   }
   return alawbyte;
}

short SPAW_Alaw2Linear(unsigned char alawbyte)
{
   return alaw2linear(alawbyte);
}

unsigned char SPAW_Linear2Alaw(short sample, int full_coding)
{
   return linear2alaw(sample, full_coding);
}
