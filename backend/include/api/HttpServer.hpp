#ifndef TRADEVERSE_HTTP_SERVER_HPP
#define TRADEVERSE_HTTP_SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <memory>
#include <sstream>
#include <map>
#include <algorithm>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

namespace tradeverse {
namespace api {

struct HttpRequest {
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string authUserToken;
};

struct HttpResponse {
    int statusCode;
    std::string contentType;
    std::string body;

    HttpResponse(int code = 200, const std::string& b = "", const std::string& ct = "application/json")
        : statusCode(code), contentType(ct), body(b) {}
};

using RequestHandler = std::function<HttpResponse(const HttpRequest&)>;

class HttpServer {
private:
    int port;
    bool running;
    std::thread serverThread;
    std::map<std::pair<std::string, std::string>, RequestHandler> routes;
    std::vector<std::pair<std::string, RequestHandler>> prefixRoutes;
    std::mutex routeMutex;
    SOCKET serverSocket;

public:
    explicit HttpServer(int p = 8080) : port(p), running(false), serverSocket(INVALID_SOCKET) {
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    }

    ~HttpServer() {
        stop();
#ifdef _WIN32
        WSACleanup();
#endif
    }

    void addRoute(const std::string& method, const std::string& path, RequestHandler handler) {
        std::lock_guard<std::mutex> lock(routeMutex);
        routes[{method, path}] = handler;
    }

    void addPrefixRoute(const std::string& prefix, RequestHandler handler) {
        std::lock_guard<std::mutex> lock(routeMutex);
        prefixRoutes.push_back({prefix, handler});
    }

    void start() {
        running = true;
        serverThread = std::thread(&HttpServer::listenLoop, this);
    }

    void stop() {
        running = false;
        if (serverSocket != INVALID_SOCKET) {
#ifdef _WIN32
            closesocket(serverSocket);
#else
            close(serverSocket);
#endif
            serverSocket = INVALID_SOCKET;
        }
        if (serverThread.joinable()) {
            serverThread.detach();
        }
    }

private:
    void listenLoop() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "[HttpServer] Failed to create socket." << std::endl;
            return;
        }

        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "[HttpServer] Bind failed on port " << port << std::endl;
#ifdef _WIN32
            closesocket(serverSocket);
#else
            close(serverSocket);
#endif
            serverSocket = INVALID_SOCKET;
            return;
        }

        if (listen(serverSocket, 50) == SOCKET_ERROR) {
            std::cerr << "[HttpServer] Listen failed." << std::endl;
            return;
        }

        std::cout << "[TradeVerse Backend] HTTP Server running on http://127.0.0.1:" << port << std::endl;

        while (running) {
            sockaddr_in clientAddr{};
            int clientLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
            if (clientSocket != INVALID_SOCKET) {
                std::thread([this, clientSocket]() {
                    try {
                        this->handleClient(clientSocket);
                    } catch (const std::exception& e) {
                        std::cerr << "[Client Exception] " << e.what() << std::endl;
                    } catch (...) {
                        std::cerr << "[Client Unknown Exception]" << std::endl;
                    }
                }).detach();
            }
        }
    }

    void handleClient(SOCKET clientSocket) {
        std::string rawRequest;
        char buffer[4096];
        size_t contentLength = 0;
        bool headerParsed = false;
        size_t headerEnd = std::string::npos;
        size_t headerLength = 0;

        while (true) {
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) break;
            rawRequest.append(buffer, bytesRead);

            if (!headerParsed) {
                headerEnd = rawRequest.find("\r\n\r\n");
                headerLength = 4;
                if (headerEnd == std::string::npos) {
                    headerEnd = rawRequest.find("\n\n");
                    headerLength = 2;
                }

                if (headerEnd != std::string::npos) {
                    headerParsed = true;
                    // Look for Content-Length
                    std::string headerPart = rawRequest.substr(0, headerEnd);
                    std::string clPattern = "content-length:";
                    std::string lowerHeaders = headerPart;
                    std::transform(lowerHeaders.begin(), lowerHeaders.end(), lowerHeaders.begin(), ::tolower);
                    size_t clPos = lowerHeaders.find(clPattern);
                    if (clPos != std::string::npos) {
                        size_t endLine = lowerHeaders.find_first_of("\r\n", clPos);
                        std::string clStr = lowerHeaders.substr(clPos + clPattern.length(), endLine - (clPos + clPattern.length()));
                        size_t firstDigit = clStr.find_first_of("0123456789");
                        if (firstDigit != std::string::npos) {
                            try {
                                contentLength = std::stoul(clStr.substr(firstDigit));
                            } catch (...) {
                                contentLength = 0;
                            }
                        }
                    }
                }
            }

            if (headerParsed) {
                size_t bodyBytes = rawRequest.length() - (headerEnd + headerLength);
                if (bodyBytes >= contentLength) {
                    break;
                }
            }
        }

        if (rawRequest.empty()) {
#ifdef _WIN32
            closesocket(clientSocket);
#else
            close(clientSocket);
#endif
            return;
        }

        HttpRequest req = parseRequest(rawRequest);
        HttpResponse res;

        if (req.method == "OPTIONS") {
            res.statusCode = 204;
            res.body = "";
        } else {
            res = dispatch(req);
        }

        std::string responseStr = buildResponse(res);
        send(clientSocket, responseStr.c_str(), static_cast<int>(responseStr.length()), 0);

