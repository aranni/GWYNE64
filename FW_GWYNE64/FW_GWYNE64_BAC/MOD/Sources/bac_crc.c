//#include <stdio.h>
//#include <stdlib.h>
#include "crcmodel.h"
#include "bac_crc.h"

unsigned short PermuteBytes(unsigned short data)
{
	unsigned short ret;
	*(char *) &ret= *((char *) &data+1);
	*((char *)&ret+1) = *(char *)&data;
	return ret;
}

#define BITMASK(X) (1L << (X))
#define MASK32 0xFFFFFFFFL

/******************************************************************************/

ulong reflect (ulong v, int b)
{
 int   i;
 ulong t = v;
 for (i=0; i<b; i++)
   {
    if (t & 1L)
       v|=  BITMASK((b-1)-i);
    else
       v&= ~BITMASK((b-1)-i);
    t>>=1;
   }
 return v;
}

/******************************************************************************/

ulong widmask P_((p_cm_t p_cm))
{
 return (((1L<<(p_cm->cm_width-1))-1L)<<1)|1L;
}

/******************************************************************************/

void cm_ini (p_cm_t p_cm)
//p_cm_t p_cm;
{
 p_cm->cm_reg = p_cm->cm_init;
}

/******************************************************************************/

void cm_nxt (p_cm_t p_cm, int ch)
//p_cm_t p_cm;
//int    ch;
{
 int   i;
 ulong uch  = (ulong) ch;
 ulong topbit = BITMASK(p_cm->cm_width-1);

 if (p_cm->cm_refin) uch = reflect(uch,8);
 p_cm->cm_reg ^= (uch << (p_cm->cm_width-8));
 for (i=0; i<8; i++)
   {
    if (p_cm->cm_reg & topbit)
       p_cm->cm_reg = (p_cm->cm_reg << 1) ^ p_cm->cm_poly;
    else
       p_cm->cm_reg <<= 1;
    p_cm->cm_reg &= widmask(p_cm);
   }
}

/******************************************************************************/

void cm_blk (p_cm_t p_cm, p_ubyte_ blk_adr, ulong blk_len)
//p_cm_t   p_cm;
//p_ubyte_ blk_adr;
//ulong    blk_len;
{
 while (blk_len--) cm_nxt(p_cm,*blk_adr++);
}

/******************************************************************************/

ulong cm_crc (p_cm_t p_cm)
//p_cm_t p_cm;
{
 if (p_cm->cm_refot)
    return p_cm->cm_xorot ^ reflect(p_cm->cm_reg,p_cm->cm_width);
 else
    return p_cm->cm_xorot ^ p_cm->cm_reg;
}

/******************************************************************************/

ulong cm_tab (p_cm_t p_cm, int index)
//p_cm_t p_cm;
//int    index;
{
 int   i;
 ulong r;
 ulong topbit = BITMASK(p_cm->cm_width-1);
 ulong inbyte = (ulong) index;

 if (p_cm->cm_refin) inbyte = reflect(inbyte,8);
 r = inbyte << (p_cm->cm_width-8);
 for (i=0; i<8; i++)
    if (r & topbit)
       r = (r << 1) ^ p_cm->cm_poly;
    else
       r<<=1;
 if (p_cm->cm_refin) r = reflect(r,p_cm->cm_width);
 return r & widmask(p_cm);
}


unsigned short CalcCrc(unsigned char * pData, unsigned short length)
{

	cm_t	cm;
	p_cm_t	p_cm=&cm;
	unsigned short retval;

	p_cm->cm_width=16;
	p_cm->cm_poly=0x1021L;        
	p_cm->cm_init=0L;
	p_cm->cm_refin=FALSE;
	p_cm->cm_refot=FALSE;
	p_cm->cm_xorot=0L;

	cm_ini(p_cm);

	while(length--)
		cm_nxt(p_cm,*pData++);
	retval=(unsigned short) cm_crc(p_cm);
	return retval;
}




