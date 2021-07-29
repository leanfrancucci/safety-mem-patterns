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
enum
{
    MAIN_BLOCK_IX, BACKUP_BLOCK_IX
};

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

typedef struct TestConfig TestConfig;
struct TestConfig
{
    Config data;
    Crc32 readCRC;
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static TestConfig cfgRead[2];
static TestConfig cfgStore[2];
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
    TEST_ASSERT_FALSE(cmock_num_calls > 1);
    *((Config *)to) = cfgRead[cmock_num_calls].data;
}

static void
cbNVMem_storeData(uint32_t to, uint32_t nBytes, const uint8_t *from, 
                  int cmock_num_calls)
{
    TEST_ASSERT_FALSE(cmock_num_calls > 1);
    if (((Config *)from)->data.optionA != 
        cfgStore[cmock_num_calls].data.data.optionA)
    {
        TEST_FAIL();
    }
}

static void
init(TestConfig *mRead, TestConfig *bRead)
{
    Crc32_init_Expect();
    NVMem_readData_Expect(CONFIG_MAIN_ADDR, sizeof(Config), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(ConfigData), 0xffffffff, 
                               mRead->readCRC);
    Crc32_calc_IgnoreArg_buf();
    NVMem_readData_Expect(CONFIG_BACKUP_ADDR, sizeof(Config), 0);
    NVMem_readData_IgnoreArg_to();
    NVMem_readData_StubWithCallback(cbNVMem_readData);
    Crc32_calc_ExpectAndReturn(0, sizeof(ConfigData), 0xffffffff, 
                               bRead->readCRC);
    Crc32_calc_IgnoreArg_buf();
}

/* ---------------------------- Global functions --------------------------- */
void 
setUp(void)
{
    Mock_NVMem_Init();
}

void 
tearDown(void)
{
    Mock_NVMem_Verify();
    Mock_NVMem_Destroy();
}

void
test_InitBothBlocksAreCorrupted(void)
{
    ConfigErrorCode res;

    cfgRead[MAIN_BLOCK_IX].data.crc = 0xdeadbeef;
    cfgRead[MAIN_BLOCK_IX].readCRC = ~cfgRead[MAIN_BLOCK_IX].data.crc;
    cfgRead[BACKUP_BLOCK_IX].data.crc = 0xdeadbeef;
    cfgRead[BACKUP_BLOCK_IX].readCRC = ~cfgRead[BACKUP_BLOCK_IX].data.crc;
    init(&cfgRead[MAIN_BLOCK_IX], &cfgRead[BACKUP_BLOCK_IX]);

    cfgStore[MAIN_BLOCK_IX].data = configDefault;
    cfgStore[BACKUP_BLOCK_IX].data = configDefault;
    Crc32_calc_ExpectAndReturn(0, sizeof(ConfigData), 0xffffffff, 
                               cfgRead[MAIN_BLOCK_IX].data.crc);
    Crc32_calc_IgnoreArg_buf();
    NVMem_storeData_Expect(CONFIG_MAIN_ADDR, sizeof(Config), 0);
    NVMem_storeData_IgnoreArg_from();
    NVMem_storeData_StubWithCallback(cbNVMem_storeData);
    NVMem_storeData_Expect(CONFIG_BACKUP_ADDR, sizeof(Config), 0);
    NVMem_storeData_IgnoreArg_from();
    NVMem_storeData_StubWithCallback(cbNVMem_storeData);

    res = Config_init();
    TEST_ASSERT_EQUAL(CORRUPT_DATA, res);
}

void
test_InitRecoveryData(void)
{
    ConfigErrorCode res;

    cfgRead[MAIN_BLOCK_IX].data = configDefault;
    cfgRead[MAIN_BLOCK_IX].data.crc = 0xffffffff;
    cfgRead[MAIN_BLOCK_IX].readCRC = ~cfgRead[MAIN_BLOCK_IX].data.crc;
    cfgRead[BACKUP_BLOCK_IX].data = configDefault;
    cfgRead[BACKUP_BLOCK_IX].data.crc = 0xdeadbeef;
    cfgRead[BACKUP_BLOCK_IX].readCRC = cfgRead[BACKUP_BLOCK_IX].data.crc;
    init(&cfgRead[MAIN_BLOCK_IX], &cfgRead[BACKUP_BLOCK_IX]);

    cfgStore[MAIN_BLOCK_IX].data = cfgRead[BACKUP_BLOCK_IX].data;
    cfgStore[MAIN_BLOCK_IX].data.crc = cfgRead[BACKUP_BLOCK_IX].data.crc;
    NVMem_storeData_Expect(CONFIG_MAIN_ADDR, sizeof(Config), 0);
    NVMem_storeData_IgnoreArg_from();
    NVMem_storeData_StubWithCallback(cbNVMem_storeData);

    res = Config_init();
    TEST_ASSERT_EQUAL(RECOVER_DATA, res);
}

