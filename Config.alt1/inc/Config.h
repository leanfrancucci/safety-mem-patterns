/*
 * --------------------------------------------------------------------------
 * MIT License
 *
 * Copyright (c) 2021 Leandro Francucci
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * --------------------------------------------------------------------------
 */

/**
 *  \file   Config.h
 *  \brief  Specifies this module.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci     lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* --------------------------------- Module -------------------------------- */
#ifndef __CONFIG_H__
#define __CONFIG_H__

/* ----------------------------- Include files ----------------------------- */
#include <stdint.h>
#include <stdbool.h>

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#define CONFIG_ADDR_BEGIN       0

typedef enum ConfigErrorCode ConfigErrorCode;
enum ConfigErrorCode
{
    NO_ERRORS,
    INIT_DATA,
    CORRUPT_DATA
};

/* ------------------------------- Data types ------------------------------ */
typedef void (*ConfigErrorHandler)(ConfigErrorCode errCode);

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
ConfigErrorCode Config_init(void);
void Config_setErrorHandler(ConfigErrorHandler errHandler);
bool Config_getOptionA(int *value);
bool Config_getOptionB(long *value);
bool Config_setOptionA(int value);
bool Config_setOptionB(long value);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ End of file ------------------------------ */
