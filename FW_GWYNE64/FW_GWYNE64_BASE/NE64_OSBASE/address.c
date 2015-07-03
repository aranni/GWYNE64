/******************************************************************************
 * 
 *                      (c) Freescale  Inc. 2004 All rights reserved          
 *
 * File Name     : address.c
 * 
 * Description    : This file contains definition of hardware and protocol (IP) 
 *                  addresses of the device
 *
 * Version : 2.1
 * Date    : 02/04/04
 *
 ******************************************************************************/

#include "MOTTYPES.h"
  
//>>>>> 
const tU08 hard_addr [6] = { 0x00, 0x12, 0x45, 0x56, 0x78, 0x00 };

const tU08 prot_addr [4] = { 192, 168, 0, 250 };
const tU08 netw_mask [4] = { 255, 255, 0,   0 };
const tU08 dfgw_addr [4] = { 192, 168, 0,   1 };
const tU08 brcs_addr [4] = { 192, 168, 0, 255 };

