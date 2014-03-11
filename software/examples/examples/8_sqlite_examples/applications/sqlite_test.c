#include <stdio.h>
#include <rtthread.h>
#include "finsh.h"
#include "sqlite3.h"

#define DB_FILE "test.db"

#define JUDGE_ERR(pMsgErr)                          \
    do{                                             \
        if (pErrMsg != SQLITE_OK)                   \
        {                                           \
            rt_kprintf("%s:%d\tSQL error: %s\n",    \
                __FUNCTION__, __LINE__, pErrMsg);   \
            sqlite3_free(pErrMsg);                  \
        }                                           \
    } while (0)

static int _callback_exec(void* notused, int argc, char** argv, char** aszColName)
{
    int i;

    for (i=0; i<argc; i++)
    {
        rt_kprintf("callback: %s = %s/n", aszColName[i], argv[i] == 0 ? "NUL" : argv[i]);
    }

    return 0;
}

static void _select_show(int row, int column, char** azResult)
{
    int i, j;

    rt_kprintf("row:%d column=%d\n", row, column);
    rt_kprintf("\nThe result of querying is : \n");

    for (i=0; i<row+1; i++)
    {
        for (j=0; j<column; j++)
        {
            rt_kprintf("%s\t", azResult[(i*column) + j]);
        }

        rt_kprintf("\n");
    }
}

static int testdb1(void)
{
    sqlite3* db = NULL;
    char* sql;
    char* pErrMsg = 0;
    char** azResult;
    int rc;
    int nrow = 0;
    int ncolumn = 0;

    rt_kprintf("\n=====case 1:\n");

    rc = sqlite3_open(DB_FILE, &db);
    if(rc)
    {
        rt_kprintf("Can’t open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    rt_kprintf("opened a sqlite3 database named %s successfully!\n", DB_FILE);

    sql = "CREATE TABLE test(ID INTEGER PRIMARY KEY,AGE INTEGER,LEVEL INTEGER,"
            "NAME VARCHAR(12),SALARY REAL);";
    sqlite3_exec(db, sql, 0, 0, &pErrMsg);
    JUDGE_ERR(pErrMsg);
    
    sql = "INSERT INTO test VALUES(NULL, 1, 1, '201312', 120.9);";
    sqlite3_exec(db, sql, 0, 0, &pErrMsg);
    JUDGE_ERR(pErrMsg);
    
    sql = "SELECT * FROM test";
    sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &pErrMsg);
    JUDGE_ERR(pErrMsg);
    _select_show(nrow, ncolumn, azResult);

    sql = "DELETE FROM test WHERE AGE = 1;";
    sqlite3_exec(db, sql, 0, 0, &pErrMsg);
    JUDGE_ERR(pErrMsg);

    sqlite3_free_table(azResult);
    sqlite3_close(db);

    return 0;
}

static int testdb2(void)
{
    const char* sSQL = "select * from t1;";
    char* pErrMsg = 0;
    int rc = 0;
    sqlite3* db = 0;

    rt_kprintf("\n=====case 2:\n");

    rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK)
    {
        rt_kprintf("Could not open database: %s", sqlite3_errmsg(db));
        return -1;
    }

    rt_kprintf("Successfully connected to database\n");

    sqlite3_exec(db, sSQL, _callback_exec, 0, &pErrMsg);
    JUDGE_ERR(pErrMsg);

    sqlite3_close(db);

    return 0;
}

static  int testdb3(void)
{
    sqlite3* db = 0;
    char* pErrMsg = 0;
    char* sql;
    char** azResult;
    int nrow = 0;
    int ncolumn = 0;
    int rc = 0;

    rt_kprintf("\n=====case 3:\n");

    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK)
    {
        rt_kprintf("Could not open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    rt_kprintf("Successfully connected to database\n");

    sql = "CREATE TABLE test(ID INTEGER PRIMARY KEY,AGE INTEGER,LEVEL INTEGER,"
            "NAME VARCHAR(12),SALARY REAL);";
    sqlite3_exec(db, sql, 0, 0, &pErrMsg);
    JUDGE_ERR(pErrMsg);
    
    sql = "INSERT INTO test VALUES(NULL, 1, 1, '201401', 200.9);";
    sqlite3_exec(db, sql, 0, 0, &pErrMsg);
    JUDGE_ERR(pErrMsg);

    sql = "SELECT * FROM test";
    sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &pErrMsg);
    JUDGE_ERR(pErrMsg);
    _select_show(nrow, ncolumn, azResult);

    sqlite3_free_table(azResult);
    sqlite3_close(db);

    return 0;
}

int sqlite_test(void)
{
    testdb1();
    testdb2();
    testdb3();
    
    return 0;
}
FINSH_FUNCTION_EXPORT(sqlite_test, SQLite Test Case)
