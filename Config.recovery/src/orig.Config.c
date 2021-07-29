/*
 * --------------------------------------------------------------------------
 * --------------------------------------------------------------------------
 */

/**
 *  \file   Config.c
 *  \brief  Implements the specifications.
 */

/* -------------------------- Development history -------------------------- */
/*
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci     lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "Config.h"
#include "NVMem.h"
#include "Crc32.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
typedef ConfigErrorCode (*RecProc)(void);

typedef struct ConfigData ConfigData;
struct ConfigData
{
    int optionA;
    long optionB;
};

typedef struct SafeConfig SafeConfig;
struct SafeConfig
{
    ConfigData data;
    Crc32 crc;
};

typedef struct Config Config;
struct Config
{
    SafeConfig safeData;
    Crc32 readCRC;
    int initResult;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static ConfigErrorHandler errorHandler = (ConfigErrorHandler)0;
static Config main, backup;
static const ConfigData configDefault =
{
    64, 1024
};

/*
 *  Recovery true table:
 *
 *  rmain: '1' if the stored CRC in the main data block matches with the 
 *         recalculated CRC, otherwise '0'
 *  rback: '1' if the stored CRC in the backup data block matches with the 
 *         recalculated CRC, otherwise '0'
 *
 *  rmain |	rback |	Process       | Output
 *  ---------------------------------------------------
 *  0     |	0     |	proc_in_error | CORRUPT_DATA
 *  0     | 1     |	proc_recovery | RECOVER_DATA
 *  1     | 0     |	proc_backup   | BACKUP_DATA
 *  1     | 1     |	proc_cmp      | -> next true table
 *
 *  CRC compare true table:
 *  ---------------------------------------------------
 *  The main's CRC matches with the backup's CRC, so it returns BACKUP_DATA, 
 *  otherwise it returns NO_ERRORS
 */
static ConfigErrorCode proc_in_error(void);
static ConfigErrorCode proc_recovery(void);
static ConfigErrorCode proc_backup(void);
static ConfigErrorCode proc_cmp(void);

static const RecProc recovery[] =
{
    proc_in_error, proc_recovery, proc_backup, proc_cmp
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static ConfigErrorCode
proc_in_error(void)
{
    SafeConfig *safeData;

    safeData = &main.safeData;
    safeData->data = configDefault;
    safeData->crc = Crc32_calc((const uint8_t *)&safeData->data, 
                               sizeof(ConfigData), 0xffffffff);
    NVMem_storeData(CONFIG_MAIN_ADDR, sizeof(SafeConfig), 
                    (const uint8_t *)safeData);
    NVMem_storeData(CONFIG_BACKUP_ADDR, sizeof(SafeConfig), 
                    (const uint8_t *)safeData);
    return CORRUPT_DATA;
}

static ConfigErrorCode
proc_recovery(void)
{
    main = backup;
    NVMem_storeData(CONFIG_MAIN_ADDR, sizeof(SafeConfig), 
                    (const uint8_t *)&main.safeData);
    return RECOVER_DATA;
}

static ConfigErrorCode
proc_backup(void)
{
    backup = main;
    NVMem_storeData(CONFIG_BACKUP_ADDR, sizeof(SafeConfig), 
                    (const uint8_t *)&backup.safeData);
    return BACKUP_DATA;
}

static ConfigErrorCode
proc_cmp(void)
{
    ConfigErrorCode res = NO_ERRORS;

    if (main.readCRC == backup.readCRC)
    {
        res = proc_backup();
    }
    return res;
}

/* ---------------------------- Global functions --------------------------- */
ConfigErrorCode
Config_init(void)
{
    int status;

    Crc32_init();
    NVMem_readData(CONFIG_MAIN_ADDR, sizeof(SafeConfig), 
                   (uint8_t *)&main.safeData);
    main.readCRC = Crc32_calc((const uint8_t *)&main.safeData.data, 
                              sizeof(ConfigData), 0xffffffff);
    main.initResult = (main.readCRC == main.safeData.crc) ? 1 : 0;
    NVMem_readData(CONFIG_BACKUP_ADDR, sizeof(SafeConfig), 
                   (uint8_t *)&backup.safeData);
    backup.readCRC = Crc32_calc((const uint8_t *)&backup.safeData.data, 
                                sizeof(ConfigData), 0xffffffff);
    backup.initResult = (backup.readCRC == backup.safeData.crc) ? 1 : 0;
    status = 0;
    status = (main.initResult << 1) | backup.initResult;
    return (*recovery[status])();
}

bool
Config_getOptionA(int *value)
{
    bool res = false;

    if (value != (int *)0)
    {
        *value = main.safeData.data.optionA;
        res = true;
    }
    return res;
}

bool
Config_setOptionA(int value)
{
    SafeConfig *safeData;

    safeData = &main.safeData;
    safeData->data.optionA = value;
    main.safeData.crc = Crc32_calc((const uint8_t *)&safeData->data, 
                                   sizeof(ConfigData), 0xffffffff);
    NVMem_storeData(CONFIG_MAIN_ADDR, sizeof(SafeConfig), 
                    (const uint8_t *)safeData);
    NVMem_storeData(CONFIG_BACKUP_ADDR, sizeof(SafeConfig), 
                    (const uint8_t *)safeData);
    return true;
}

/* ------------------------------ End of file ------------------------------ */
