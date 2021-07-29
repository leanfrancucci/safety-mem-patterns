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

typedef struct TestConfig TestConfig;
struct TestConfig
{
    Config data;
    int blockAddr;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static TestConfig cfgRead[2];
static SafeConfig cfgStore[2];
static const SafeConfig configDefault =
{
    {64, 1024}, 0
};

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void
cbNVMem_readData(uint32_t from, uint32_t nBytes, uint8_t *to, 
                 int cmock_num_calls)
{
    TEST_ASSERT_FALSE(cmock_num_calls > 1);
    if (cmock_num_calls == 0)
    {
        TEST_ASSERT_EQUAL(CONFIG_MAIN_ADDR, 
                          cfgRead[cmock_num_calls].blockAddr);
    }
    else
    {
        TEST_ASSERT_EQUAL(CONFIG_BACKUP_ADDR, 
                          cfgRead[cmock_num_calls].blockAddr);
    }
    *((SafeConfig *)to) = cfgRead[cmock_num_calls].data.safeData;
}

static void
cbNVMem_storeData(uint32_t to, uint32_t nBytes, const uint8_t *from, 
                  int cmock_num_calls)
{
    TEST_ASSERT_FALSE(cmock_num_calls > 1);
    if (((SafeConfig *)from)->data.optionA != 
        cfgStore[cmock_num_calls].data.optionA)
    {
        TEST_FAIL();
    }
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
test_InitWithBothBlocksCorrupted(void)
{
    ConfigErrorCode res;

    Crc32_init_Expect();
    cfgRead[0].data.safeData.crc = 0xffffffff;
    cfgRead[1].data.safeData.crc = 0xffffffff;
    NVMem_readData_Expect(CONFIG_MAIN_ADDR, sizeof(SafeConfig), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_IgnoreArg_from();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(ConfigData), 0xffffffff, 
                               ~cfgRead[0].data.safeData.crc);
    Crc32_calc_IgnoreArg_buf();
    NVMem_readData_Expect(CONFIG_BACKUP_ADDR, sizeof(SafeConfig), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_IgnoreArg_from();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(ConfigData), 0xffffffff, 
                               ~cfgRead[1].data.safeData.crc);
    Crc32_calc_IgnoreArg_buf();
    cfgStore[0] = configDefault;
    cfgStore[1] = configDefault;
    Crc32_calc_ExpectAndReturn(0, sizeof(ConfigData), 0xffffffff, 0xdeadbeaf);
    Crc32_calc_IgnoreArg_buf();
    NVMem_storeData_Expect(CONFIG_MAIN_ADDR, sizeof(SafeConfig), 0);
    NVMem_storeData_IgnoreArg_from();
    NVMem_storeData_StubWithCallback(cbNVMem_storeData);
    NVMem_storeData_Expect(CONFIG_BACKUP_ADDR, sizeof(SafeConfig), 0);
    NVMem_storeData_IgnoreArg_from();
    NVMem_storeData_StubWithCallback(cbNVMem_storeData);

    res = Config_init();
    TEST_ASSERT_EQUAL(CORRUPT_DATA, res);
}

/* ------------------------------ End of file ------------------------------ */
