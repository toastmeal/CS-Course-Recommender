// SQL.h
#include <string>
#include <vector>
#include <sqlite3.h> 

using namespace std;

#ifndef __SQL_H__
#define __SQL_H__

class SQL {
    private:
        // Variables
        sqlite3 *db;
        char *zErrMsg;
        int rc;
 
        //2D Vector thats stores info for SQL Table Columns (column name, data type, condition)
        vector< vector<string> > coursesDifficultyOutline = {
            {"name", "TEXT", "PRIMARY KEY", "NOT NULL"},
            {"difficulty", "INT", "NOT NULL"}
        };

        // create type of SQL table in database based on passed in tableName
        void _createTable(string tableName);

        // Insert data into test table
        void _insertTestTable(string name, string age);

        //_easyClass("SOC-A") -> "ECON 003"
        string _easyClass(string requirementName);

        string _getRating(string courseName);

    public:

        // Constructor
        SQL();

        // Destructor
        ~SQL();

        //Getters
        void createTable(string tableName) {_createTable(tableName);}
        void insertTestTable(string name, string age) {_insertTestTable(name, age);}

        // Prints out the data entries of test table
        void printTestTable();

        //getEasiestClass
        string getEasiestCourse(string requirementName, int limit = 3);
};

#endif