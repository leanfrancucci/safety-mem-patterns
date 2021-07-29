/**
 *  \file       Crc32.c
 *  \brief      Implementation of CRC-32 using STM32.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 *  DaBa  Darío Baliña db@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include <string.h>
#include "Crc32.h"
#include "bsp.h"
#include "crc.h"
#include "GStatus.h"
#include "rfile.h"
#include "Config.h"
#include "rkhassert.h"

RKH_MODULE_NAME(Crc32)

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
typedef union SecBlock SecBlock;
union SecBlock
{
    FFILE_T ffileData;
    Config configData;
    GStatusType gStatusTypeData;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static uint32_t buffer[sizeof(SecBlock)];

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void
Crc32_init(void)
{
}

Crc32
Crc32_calc(const uint8_t *message, size_t nBytes, Crc32 init)
{
	size_t i;

    RKH_REQUIRE(nBytes <= sizeof(SecBlock));
	for (i = 0; i < nBytes; ++i, ++message)
    {
		buffer[i] = *message;
    }

	return HAL_CRC_Calculate(&hcrc, buffer, nBytes);
}

/* ------------------------------ End of file ------------------------------ */
