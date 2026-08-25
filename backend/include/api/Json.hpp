#ifndef TRADEVERSE_JSON_HPP
#define TRADEVERSE_JSON_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace tradeverse {
namespace json {

inline std::string escapeString(const std::string& str) {
    std::ostringstream ss;
    for (char c : str) {
        switch (c) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < ' ') {
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    ss << c;
                }
        }
    }
    return ss.str();
}

inline std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) {
        start++;
    }
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(static_cast<unsigned char>(*end)));
    return std::string(start, end + 1);
}

/**
 * @brief Robust key-value extractor for JSON payloads.
 */
inline std::string extractString(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos + searchKey.length());
    if (pos == std::string::npos) return "";

    // Skip whitespace
    pos = json.find_first_not_of(" \t\r\n", pos + 1);
    if (pos == std::string::npos) return "";

    if (json[pos] == '"') {
        // String value
        size_t start = pos + 1;
        std::string result;
        bool escape = false;
        for (size_t i = start; i < json.length(); ++i) {
            char c = json[i];
            if (escape) {
                result += c;
                escape = false;
            } else if (c == '\\') {
                escape = true;
            } else if (c == '"') {
                return result;
            } else {
                result += c;
            }
        }
        return result;
    } else {
        // Numeric, boolean, or null value
        size_t endPos = json.find_first_of(",}\r\n \t", pos);
        if (endPos != std::string::npos) {
            return json.substr(pos, endPos - pos);
        }
        return json.substr(pos);
    }
}

inline double extractDouble(const std::string& json, const std::string& key, double defaultVal = 0.0) {
    std::string valStr = extractString(json, key);
    if (valStr.empty()) return defaultVal;
    try {
        return std::stod(valStr);
    } catch (...) {
        return defaultVal;
    }
}

inline int extractInt(const std::string& json, const std::string& key, int defaultVal = 0) {
    std::string valStr = extractString(json, key);
    if (valStr.empty()) return defaultVal;
    try {
        return std::stoi(valStr);
    } catch (...) {
        return defaultVal;
    }
}

} // namespace json
} // namespace tradeverse

#endif // TRADEVERSE_JSON_HPP