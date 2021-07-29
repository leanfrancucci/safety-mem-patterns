/**
 *  \file       Crc32.h
 *  \brief      Specification of CRC-32 module.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __CRC32_H__
#define __CRC32_H__

/* ----------------------------- Include files ----------------------------- */
#include <stddef.h>
#include <stdint.h>

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
typedef uint32_t Crc32;

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void Crc32_init(void);
Crc32 Crc32_calc(const uint8_t *buf, size_t len, Crc32 init);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
