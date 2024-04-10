#pragma once
#include <iostream>
#include <sqlite3.h>
#include "../common/SQL_table.h"

class DataBase
{
	std::string db_file;
public:
	DataBase(std::string file) 
        :db_file(file)
    {}

    void dropDB()
    {
        std::remove("example.db");
    }

    void createTable(std::string create_table, std::string name)
    {
        sqlite3* DB;
        int exit = 0;
        char* messaggeError;

        exit = sqlite3_open(db_file.c_str(), &DB);

        exit = sqlite3_exec(DB, create_table.c_str(), NULL, 0, &messaggeError);

        if (exit != SQLITE_OK) {
            std::cerr << "Error Create " << name << " Table" << std::endl;
            sqlite3_free(messaggeError);
        }
        else
            std::cout << name << " Table created Successfully" << std::endl;

        sqlite3_close(DB);
    }

    void insert(std::string insert_query)
    {
        sqlite3* DB;
        char* messaggeError;
        int exit = sqlite3_open(db_file.c_str(), &DB);

        exit = sqlite3_exec(DB, insert_query.c_str(), NULL, 0, &messaggeError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error Insert: " << messaggeError << std::endl;
            sqlite3_free(messaggeError);
        }
        else
            std::cout << "Records created Successfully!" << std::endl;

        sqlite3_close(DB);
    }

	void setup()
	{
        dropDB();


        std::string user_table = 
            "CREATE     TABLE   USER("
            "USERNAME   TEXT    PRIMARY KEY NOT NULL, "
            "PASSWORD   TEXT    NOT NULL, "
            "PRIVILIDGE INT     NOT NULL DEFAULT '0');";

        std::string employee_table = 
            "CREATE     TABLE   EMPLOYEE("
            "ID         INT     PRIMARY KEY NOT NULL, "
            "NAME       TEXT    NOT NULL, "
            "SURNAME    TEXT    NOT NULL, "
            "EMPLOYED   DATE, "
            "BIRTHDATE  DATE, "
            "SALARY     DECIMAL );";

        std::string project_table =
            "CREATE     TABLE   PROJECT("
            "ID         INT     PRIMARY KEY NOT NULL, "
            "NAME       TEXT    NOT NULL, "
            "START      DATE, "
            "END        DATE, "
            "MANAGER    INT     NOT NULL);";

        createTable(user_table, "User");
        createTable(employee_table, "Employee");
        createTable(project_table, "Project");

        
	}

    void initData()
    {
        std::string sql("INSERT INTO USER VALUES('admin', 'admin', 2);"
                        "INSERT INTO USER VALUES('stroma', '1234', 1);"
                        "INSERT INTO USER VALUES('karol', 'karol', 0);"

                        "INSERT INTO EMPLOYEE VALUES(1, 'STEVE', 'GATES', '2015-08-01', '1995-12-01', 1000.0);"
                        "INSERT INTO EMPLOYEE VALUES(2, 'JULIUSZ', 'CEZAR', '1999-12-01', '1977-10-23', 8000.0);"
                        "INSERT INTO EMPLOYEE VALUES(3, 'AL', 'ADIN', '2003-05-05', '1989-09-12', 13000.0);"

                        "INSERT INTO PROJECT VALUES(1, 'CAR', '2023-05-05', '2024-05-05', 3);"
                        "INSERT INTO PROJECT VALUES(2, 'FACEBOOK', '2024-05-05', '2025-05-05', 3);"
                        "INSERT INTO PROJECT VALUES(3, 'GTA', '2020-01-05', '2022-05-05', 2);");

        insert(sql);
    }

    static int select_callback(void* data, int argc, char** argv, char** azColName)
    {
        common::SQL_table* table = (common::SQL_table*)data;
        
        table->SetColNames(azColName, argc);
        table->addRow(argv, argc);

        return 0;
    }

    common::SQL_table select(std::string select_query)
    {
        sqlite3* DB;
        char* messaggeError;
        int exit = sqlite3_open(db_file.c_str(), &DB);

        common::SQL_table table = common::SQL_table();
        exit = sqlite3_exec(DB, select_query.c_str(), select_callback, &table, &messaggeError);

        //sqlite3_close(DB);

        return table;
    }
};