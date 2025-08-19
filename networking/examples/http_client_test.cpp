#include "http/http_parser.h"
#include "network/socket.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace networking;
using namespace networking::http;

class HTTPClient {
public:
    static void test_server(const std::string& host, uint16_t port) {
        std::cout << "🧪 Testing HTTP Server at " << host << ":" << port << std::endl;
        std::cout << "================================================" << std::endl;
        
        // Test 1: GET /
        test_request("GET", host, port, "/", "");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Test 2: GET /api/hello
        test_request("GET", host, port, "/api/hello", "");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Test 3: POST /api/echo
        test_request("POST", host, port, "/api/echo", "Hello from HTTP Client!");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Test 4: GET /stats
        test_request("GET", host, port, "/stats", "");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Test 5: 404 test
        test_request("GET", host, port, "/nonexistent", "");
        
        std::cout << "================================================" << std::endl;
        std::cout << "✅ All tests completed!" << std::endl;
    }
    
private:
    static void test_request(const std::string& method, const std::string& host, 
                           uint16_t port, const std::string& path, const std::string& body) {
        try {
            std::cout << "\n🔄 Testing: " << method << " " << path << std::endl;
            
            // Create socket
            auto socket_result = Socket::create(Socket::Type::TCP);
            if (!socket_result) {
                std::cerr << "❌ Failed to create socket: " << socket_result.error() << std::endl;
                return;
            }
            auto socket = std::move(*socket_result);
            
            // Connect to server
            auto addr_result = SocketAddress::from_ip_port(host, port);
            if (!addr_result) {
                std::cerr << "❌ Failed to create address: " << addr_result.error() << std::endl;
                return;
            }
            
            auto connect_result = socket.connect(*addr_result);
            if (!connect_result) {
                std::cerr << "❌ Failed to connect: " << connect_result.error() << std::endl;
                return;
            }
            
            // Create HTTP request
            Method http_method = Method::GET;
            if (method == "POST") http_method = Method::POST;
            else if (method == "PUT") http_method = Method::PUT;
            else if (method == "DELETE") http_method = Method::DELETE;
            
            Request request(http_method, path, Version{1, 1});
            request.set_header("Host", host + ":" + std::to_string(port));
            request.set_header("User-Agent", "C++ HTTP Client/1.0");
            request.set_header("Connection", "close");
            
            if (!body.empty()) {
                request.set_body(body);
                request.set_header("Content-Type", "text/plain");
            }
            
            // Send request
            std::string request_str = request.to_string();
            auto send_result = socket.send(request_str.data(), request_str.size());
            
            if (!send_result) {
                std::cerr << "❌ Failed to send request: " << send_result.error() << std::endl;
                return;
            }
            
            std::cout << "📤 Sent request (" << request_str.size() << " bytes)" << std::endl;
            
            // Receive response
            std::vector<uint8_t> buffer(8192);
            auto recv_result = socket.recv(buffer.data(), buffer.size());
            if (!recv_result) {
                std::cerr << "❌ Failed to receive response: " << recv_result.error() << std::endl;
                return;
            }
            
            size_t bytes_received = *recv_result;
            buffer.resize(bytes_received);
            
            std::cout << "📥 Received response (" << bytes_received << " bytes)" << std::endl;
            
            // Parse response (basic parsing for demo)
            std::string response_str(buffer.begin(), buffer.end());
            
            // Extract status line
            size_t first_line_end = response_str.find("\r\n");
            if (first_line_end != std::string::npos) {
                std::string status_line = response_str.substr(0, first_line_end);
                std::cout << "📊 Status: " << status_line << std::endl;
                
                // Find body start
                size_t headers_end = response_str.find("\r\n\r\n");
                if (headers_end != std::string::npos) {
                    std::string body = response_str.substr(headers_end + 4);
                    
                    // Show first 200 chars of body for readability
                    if (body.length() > 200) {
                        std::cout << "📄 Body preview: " << body.substr(0, 200) << "..." << std::endl;
                    } else if (!body.empty()) {
                        std::cout << "📄 Body: " << body << std::endl;
                    }
                }
            }
            
            std::cout << "✅ Request completed successfully" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "❌ Error during request: " << e.what() << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << "🚀 C++ HTTP Client Test Suite" << std::endl;
    std::cout << "Testing your production-grade networking library!" << std::endl;
    std::cout << "================================================" << std::endl;
    
    std::string host = "127.0.0.1";
    uint16_t port = 8080;
    
    if (argc >= 2) {
        host = argv[1];
    }
    if (argc >= 3) {
        port = static_cast<uint16_t>(std::stoi(argv[2]));
    }
    
    std::cout << "🎯 Target: http://" << host << ":" << port << std::endl;
    std::cout << "💡 Make sure the HTTP server is running first!" << std::endl;
    std::cout << "⏱️  Starting tests in 2 seconds..." << std::endl;
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    HTTPClient::test_server(host, port);
    
    return 0;
}
