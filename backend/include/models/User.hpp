#ifndef TRADEVERSE_USER_HPP
#define TRADEVERSE_USER_HPP

#include <string>
#include <cstdint>

namespace tradeverse {
namespace models {

/**
 * @brief Represents an authenticated user profile.
 */
struct User {
    std::string userId;
    std::string username;
    std::string email;
    std::string fullName;
    std::string passwordHash;
    std::string salt;
    double cashBalance; // Virtual cash balance in INR (e.g. starting ₹10,00,000)
    int64_t createdAt;
    bool isAdmin;
};

} // namespace models
} // namespace tradeverse

#endif // TRADEVERSE_USER_HPP
