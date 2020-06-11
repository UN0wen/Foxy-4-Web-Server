//various functions for sqlite

#include <stdio.h>
#include <stdlib.h>
#include <sqlite/sqlite3.h>
#include <iostream>
#include "database.h"
#include <boost/log/trivial.hpp>
#include "password.h"

namespace database
{
  static int callback_select(void *pointer, int argc, char **argv, char **azColName)
  {
    int i;
    std::string *s = (std::string *)pointer;
    *s = argv[0];
    return 0;
  }

  int db_create()
  {
    sqlite3 *db;
    int rc;
    char *zErrMsg = 0;
    const char *sql;

    rc = sqlite3_open("auth.db", &db);

    sql = "CREATE TABLE IF NOT EXISTS AUTH("
          "USER TEXT NOT NULL,"
          "PASS TEXT NOT NULL,"
          "PRIMARY KEY (USER));";

    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
      BOOST_LOG_TRIVIAL(error) << "[SQLite] Error: " << zErrMsg;
      sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);
    return rc;
  }

  int db_insert(std::string user, std::string pass)
  {
    sqlite3 *db;
    int rc;
    std::string sql;
    sqlite3_stmt *query;
    std::string hashed_password = Password::generate_hash(pass);

    sqlite3_open("auth.db", &db);
    
    sql = "INSERT INTO AUTH(user,pass) "
          "VALUES (?1, ?2);";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &query, NULL);
    sqlite3_bind_text(query, 1, user.c_str(), user.size(), NULL);
    sqlite3_bind_text(query, 2, hashed_password.c_str(), hashed_password.size(), NULL);

    if (rc != SQLITE_OK)
    {
      BOOST_LOG_TRIVIAL(error) << "[SQLite] Error: " << sqlite3_errmsg(db);
      return rc;
    }

    rc = sqlite3_step(query);

    if (rc != SQLITE_DONE)
    {
      BOOST_LOG_TRIVIAL(error) << "[SQLite] Error: " << sqlite3_errmsg(db);
    }

    BOOST_LOG_TRIVIAL(info) << "[SQLite] Insert: " << user << " , " << hashed_password;
    sqlite3_finalize(query);
    sqlite3_close(db);
    return rc;
  }

  int db_select_pass(std::string user, std::string& pass)
  {
    sqlite3 *db;
    int rc;
    std::string sql;
    sqlite3_stmt *query;
    std::string data;

    sqlite3_open("auth.db", &db);

    sql = "SELECT pass from AUTH WHERE user=?1";
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &query, NULL);
    sqlite3_bind_text(query, 1, user.c_str(), user.size(), NULL);

    if (rc != SQLITE_OK)
    {
      BOOST_LOG_TRIVIAL(error) << "[SQLite] Error: " << sqlite3_errmsg(db);
      return rc;
    }

    rc = sqlite3_step(query);

    while (rc == SQLITE_ROW)
    {
      data = (const char *)sqlite3_column_text(query, 0);
      rc = sqlite3_step(query);
    }
    if (rc != SQLITE_DONE)
    {
      BOOST_LOG_TRIVIAL(error) << "[SQLite] Error: " << sqlite3_errmsg(db);
    }

    BOOST_LOG_TRIVIAL(info) << "[SQLite] Selected: " << user << " , " << data;
    pass = data;
    sqlite3_finalize(query);
    sqlite3_close(db);
    return rc;
  }

} // namespace database
