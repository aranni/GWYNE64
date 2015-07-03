/* Cada banco se direccionara en Modbus de la siguiente manera:
Banco 0: 40000
Banco 1: 40500
Banco 2: 41000
 .....
Banco Diagnostico:49500

Los datos en memoria seran los del array "bac_data"
Puede ser un  ACK (secuencia 0x10 - 0x06) o un mensaje completo crc incluido

Para los datos fuera de rango el conversor devolvera el valor 0xFF
*/

#ifndef __BAC_STUFF_INCLUDED__
#define __BAC_STUFF_INCLUDED__

// Definiciones bytes de protocolo
#define DLE_CHAR		0x10
#define STX_CHAR		0x02
#define CTRLL_CHAR	0x00
#define CTRLN_CHAR	0x00
#define PRIOR_CHAR  0xC8
#define BC_CHAR			0xC0
#define ETX_CHAR    0x03
#define ACK_CHAR		0x06

#define MAX_BAC_BUF   256
#define MAX_BAC_BANKS 8
#define MAX_BAC_LEN   128     // Maxima longitud de un banco de datos
#define DIAG_BAC_LEN  4 * 4   // double words conteniendo estadisticas y estado del equipo, comunic, etc.

//char bac_buff[MAX_BAC_BUF];
char bac_len;
char bac_data[MAX_BAC_BANKS][MAX_BAC_LEN];
char bac_diag[DIAG_BAC_LEN];  // Los dos primeros double words los escribe la capa inferior
				     // con datos enciados y recibidos, el resto lo vemos

/* Esta funcion se llama desde la capa inferior cada vez que llega una trama
   y procesara los bac_len datos almacenados en bac_buff
*/
//void ProcessBacRead(void);
/* Pedido de encuesta */
//void SendBacMsg(void);
/* Timeout handler. Se llama desde la capa inferior cuando se detecta un timeout */
//void BacTimeout(void);

#endif
