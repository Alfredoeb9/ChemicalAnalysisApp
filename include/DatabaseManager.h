#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <sqlite3.h>
#include <string>
#include <iostream>
#include <stdexcept>

/**
 * class: DatabaseManager
 * @description:
 *  - Manages SQLite database interactions for chemical fluid analysis results.
 *  - Acts as a Data Access Object (DAO) encapsulating all database operations, including schema initialization, transaction management, and query execution.
 */
class DatabaseManager {
    private:
        // SQLite database handle
        sqlite3* db;

        /**
         * function: initializeSchema
         * @description:
         * - Initializes the database schema by creating necessary tables if they do not already exist.
         * @returns: void
         */
        void initializeSchema() {
            const char* sql = "CREATE TABLE IF NOT EXISTS AnalysisResults ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "session_id INTEGER,"
                            "substance_name TEXT,"
                            "concentration REAL,"
                            "units TEXT,"
                            "detection_limit REAL,"
                            "result_status TEXT);";

            char* ErrMsg = nullptr;
            // Execute the SQL statement to create the table
            int returnCode = sqlite3_exec(db, sql, nullptr, nullptr, &ErrMsg);

            if (returnCode != SQLITE_OK) {
                std::string err = ErrMsg;
                // Free the error message buffer allocated by SQLite
                sqlite3_free(ErrMsg);

                throw std::runtime_error("Database schema initialization failed: " + err);
            }
        }

    public:
        /**
         * constructor: DatabaseManager
         * @description:
         * - Opens a connection to the SQLite database at the specified path and initializes the schema.
         * @param dbPath: The file path to the SQLite database.
         * @returns: void
         */
        DatabaseManager(const std::string& dbPath) : db(nullptr) {
            // Attempt to open the SQLite database
            if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
                throw std::runtime_error("Failed to open SQLite database: " + std::string(sqlite3_errmsg(db)));
            }

            // Initialize the database schema
            initializeSchema();
        }

        /**
         * destructor: ~DatabaseManager
         * @description:
         * - Closes the database connection when the DatabaseManager object is destroyed to ensure proper resource cleanup.
         * @returns: void
         */
        ~DatabaseManager() {
            if (db) {
                sqlite3_close(db);
            }
        }

        /**
         * function: beginTransaction
         * @description:
         * - Begins a new database transaction. If the operation fails, an exception is thrown with the error message.
         * @returns: void
         */
        void beginTransaction() {
            // Create a buffer for the error message
            char* ErrMsg = nullptr;
            // Execute the SQL statement to begin a transaction
            int returnCode = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &ErrMsg);

            // Check if the transaction began successfully
            if (returnCode != SQLITE_OK) {
                std::string err = ErrMsg;
                sqlite3_free(ErrMsg);
                throw std::runtime_error("Failed to begin transaction: " + err);
            }
        }

        /**
         * function: commitTransaction
         * @description:
         * - Commits the current database transaction. If the operation fails, an exception is thrown with the error message.
         * @returns: void
         */
        void commitTransaction() {
            // Create a buffer for the error message
            char* ErrMsg = nullptr;
            // Execute the SQL statement to commit the transaction
            int returnCode = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &ErrMsg);
            // Check if the transaction was committed successfully
            if (returnCode != SQLITE_OK) {
                std::string err = ErrMsg;
                sqlite3_free(ErrMsg);
                throw std::runtime_error("Failed to commit transaction: " + err);
            }
        }

        /**
         * function: rollbackTransaction
         * @description:
         * - Rolls back the current database transaction. If the operation fails, an exception is thrown with the error message.
         * @returns: void
         */
        void rollbackTransaction() {
            // does there need to be error checks here?
            char* ErrMsg = nullptr;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, &ErrMsg);
            if (ErrMsg) {
                std::string err = ErrMsg;
                sqlite3_free(ErrMsg);
                throw std::runtime_error("Failed to rollback transaction: " + err);
            }
            std::cout << "[Transaction] Rollback executed due to upstream process failure." << std::endl;
        }

        /**
         * function: simulateHardwareInput
         * @description:
         * - Simulates hardware input by inserting a new analysis result into the database.
         * @param sampleId: The ID of the sample to insert.
         * @returns: void
         */
        void simulateHardwareInput(int sampleId) {

            // Simulated data generation for demonstration purposes
            std::string substance = "SensorSample_" + std::to_string(sampleId);
            // Generate a concentration value based on the sample ID to create variability in the data
            double value = 0.1 + (sampleId % 5) * 0.25; 
            // Units and detection limit are hardcoded for this simulation
            const char* unit = "ppm";
            double detection_limit = 0.05;

            // SQL statement for inserting analysis results
            std::string sql = "INSERT INTO AnalysisResults (session_id, substance_name, concentration, units, detection_limit, result_status) "
                            "VALUES (1, ?, ?, ?, ?, 'Detected');";
            
            // sqilite3_stmt is a structure used to represent a prepared statement in SQLite. 
            // It is used to execute SQL statements with parameters in a safe and efficient manner.
            sqlite3_stmt* stmt = nullptr;
            // Check if the SQL statement was prepared successfully
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                return;
            }

            // Memory-safe transient binding due to local function scope
            sqlite3_bind_text(stmt, 1, substance.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 2, value);
            sqlite3_bind_text(stmt, 3, unit, -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 4, detection_limit);

            // Execute the prepared statement to insert the data into the database
            sqlite3_step(stmt);
            // Finalize the statement to release resourcess allocated by SQLite for this statement
            sqlite3_finalize(stmt);
        }

        // Evaluation Engine Query
        std::string fetchFlaggedViolations(double threshold) const {
            std::string sql = "SELECT substance_name, concentration, units FROM AnalysisResults WHERE concentration > ?;";
            sqlite3_stmt* stmt = nullptr;
            std::string resultBuffer = "";

            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_double(stmt, 1, threshold);

                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    double conc = sqlite3_column_double(stmt, 1);
                    std::string units = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

                    resultBuffer += "[!] VIOLATION: " + name + " | " + std::to_string(conc) + " " + units + "\n";
                }
            }
            sqlite3_finalize(stmt);
            return resultBuffer;
        }
};

#endif // DATABASEMANAGER_H