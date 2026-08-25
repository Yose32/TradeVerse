#ifndef TRADEVERSE_AUTH_MANAGER_HPP
#define TRADEVERSE_AUTH_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <mutex>
#include <random>
#include <chrono>
#include <regex>

#include "SHA256.hpp"
#include "../models/User.hpp"

namespace tradeverse {
namespace auth {

struct AuthResult {
    bool success;
    std::string token;
    models::User user;
    std::string errorMessage;
};

/**
 * @brief Handles user authentication, token generation, and secure credential storage.
 */
class AuthManager {
private:
    std::unordered_map<std::string, models::User> usersById;
    std::unordered_map<std::string, std::string> usernameToId;
    std::unordered_map<std::string, std::string> emailToId;
    std::unordered_map<std::string, std::string> tokenToUserId;
    std::mutex authMutex;

    std::string generateRandomString(size_t length = 32) {
        static const char charset[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);
        std::string s;
        s.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            s += charset[dis(gen)];
        }
        return s;
    }

    int64_t getCurrentTimeMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

public:
    AuthManager() {
        seedDefaultUsers();
    }

    void seedDefaultUsers() {
        registerUser("demo_trader", "demo@tradeverse.com", "Demo Trader", "DemoPass123!", false);
        registerUser("alex_invest", "alex@tradeverse.com", "Alex Mercer", "AlexPass123!", false);
        registerUser("sarah_quant", "sarah@tradeverse.com", "Sarah Chen", "SarahPass123!", false);
        registerUser("rahul_alpha", "rahul@tradeverse.com", "Rahul Sharma", "RahulPass123!", false);
        registerUser("admin", "admin@tradeverse.com", "System Administrator", "AdminTradeVerse2026!", true);
    }

    AuthResult registerUser(const std::string& username, const std::string& email,
                             const std::string& fullName, const std::string& password,
                             bool isAdmin = false) {
        std::lock_guard<std::mutex> lock(authMutex);
        AuthResult res;

        if (username.length() < 3) {
            res.success = false;
            res.errorMessage = "Username must be at least 3 characters long.";
            return res;
        }

        if (password.length() < 6) {
            res.success = false;
            res.errorMessage = "Password must be at least 6 characters long.";
            return res;
        }

        if (usernameToId.find(username) != usernameToId.end()) {
            res.success = false;
            res.errorMessage = "Username already exists.";
            return res;
        }

        if (emailToId.find(email) != emailToId.end()) {
            res.success = false;
            res.errorMessage = "Email is already registered.";
            return res;
        }

        std::string userId = "USR-" + std::to_string(usersById.size() + 1001);
        std::string salt = generateRandomString(16);
        std::string passwordHash = SHA256::hashWithSalt(password, salt);

        models::User user;
        user.userId = userId;
        user.username = username;
        user.email = email;
        user.fullName = fullName.empty() ? username : fullName;
        user.passwordHash = passwordHash;
        user.salt = salt;
        user.cashBalance = 1000000.0; // ₹10,00,000 virtual capital
        user.createdAt = getCurrentTimeMs();
        user.isAdmin = isAdmin;

        usersById[userId] = user;
        usernameToId[username] = userId;
        emailToId[email] = userId;

        std::string token = "tv_sess_" + generateRandomString(32);
        tokenToUserId[token] = userId;

        res.success = true;
        res.token = token;
        res.user = user;
        res.user.passwordHash = ""; // Scrub sensitive hash
        res.user.salt = "";
        return res;
    }

    AuthResult loginUser(const std::string& identifier, const std::string& password) {
        std::lock_guard<std::mutex> lock(authMutex);
        AuthResult res;

        std::string userId;
        auto uIt = usernameToId.find(identifier);
        if (uIt != usernameToId.end()) {
            userId = uIt->second;
        } else {
            auto eIt = emailToId.find(identifier);
            if (eIt != emailToId.end()) {
                userId = eIt->second;
            } else {
                res.success = false;
                res.errorMessage = "Invalid username or password.";
                return res;
            }
        }

        const auto& user = usersById[userId];
        std::string testHash = SHA256::hashWithSalt(password, user.salt);
        if (testHash != user.passwordHash) {
            res.success = false;
            res.errorMessage = "Invalid username or password.";
            return res;
        }

        std::string token = "tv_sess_" + generateRandomString(32);
        tokenToUserId[token] = userId;

        res.success = true;
        res.token = token;
        res.user = user;
        res.user.passwordHash = "";
        res.user.salt = "";
        return res;
    }

    bool validateToken(const std::string& token, models::User& outUser) {
        std::lock_guard<std::mutex> lock(authMutex);
        auto it = tokenToUserId.find(token);
        if (it == tokenToUserId.end()) {
            return false;
        }
        auto uIt = usersById.find(it->second);
        if (uIt == usersById.end()) {
            return false;
        }
        outUser = uIt->second;
        outUser.passwordHash = "";
        outUser.salt = "";
        return true;
    }

    void logout(const std::string& token) {
        std::lock_guard<std::mutex> lock(authMutex);
        tokenToUserId.erase(token);
    }

    models::User* getUserById(const std::string& userId) {
        std::lock_guard<std::mutex> lock(authMutex);
        auto it = usersById.find(userId);
        if (it != usersById.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    std::vector<models::User> getAllUsers() {
        std::lock_guard<std::mutex> lock(authMutex);
        std::vector<models::User> list;
        for (const auto& pair : usersById) {
            models::User sanitized = pair.second;
            sanitized.passwordHash = "";
            sanitized.salt = "";
            list.push_back(sanitized);
        }
        return list;
    }
};

} // namespace auth
} // namespace tradeverse

#endif // TRADEVERSE_AUTH_MANAGER_HPP
