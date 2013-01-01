/** @file wlan_join.h
 *
 *  @brief Interface for the wlan infrastructure and adhoc join routines
 *
 *  Driver interface functions and type declarations for the join module
 *    implemented in wlan_join.c.  Process all start/join requests for
 *    both adhoc and infrastructure networks
 *
 *  Copyright © Marvell International Ltd. and/or its affiliates, 2003-2006
 *
 *  @sa wlan_join.c
 */
/*************************************************************
Change Log:
    01/11/06: Initial revision. Match new scan code, relocate related functions

************************************************************/

#ifndef _WLAN_JOIN_H
#define _WLAN_JOIN_H

//! Size of buffer allocated to store the association response from firmware
#define MRVDRV_ASSOC_RSP_BUF_SIZE  500

//! Size of buffer allocated to store IEs passed to firmware in the assoc req
#define MRVDRV_GENIE_BUF_SIZE      256

//! Size of buffer allocated to store TLVs passed to firmware in the assoc req
#define MRVDRV_ASSOC_TLV_BUF_SIZE  256
#endif
