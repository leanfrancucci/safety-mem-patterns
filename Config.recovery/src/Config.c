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
#include "ConfigDft.h"
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

typedef struct Config Config;
struct Config
{
    ConfigData data;
    Crc32 crc;
};

typedef struct ConfigInitBlock ConfigInitBlock;
struct ConfigInitBlock
{
    Crc32 readCRC;
    int result;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static ConfigErrorHandler errorHandler = (ConfigErrorHandler)0;
static ConfigInitBlock main, backup;
static Config block, backupBlock;
static const Config configDefault =
{
    {
        CONFIG_OPTA_DFT, 
        CONFIG_OPTB_DFT
    }, 0
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
    block = configDefault;
    block.crc = Crc32_calc((const uint8_t *)&block.data, 
                           sizeof(ConfigData), 0xffffffff);
    NVMem_storeData(CONFIG_MAIN_ADDR, sizeof(Config), 
                    (const uint8_t *)&block);
    NVMem_storeData(CONFIG_BACKUP_ADDR, sizeof(Config), 
                    (const uint8_t *)&block);
    return CORRUPT_DATA;
}

static ConfigErrorCode
proc_recovery(void)
{
    block = backupBlock;
    NVMem_storeData(CONFIG_MAIN_ADDR, sizeof(Config), 
                    (const uint8_t *)&block);
    return RECOVER_DATA;
}

static ConfigErrorCode
proc_backup(void)
{
    NVMem_storeData(CONFIG_BACKUP_ADDR, sizeof(Config), 
                    (const uint8_t *)&block);
    return BACKUP_DATA;
}

static ConfigErrorCode
proc_cmp(void)
{
    ConfigErrorCode res = NO_ERRORS;

    if (main.readCRC != backup.readCRC)
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
    NVMem_readData(CONFIG_MAIN_ADDR, sizeof(Config), 
                   (uint8_t *)&block);
    main.readCRC = Crc32_calc((const uint8_t *)&block.data, 
                              sizeof(ConfigData), 0xffffffff);
    main.result = (main.readCRC == block.crc) ? 1 : 0;
    NVMem_readData(CONFIG_BACKUP_ADDR, sizeof(Config), 
                   (uint8_t *)&backupBlock);
    backup.readCRC = Crc32_calc((const uint8_t *)&backupBlock.data, 
                                sizeof(ConfigData), 0xffffffff);
    backup.result = (backup.readCRC == backupBlock.crc) ? 1 : 0;
    status = 0;
    status = (main.result << 1) | backup.result;
    return (*recovery[status])();
}

bool
Config_getOptionA(int *value)
{
    bool res = false;

    if (value != (int *)0)
    {
        *value = block.data.optionA;
        res = true;
    }
    return res;
}

bool
Config_setOptionA(int value)
{
    block.data.optionA = value;
    block.crc = Crc32_calc((const uint8_t *)&block.data, 
                           sizeof(ConfigData), 0xffffffff);
    NVMem_storeData(CONFIG_MAIN_ADDR, sizeof(Config), 
                    (const uint8_t *)&block.data);
    NVMem_storeData(CONFIG_BACKUP_ADDR, sizeof(Config), 
                    (const uint8_t *)&block.data);
    return true;
}

/* ------------------------------ End of file ------------------------------ */
