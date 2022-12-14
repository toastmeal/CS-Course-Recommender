#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "../include/SQL.h"

using namespace std;

//=========================== GENERAL SQL COMMANDS =========================================

// Constructor
SQL::SQL(){

    // Attemp to connect to local database
    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        this->~SQL();
        exit(-1);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    // Create Tables
    cout << "Creating Tables" << endl;
    _createTable("Course Difficulty");
    _createTable("Computer Science Courses");
    _createTable("Breadth Courses");
    _createTable("User Database");

    cout << "Created tables" << endl;

    // Populate Tables from csv files in db directory
    _readData("../db/difficultyDatabase.csv", "Course Difficulty");
    _readData("../db/breadthCourses.csv", "Breadth Courses");
    _readData("../db/computerScienceClasses.csv", "Computer Science Courses");
}

// Destructor
SQL::~SQL() {

    // Closes connection to local database
    sqlite3_close(db);

    // dealloc private pointer variables
    if (zErrMsg != nullptr) {delete zErrMsg;}
}

// Getter for getting rc for testing reasons
int SQL::getRC() { return this->rc;}

// create type of SQL table in database based on passed in tableName
void SQL::_createTable(string tableName) {
    string sql;

    if (tableName == "Course Difficulty") {
        //SQL language: Create Table with passed in tableName
        sql = "CREATE TABLE IF NOT EXISTS 'Course Difficulty'"
                    "("
                    "name TEXT NOT NULL,"
                    "difficulty REAL NOT NULL,"
                    "UNIQUE(name, difficulty)"
                    ");";
    }
    if(tableName == "Breadth Courses") {

        sql = "CREATE TABLE IF NOT EXISTS 'Breadth Courses'"
                    "("
                    "name TEXT NOT NULL,"
                    "units INTEGER NOT NULL," 
                    "requirements TEXT NOT NULL,"
                    "UNIQUE(name, units, requirements)"
                    ");";
    }

    if(tableName == "User Database") {
        //SQL language: Create Table with passed in tableName
        sql = "CREATE TABLE IF NOT EXISTS 'User Database'"
                    "("
                    "username TEXT PRIMARY KEY NOT NULL,"
                    "password TEXT NOT NULL"
                    ");";
    }
    if(tableName == "Computer Science Courses") {
        sql = "CREATE TABLE IF NOT EXISTS 'Computer Science Courses'"
                    "("
                    "name TEXT NOT NULL,"
                    "units INTEGER NOT NULL,"
                    "requirements TEXT NOT NULL,"
                    "UNIQUE(name, units, requirements)"
                    ");";
    }
    _executeSQL(sql);
}

void SQL::createUserTable(string username) {
    string sql = "CREATE TABLE IF NOT EXISTS '" + username +"'"
                    "("
                    "breadth TEXT PRIMARY KEY NOT NULL"
                    ");";    
    _executeSQL(sql);
}

void SQL::insertTable(vector<string> columns, string tableName) {
    // Load insert test statement
    string sql = "INSERT INTO '" + tableName
               + "' VALUES(";
    
    for(int i = 0; i < columns.size(); ++i)
    {
        if(i == columns.size() - 1) sql += "'" + columns[i] + "');";
        else sql += "'" + columns[i] + "', ";
    }
    // Execute SQL Statement
    _executeSQL(sql);

}

void SQL::deleteDataFromTable(string tableName, string columnName, string condition) {
    string sql = "DELETE FROM '"
                + tableName
                + "' WHERE "
                + columnName
                + " = '"
                + condition
                + "';";

    
    _executeSQL(sql);
}

// Fetch a a list of data entries from a table
void SQL::_fetchSQL(string sql) {
    int pnRow;
    int pnColumn;
    char *cErrMsg;
    char **pazResult;

    rc = sqlite3_get_table(
        db,          /* An open database */
        sql.c_str(),     /* SQL to be evaluated */
        &pazResult,    /* Results of the query */
        &pnRow,           /* Number of result rows written here */
        &pnColumn,        /* Number of result columns written here */
        &cErrMsg       /* Error msg written here */
        );

    if (rc == SQLITE_OK){dataTable = new DataTable(pazResult, pnColumn, pnRow);}
    else {_showErrMsg(db);} 

    sqlite3_free_table(pazResult);
}

void SQL::_executeSQL(string SQLstatement) {
    sqlite3_stmt* statement;
    rc = sqlite3_prepare_v2(db, SQLstatement.c_str(), -1, &statement, NULL);
    if(rc == SQLITE_OK) sqlite3_step(statement);
    else _showErrMsg(db);
    sqlite3_finalize(statement);
}

// Fetch a a list of data entries from a table
void SQL::fetchTable(string tableName) {
    string sql = "SELECT * FROM '" +  tableName + "';";
    _fetchSQL(sql);
}

// Reading data from CSV file
void SQL::_readData(string fileName, string tableName) {

    ifstream inFS;
    vector<string> dataVector;
    string data;
    string line;
    
    cout << "Opening " + fileName + " file" << endl;
    inFS.open(fileName);

    if(!inFS.is_open()) {
        cout << "Error opening file" << endl;
        return;
    }
    else cout << "Success!" << endl;

    while(getline(inFS, line)) // Reads every line in CSV file
    {
        stringstream ss (line);
        while(getline(ss, data, ',')) dataVector.push_back(data); // Reads through the line and pushes data in data vector.
        insertTable(dataVector, tableName); // Insert to table
        dataVector.clear();
    }
    inFS.close();

}
void SQL::_showErrMsg(sqlite3* dbError) {
    const char* db_error_msg = sqlite3_errmsg(dbError);
    string s; 
    s.append(reinterpret_cast<const char*>(db_error_msg));
    cout << "ERROR: " + s << endl;
}
//=========================== COURSE RECOMMENDER COMMANDS =========================================
void SQL::easyClass(string requirementName, int limit, string tableName){
    string sql = "SELECT 'Course Difficulty'.name, 'Course Difficulty'.difficulty FROM 'Course Difficulty' "
                "INNER JOIN '"
                + tableName
                + "' ON '"
                + tableName 
                + "'.name = 'Course Difficulty'.name "
                + "WHERE '"
                + tableName
                +"'.requirements = '" + requirementName 
                + "' ORDER BY 'Course Difficulty'.difficulty ASC "
                + "LIMIT "
                + to_string(limit)
                + ";"; 
    _fetchSQL(sql);
}

void SQL::getValue(string selectionColumn, string relativeColumn, string find, string tableName) {
    string sql = "SELECT " 
                + selectionColumn
                + " FROM '"
                + tableName
                + "' WHERE "
                + relativeColumn
                +" = '" 
                + find 
                + "';";
    _fetchSQL(sql);
}

//=========================== LOGIN SYSTEM COMMANDS =========================================
bool SQL::doesExist(string given, string column, string tableName) {
    string sql = "SELECT * FROM '"
                + tableName
                + "' WHERE "
                + column
                + " = '"
                + given
                + "';";
    _fetchSQL(sql);
    return !this->dataTable->isEmpty();
}