#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
    }

    HttpRequest parseRequest(const std::string& raw) {
        HttpRequest req;
        size_t headerEnd = raw.find("\r\n\r\n");
        size_t headerLength = 4;
        if (headerEnd == std::string::npos) {
            headerEnd = raw.find("\n\n");
            headerLength = 2;
        }

        std::string headerSection = (headerEnd != std::string::npos) ? raw.substr(0, headerEnd) : raw;
        if (headerEnd != std::string::npos && headerEnd + headerLength <= raw.length()) {
            req.body = raw.substr(headerEnd + headerLength);
        }

        std::istringstream stream(headerSection);
        std::string line;

        if (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            std::istringstream lineStream(line);
            lineStream >> req.method >> req.path;
        }

        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string headerName = line.substr(0, colon);
                std::string headerVal = line.substr(colon + 1);
                size_t first = headerVal.find_first_not_of(" \t");
                if (first != std::string::npos) headerVal = headerVal.substr(first);
                
                std::string lowerName = headerName;
                std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
                req.headers[lowerName] = headerVal;

                if (lowerName == "authorization") {
                    if (headerVal.rfind("Bearer ", 0) == 0) {
                        req.authUserToken = headerVal.substr(7);
                    } else {
                        req.authUserToken = headerVal;
                    }
                }
            }
        }

        return req;
    }

    HttpResponse dispatch(const HttpRequest& req) {
        std::lock_guard<std::mutex> lock(routeMutex);

        auto it = routes.find({req.method, req.path});
        if (it != routes.end()) {
            try {
                return it->second(req);
            } catch (const std::exception& e) {
                return HttpResponse(500, std::string("{\"error\": \"") + e.what() + "\"}");
            }
        }

        for (const auto& pair : prefixRoutes) {
            if (req.path.rfind(pair.first, 0) == 0) {
                try {
                    return pair.second(req);
                } catch (const std::exception& e) {
                    return HttpResponse(500, std::string("{\"error\": \"") + e.what() + "\"}");
                }
            }
        }

        return HttpResponse(404, "{\"error\": \"Endpoint not found\"}", "application/json");
    }

    std::string buildResponse(const HttpResponse& res) {
        std::ostringstream ss;
        ss << "HTTP/1.1 " << res.statusCode << " ";
        if (res.statusCode == 200) ss << "OK\r\n";
        else if (res.statusCode == 201) ss << "Created\r\n";
        else if (res.statusCode == 204) ss << "No Content\r\n";
        else if (res.statusCode == 400) ss << "Bad Request\r\n";
        else if (res.statusCode == 401) ss << "Unauthorized\r\n";
        else if (res.statusCode == 403) ss << "Forbidden\r\n";
        else if (res.statusCode == 404) ss << "Not Found\r\n";
        else ss << "Internal Server Error\r\n";

        ss << "Access-Control-Allow-Origin: *\r\n";
        ss << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
        ss << "Access-Control-Allow-Headers: Content-Type, Authorization, Accept\r\n";
        ss << "Content-Type: " << res.contentType << "\r\n";
        ss << "Content-Length: " << res.body.length() << "\r\n";
        ss << "Connection: close\r\n\r\n";
        ss << res.body;

        return ss.str();
    }
};

} // namespace api
} // namespace tradeverse

#endif // TRADEVERSE_HTTP_SERVER_HPP