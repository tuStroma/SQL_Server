#include <iostream>
#include <sqlite3.h>
#include "Server.h"
#include "DataBase.h"

int main()
{
    DataBase db = DataBase("example.db");
    db.setup();
    db.initData();
    
    //db.select("SELECT * FROM USER;").print();
    common::SQL_table t = db.select("SELECT * FROM EMPLOYEE;");
    t.print();
    void* serialized;
    int size = t.Serialize(&serialized);

    common::SQL_table t2 = common::SQL_table::Deserialize(serialized, size);
    t2.print();

    db.select("SELECT * FROM PROJECT;").print();
    //db.select("SELECT timediff('now', EMPLOYED) FROM EMPLOYEE;").print();
    //db.select("SELECT * FROM PERSON;").print();
    //
    //
    //db.select(  "SELECT PRIVILIDGE FROM USER"
    //            "   WHERE USERNAME == 'stroma';").print();


    // Database metadata
    db.select("SELECT name, type FROM sqlite_schema  WHERE type = 'table' ORDER BY name; ").print();
    db.select("PRAGMA table_info(USER);").print();

    SQL_Server server(60001, db);
    server.Start();


    while (true)
    {
        std::string command;
        std::cin >> command;

        if (command == "q")
            break;
        else if (command == "d")
        {

        }
        else if (command == "w")
        {
        }
    }
    server.Stop();

    return 0;
}