void
test_InitBackupData(void)
{
    ConfigErrorCode res;

    cfgRead[MAIN_BLOCK_IX].data = configDefault;
    cfgRead[MAIN_BLOCK_IX].data.crc = 0xdeadbeef;
    cfgRead[MAIN_BLOCK_IX].readCRC = cfgRead[MAIN_BLOCK_IX].data.crc;
    cfgRead[BACKUP_BLOCK_IX].data = configDefault;
    cfgRead[BACKUP_BLOCK_IX].data.crc = 0xdeadbeef;
    cfgRead[BACKUP_BLOCK_IX].readCRC = ~cfgRead[BACKUP_BLOCK_IX].data.crc;
    init(&cfgRead[MAIN_BLOCK_IX], &cfgRead[BACKUP_BLOCK_IX]);

    cfgStore[MAIN_BLOCK_IX].data = cfgRead[MAIN_BLOCK_IX].data;
    cfgStore[MAIN_BLOCK_IX].data.crc = cfgRead[MAIN_BLOCK_IX].data.crc;
    NVMem_storeData_Expect(CONFIG_BACKUP_ADDR, sizeof(Config), 0);
    NVMem_storeData_IgnoreArg_from();
    NVMem_storeData_StubWithCallback(cbNVMem_storeData);

    res = Config_init();
    TEST_ASSERT_EQUAL(BACKUP_DATA, res);
}

void
test_InitBothBlocksAreHealthy(void)
{
    ConfigErrorCode res;

    cfgRead[MAIN_BLOCK_IX].data = configDefault;
    cfgRead[MAIN_BLOCK_IX].data.crc = 0xdeadbeef;
    cfgRead[MAIN_BLOCK_IX].readCRC = cfgRead[MAIN_BLOCK_IX].data.crc;
    cfgRead[BACKUP_BLOCK_IX].data = configDefault;
    cfgRead[BACKUP_BLOCK_IX].data.crc = 0xdeadbeef;
    cfgRead[BACKUP_BLOCK_IX].readCRC = cfgRead[BACKUP_BLOCK_IX].data.crc;
    init(&cfgRead[MAIN_BLOCK_IX], &cfgRead[BACKUP_BLOCK_IX]);

    res = Config_init();
    TEST_ASSERT_EQUAL(NO_ERRORS, res);
}

void
test_InitBothBlocksAreHealthyButTheyAreDifferent(void)
{
    ConfigErrorCode res;

    cfgRead[MAIN_BLOCK_IX].data = configDefault;
    cfgRead[MAIN_BLOCK_IX].data.crc = 0xdeadbeef;
    cfgRead[MAIN_BLOCK_IX].readCRC = cfgRead[MAIN_BLOCK_IX].data.crc;
    cfgRead[BACKUP_BLOCK_IX].data = configDefault;
    cfgRead[BACKUP_BLOCK_IX].data.crc = 0xdeaddead;
    cfgRead[BACKUP_BLOCK_IX].readCRC = cfgRead[BACKUP_BLOCK_IX].data.crc;
    init(&cfgRead[MAIN_BLOCK_IX], &cfgRead[BACKUP_BLOCK_IX]);

    cfgStore[MAIN_BLOCK_IX].data = cfgRead[MAIN_BLOCK_IX].data;
    cfgStore[MAIN_BLOCK_IX].data.crc = cfgRead[MAIN_BLOCK_IX].data.crc;
    NVMem_storeData_Expect(CONFIG_BACKUP_ADDR, sizeof(Config), 0);
    NVMem_storeData_IgnoreArg_from();
    NVMem_storeData_StubWithCallback(cbNVMem_storeData);

    res = Config_init();
    TEST_ASSERT_EQUAL(BACKUP_DATA, res);
}

/* ------------------------------ End of file ------------------------------ */
