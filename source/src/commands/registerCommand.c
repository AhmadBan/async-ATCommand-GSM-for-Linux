/*
 * registerCommand.c
 *
 *  Created on: Dec 18, 2020
 *      Author: Ahmad
 */
#include <commands/baseCommand.h>

extern void smsSend_ctor(void);
const pCtorFunc ctorsArr[] =
{
		smsSend_ctor,
};
const u_int8_t ctorCnt = sizeof( ctorsArr ) / sizeof( ctorsArr[0] );
