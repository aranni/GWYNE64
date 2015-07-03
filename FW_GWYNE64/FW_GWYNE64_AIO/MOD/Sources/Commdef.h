
#ifndef CommdefH
#define CommdefH

//----- pertenece a datatypes.h-----------------------------------------------------------
//
#ifndef INCLUDE_DATATYPES_H
#define INCLUDE_DATATYPES_H

#define LWORD	unsigned long		/**< 32 bit unsigned */

typedef unsigned long	uint32;
typedef signed long	    int32;

#define BYTE 	unsigned char				
#define WORD 	unsigned short		/**< 16 bit unsigned */

#ifndef UINT8
#define UINT8	unsigned char		/**< 8 bit unsigned */
#endif

#ifndef INT8
#define INT8	signed char			/**< 8 bit signed */
#endif

#ifndef UINT16
#define	UINT16	unsigned short		/**< 16 bit unsigned */
#endif

#ifndef INT16
#define INT16	short				/**< 16 bit signed */
#endif

typedef unsigned char	uint8;
typedef unsigned short uint16;
typedef signed char  int8;
typedef signed short int16;

#endif  // DATATYPESH


#ifndef RETCODE
#define RETCODE	unsigned char		
#endif

#include "hComSwitches.h"
#include "hComConstants.h"
#include "hComStructDat.h"

#endif      // CommdefH