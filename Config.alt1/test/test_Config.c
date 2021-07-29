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
static Config cfgRead, cfgStore;
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

static void
cbNVMem_storeData(uint32_t to, uint32_t nBytes, const uint8_t *from, 
                  int cmock_num_calls)
{
    if (((Config *)from)->data.optionA != cfgStore.data.optionA)
    {
        TEST_FAIL();
    }
}

static void 
errorHandler(ConfigErrorCode errCode)
{
    TEST_ASSERT_EQUAL(errCodeCb, errCode);
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
test_InitWithInvalidData(void)
{
    ConfigErrorCode res;

    cfgRead.crc = 0xffffffff;
    cfgStore = configDefault;
    Crc32_init_Expect();
    NVMem_readData_Expect(CONFIG_ADDR_BEGIN, sizeof(Config), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, ~cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();
    NVMem_storeData_Expect(CONFIG_ADDR_BEGIN, sizeof(Config), 0);
    NVMem_storeData_IgnoreArg_from();
    NVMem_storeData_StubWithCallback(cbNVMem_storeData);

    res = Config_init();
    TEST_ASSERT_EQUAL(INIT_DATA, res);
}

void
test_InitWithValidData(void)
{
    ConfigErrorCode res;

    cfgRead.crc = 0xdeadbeef;
    Crc32_init_Expect();
    NVMem_readData_Expect(CONFIG_ADDR_BEGIN, sizeof(Config), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();

    res = Config_init();
    TEST_ASSERT_EQUAL(NO_ERRORS, res);
}

void
test_InitAndCallErrorHandler(void)
{
    ConfigErrorCode res;

    cfgRead.crc = 0xffffffff;
    cfgStore = configDefault;
    errCodeCb = INIT_DATA;
    Crc32_init_Expect();
    NVMem_readData_Expect(CONFIG_ADDR_BEGIN, sizeof(Config), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, ~cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();
    NVMem_storeData_Expect(CONFIG_ADDR_BEGIN, sizeof(Config), 0);
    NVMem_storeData_IgnoreArg_from();
    NVMem_storeData_StubWithCallback(cbNVMem_storeData);

    Config_setErrorHandler(errorHandler);
    res = Config_init();
    TEST_ASSERT_EQUAL(INIT_DATA, res);
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

    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();

    getRes = Config_getOptionA(&value);

    TEST_ASSERT_TRUE(getRes);
    TEST_ASSERT_EQUAL(64, value);
}

void
test_TryToGetOptAWithCorruptedData(void)
{
    bool getRes;
    int value;

    value = 1024;
    cfgRead = configDefault;
    errCodeCb = INIT_DATA;
    Crc32_init_Expect();
    NVMem_readData_Expect(CONFIG_ADDR_BEGIN, sizeof(Config), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();

    Config_init();

    errCodeCb = CORRUPT_DATA;
    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, ~cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();

    getRes = Config_getOptionA(&value);

    TEST_ASSERT_FALSE(getRes);
    TEST_ASSERT_EQUAL(1024, value);
}

void
test_TryToGetOptAWithAnInvalidArg(void)
{
    bool res;

    cfgRead = configDefault;
    errCodeCb = INIT_DATA;
    Crc32_init_Expect();
    NVMem_readData_Expect(CONFIG_ADDR_BEGIN, sizeof(Config), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();

    res = Config_init();

    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();

    res = Config_getOptionA(0);

    TEST_ASSERT_FALSE(res);
}

void
test_SetOptA(void)
{
    bool setRes;
    int value;

    cfgRead = configDefault;
    cfgStore.data.optionA = value = 2048;
    errCodeCb = INIT_DATA;
    Crc32_init_Expect();
    NVMem_readData_Expect(CONFIG_ADDR_BEGIN, sizeof(Config), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();

    Config_init();

    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();
    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();
    NVMem_storeData_Expect(CONFIG_ADDR_BEGIN, sizeof(Config), 0);
    NVMem_storeData_IgnoreArg_from();
    NVMem_storeData_StubWithCallback(cbNVMem_storeData);

    setRes = Config_setOptionA(value);

    TEST_ASSERT_TRUE(setRes);
}

void
test_TryToSetOptAWithCorruptedData(void)
{
    bool setRes;

    cfgRead = configDefault;
    errCodeCb = INIT_DATA;
    Crc32_init_Expect();
    NVMem_readData_Expect(CONFIG_ADDR_BEGIN, sizeof(Config), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();

    Config_init();

    errCodeCb = CORRUPT_DATA;
    Crc32_calc_ExpectAndReturn(0, sizeof(Config), 0xffffffff, ~cfgRead.crc);
    Crc32_calc_IgnoreArg_buf();

    setRes = Config_setOptionA(2048);

    TEST_ASSERT_FALSE(setRes);
}

/* ------------------------------ End of file ------------------------------ */
