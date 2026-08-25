#ifndef TRADEVERSE_DATABASE_HPP
#define TRADEVERSE_DATABASE_HPP

#include <string>
#include <vector>
#include <mutex>
#include <iostream>
#include <stdexcept>
#include "sqlite3.h"

#include "../models/User.hpp"
#include "../models/Order.hpp"
#include "../models/Trade.hpp"
#include "../models/Stock.hpp"
#include "../models/Portfolio.hpp"

namespace tradeverse {
namespace database {

class Database {
private:
    sqlite3* db;
    std::string dbPath;
    std::mutex dbMutex;

public:
    explicit Database(const std::string& path = "tradeverse.db") : db(nullptr), dbPath(path) {}

    ~Database() {
        close();
    }

    bool open() {
        std::lock_guard<std::mutex> lock(dbMutex);
        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc != SQLITE_OK) {
            std::cerr << "[DB Error] Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        // Enable Write-Ahead Logging (WAL) for concurrent read-write performance
        char* errMsg = nullptr;
        sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, &errMsg);
        return true;
    }

    void close() {
        std::lock_guard<std::mutex> lock(dbMutex);
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    bool execute(const std::string& sql) {
        std::lock_guard<std::mutex> lock(dbMutex);
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "[DB Exec Error] " << (errMsg ? errMsg : "Unknown") << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

    bool initSchema() {
        const char* schema = R"(
            CREATE TABLE IF NOT EXISTS users (
                user_id TEXT PRIMARY KEY,
                username TEXT UNIQUE NOT NULL,
                email TEXT UNIQUE NOT NULL,
                full_name TEXT NOT NULL,
                password_hash TEXT NOT NULL,
                salt TEXT NOT NULL,
                cash_balance REAL NOT NULL DEFAULT 1000000.0,
                created_at INTEGER NOT NULL,
                is_admin INTEGER NOT NULL DEFAULT 0
            );

            CREATE TABLE IF NOT EXISTS orders (
                order_id TEXT PRIMARY KEY,
                user_id TEXT NOT NULL,
                symbol TEXT NOT NULL,
                order_type TEXT NOT NULL,
                side TEXT NOT NULL,
                quantity INTEGER NOT NULL,
                filled_quantity INTEGER NOT NULL DEFAULT 0,
                price REAL NOT NULL,
                status TEXT NOT NULL,
                timestamp INTEGER NOT NULL
            );

            CREATE TABLE IF NOT EXISTS trades (
                trade_id TEXT PRIMARY KEY,
                buy_order_id TEXT NOT NULL,
                sell_order_id TEXT NOT NULL,
                symbol TEXT NOT NULL,
                quantity INTEGER NOT NULL,
                price REAL NOT NULL,
                buyer_id TEXT NOT NULL,
                seller_id TEXT NOT NULL,
                timestamp INTEGER NOT NULL
            );

            CREATE TABLE IF NOT EXISTS holdings (
                user_id TEXT NOT NULL,
                symbol TEXT NOT NULL,
                quantity INTEGER NOT NULL,
                average_buy_price REAL NOT NULL,
                PRIMARY KEY (user_id, symbol)
            );

            CREATE TABLE IF NOT EXISTS portfolio_snapshots (
                snapshot_id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id TEXT NOT NULL,
                total_portfolio_value REAL NOT NULL,
                cash_balance REAL NOT NULL,
                invested_value REAL NOT NULL,
                total_pnl REAL NOT NULL,
                timestamp INTEGER NOT NULL
            );
        )";
        return execute(schema);
    }

    bool saveOrder(const models::Order& order) {
        std::lock_guard<std::mutex> lock(dbMutex);
        const char* sql = "INSERT OR REPLACE INTO orders (order_id, user_id, symbol, order_type, side, quantity, filled_quantity, price, status, timestamp) "
                          "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

        sqlite3_bind_text(stmt, 1, order.orderId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, order.userId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, order.symbol.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, models::orderTypeToString(order.type).c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, models::orderSideToString(order.side).c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 6, order.quantity);
        sqlite3_bind_int(stmt, 7, order.filledQuantity);
        sqlite3_bind_double(stmt, 8, order.price);
        sqlite3_bind_text(stmt, 9, models::orderStatusToString(order.status).c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 10, order.timestamp);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    bool saveTrade(const models::Trade& trade) {
        std::lock_guard<std::mutex> lock(dbMutex);
        const char* sql = "INSERT OR REPLACE INTO trades (trade_id, buy_order_id, sell_order_id, symbol, quantity, price, buyer_id, seller_id, timestamp) "
                          "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

        sqlite3_bind_text(stmt, 1, trade.tradeId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, trade.buyOrderId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, trade.sellOrderId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, trade.symbol.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 5, trade.quantity);
        sqlite3_bind_double(stmt, 6, trade.price);
        sqlite3_bind_text(stmt, 7, trade.buyerId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 8, trade.sellerId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 9, trade.timestamp);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    std::vector<models::Order> getUserOrders(const std::string& userId) {
        std::lock_guard<std::mutex> lock(dbMutex);
        std::vector<models::Order> list;
        const char* sql = "SELECT order_id, user_id, symbol, order_type, side, quantity, filled_quantity, price, status, timestamp FROM orders WHERE user_id = ? ORDER BY timestamp DESC;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

        sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            models::Order o;
            o.orderId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            o.userId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            o.symbol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            o.type = models::stringToOrderType(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            o.side = models::stringToOrderSide(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            o.quantity = sqlite3_column_int(stmt, 5);
            o.filledQuantity = sqlite3_column_int(stmt, 6);
            o.price = sqlite3_column_double(stmt, 7);
            o.status = models::stringToOrderStatus(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
            o.timestamp = sqlite3_column_int64(stmt, 9);
            list.push_back(o);
        }
        sqlite3_finalize(stmt);
        return list;
    }

    std::vector<models::Trade> getUserTrades(const std::string& userId) {
        std::lock_guard<std::mutex> lock(dbMutex);
        std::vector<models::Trade> list;
        const char* sql = "SELECT trade_id, buy_order_id, sell_order_id, symbol, quantity, price, buyer_id, seller_id, timestamp FROM trades WHERE buyer_id = ? OR seller_id = ? ORDER BY timestamp DESC;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

        sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, userId.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            models::Trade t;
            t.tradeId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            t.buyOrderId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            t.sellOrderId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            t.symbol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            t.quantity = sqlite3_column_int(stmt, 4);
            t.price = sqlite3_column_double(stmt, 5);
            t.buyerId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            t.sellerId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
            t.timestamp = sqlite3_column_int64(stmt, 8);
            list.push_back(t);
        }
        sqlite3_finalize(stmt);
        return list;
    }
};

} // namespace database
} // namespace tradeverse

#endif // TRADEVERSE_DATABASE_HPP
