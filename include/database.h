//various functions for sqlite

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sqlite/sqlite3.h>
#include <iostream>

static int callback_select(void *pointer, int argc, char **argv, char **azColName) {
    int i;
    std::string *s = (std::string *)pointer;
    *s = argv[0];
    return 0;
}

void db_create()
{
    sqlite3 *db;
    int rc;
    char *zErrMsg = 0;
    const char *sql;

    rc = sqlite3_open("auth.db", &db);

    sql = "CREATE TABLE AUTH("  \
    "USER TEXT NOT NULL," \
    "PASS TEXT NOT NULL,"
    "PRIMARY KEY (USER));";

    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL msg: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } 
    sqlite3_close(db);
}

void db_insert(std::string user, std::string pass)
{
	sqlite3 *db;
    int rc;
    char *zErrMsg = 0;
    std::string sql;

    rc = sqlite3_open("auth.db", &db);

    sql = "INSERT INTO AUTH(user,pass) "  \
    "VALUES ('"+ user + "', '"+ pass +"');";   

    rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL msg: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    } 

    sqlite3_close(db);
}

std::string db_select_pass(std::string user)
{
    sqlite3 *db;
    int rc;
    char *zErrMsg = 0;
    std::string sql;
    std::string data;

    rc = sqlite3_open("auth.db", &db);

    sql = "SELECT pass from AUTH WHERE user='"+ user +"'";

    rc = sqlite3_exec(db, sql.c_str(), callback_select, &data, &zErrMsg);
    
    if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    
    sqlite3_close(db);

    return data;
}
