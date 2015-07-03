#ifndef CM_DONE
#define CM_DONE

/******************************************************************************/

/* The following definitions are extracted from my style header file which    */
/* would be cumbersome to distribute with this package. The DONE_STYLE is the */
/* idempotence symbol used in my style header file.                           */

#ifndef DONE_STYLE

typedef unsigned long   ulong;
typedef unsigned        bool;
typedef unsigned char * p_ubyte_;

#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif

/* Change to the second definition if you don't have prototypes. */
#define P_(A) A
/* #define P_(A) () */

/* Uncomment this definition if you don't have void. */
/* typedef int void; */

#endif

/******************************************************************************/

/* CRC Model Abstract Type */
/* ----------------------- */
/* The following type stores the context of an executing instance of the  */
/* model algorithm. Most of the fields are model parameters which must be */
/* set before the first initializing call to cm_ini.                      */
typedef struct
  {
   int   cm_width;   /* Parameter: Width in bits [8,32].       */
   ulong cm_poly;    /* Parameter: The algorithm's polynomial. */
   ulong cm_init;    /* Parameter: Initial register value.     */
   bool  cm_refin;   /* Parameter: Reflect input bytes?        */
   bool  cm_refot;   /* Parameter: Reflect output CRC?         */
   ulong cm_xorot;   /* Parameter: XOR this to output CRC.     */

   ulong cm_reg;     /* Context: Context during execution.     */
  } cm_t;
typedef cm_t *p_cm_t;

/******************************************************************************/

/* Functions That Implement The Model */
/* ---------------------------------- */
/* The following functions animate the cm_t abstraction. */

void cm_ini P_((p_cm_t p_cm));
/* Initializes the argument CRC model instance.          */
/* All parameter fields must be set before calling this. */

void cm_nxt P_((p_cm_t p_cm,int ch));

void cm_blk P_((p_cm_t p_cm,p_ubyte_ blk_adr,ulong blk_len));

ulong cm_crc P_((p_cm_t p_cm));


ulong cm_tab P_((p_cm_t p_cm,int index));

/******************************************************************************/


unsigned short CalcCrc(unsigned char * pData, unsigned short length);


#endif


