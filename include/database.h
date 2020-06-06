#pragma once


#include <string>
//various functions for sqlite
namespace database
{
    static int callback_select(void *pointer, int argc, char **argv, char **azColName);

    void db_create();

    void db_insert(std::string user, std::string pass);

    std::string db_select_pass(std::string user);
} // namespace database
