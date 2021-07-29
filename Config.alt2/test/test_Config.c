/*
 * ---------------------------------------------------------------------------
 * ---------------------------------------------------------------------------
 */

/**
 *  \file   test_Config.c
 *  \brief  Unit test for this module.
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
#include "unity.h"
#include "Config.h"
#include "Mock_NVMem.h"
#include "Mock_Crc32.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* 
 * Even though both types ConfigData and Config have already defined by 
 * Config.c file, they are redefined here to test this module in a simple way.
 */
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
static Config cfgRead;
static ConfigErrorCode errCodeCb;
static const Config configDefault =
{
    {64, 1024}, 0
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
cbNVMem_readData(uint32_t from, uint32_t nBytes, uint8_t *to, 
                 int cmock_num_calls)
{
    *((Config *)to) = cfgRead;
}

/* ---------------------------- Global functions --------------------------- */
void 
setUp(void)
{
}

void 
tearDown(void)
{
}

void
test_GetOptA(void)
{
    bool getRes;
    int value;

    cfgRead = configDefault;
    errCodeCb = INIT_DATA;
    Crc32_init_Expect();
    NVMem_readData_Expect(CONFIG_ADDR_BEGIN, sizeof(Config), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();

    Config_init();

    getRes = Config_getOptionA(&value);

    TEST_ASSERT_TRUE(getRes);
    TEST_ASSERT_EQUAL(64, value);
}

/* ------------------------------ End of file ------------------------------ */
