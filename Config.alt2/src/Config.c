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

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static ConfigErrorHandler errorHandler = (ConfigErrorHandler)0;
static Config config;
static const Config configDefault =
{
    {
        CONFIG_OPTA_DFT, 
        CONFIG_OPTB_DFT
    }, 0
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static bool
checkDataFromNVMem(Config *data)
{
    bool res = false;
    Config cfg;
    Crc32 crc;

    NVMem_readData(CONFIG_ADDR_BEGIN, sizeof(Config), (uint8_t *)&cfg);
    crc = Crc32_calc((const uint8_t *)&cfg, sizeof(Config), 0xffffffff);
    if (crc == cfg.crc)
    {
        if (data != (Config *)0)
        {
            *data = cfg;
        }
        res = true;
    }
    return res;
}

/* ---------------------------- Global functions --------------------------- */
ConfigErrorCode
Config_init(void)
{
    ConfigErrorCode res = NO_ERRORS;

    Crc32_init();
    if (checkDataFromNVMem(&config) == false)
    {
        res = INIT_DATA;
        if (errorHandler != (ConfigErrorHandler)0)
        {
            errorHandler(res);
        }
        config = configDefault;
        NVMem_storeData(CONFIG_ADDR_BEGIN, sizeof(Config), 
                        (const uint8_t *)&config);
    }
    return res;
}

void 
Config_setErrorHandler(ConfigErrorHandler errHandler)
{
    errorHandler = errHandler;
}

bool
Config_getOptionA(int *value)
{
    bool res = false;

    if (value != (int *)0)
    {
        *value = config.data.optionA;
        res = true;
    }
    return res;
}

bool
Config_setOptionA(int value)
{
    config.data.optionA = value;
    config.crc = Crc32_calc((const uint8_t *)&config, sizeof(Config), 
                            0xffffffff);
    NVMem_storeData(CONFIG_ADDR_BEGIN, sizeof(Config), 
                    (const uint8_t *)&config);
    return true;
}

/* ------------------------------ End of file ------------------------------ */
