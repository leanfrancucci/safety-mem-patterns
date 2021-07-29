/*
 * --------------------------------------------------------------------------
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
#define CONFIG_MAIN_ADDR        0
#define CONFIG_BACKUP_ADDR      512

typedef enum ConfigErrorCode ConfigErrorCode;
enum ConfigErrorCode
{
    NO_ERRORS,
    INIT_DATA,
    CORRUPT_DATA,
    RECOVER_DATA,
    BACKUP_DATA
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
