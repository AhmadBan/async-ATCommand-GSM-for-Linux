/*
 * registerCommand.c
 *
 *  Created on: Dec 18, 2020
 *      Author: Ahmad
 */
#include <commands/baseCommand.h>

extern void SMSSend_ctor(void);
const pCtorFunc ctorsArr[] =
{
		SMSSend_ctor,


};
const u_int8_t ctorCnt = sizeof( ctorsArr ) / sizeof( ctorsArr[0] );
