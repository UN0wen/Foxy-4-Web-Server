#pragma once


#include <string>
//various functions for sqlite
namespace database
{
    static int callback_select(void *pointer, int argc, char **argv, char **azColName);

    int db_create();

    int db_insert(std::string user, std::string pass);

    int db_select_pass(std::string user, std::string& pass);
} // namespace database
