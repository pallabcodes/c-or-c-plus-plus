#include <gtest/gtest.h>
#include "http/http_parser.h"
#include "websocket/frame_parser.h"
#include "network/socket.h"
#include <sstream>

using namespace networking;

// ================================================================================================
// HTTP Parser Tests
// ================================================================================================

class HttpParserTest : public ::testing::Test {
protected:
    http::RequestParser parser;
};

TEST_F(HttpParserTest, ParseSimpleGetRequest) {
    std::string request_data = 
        "GET /hello HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Test/1.0\r\n"
        "\r\n";
    
    std::vector<uint8_t> data(request_data.begin(), request_data.end());
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result.has_value());
    auto [request, consumed] = *result;
    
    EXPECT_EQ(request.method(), http::Method::GET);
    EXPECT_EQ(request.uri(), "/hello");
    EXPECT_EQ(request.version().major, 1);
    EXPECT_EQ(request.version().minor, 1);
    EXPECT_EQ(request.get_header("Host"), "example.com");
    EXPECT_EQ(request.get_header("User-Agent"), "Test/1.0");
    EXPECT_EQ(consumed, request_data.length());
}

TEST_F(HttpParserTest, ParsePostRequestWithBody) {
    std::string request_data = 
        "POST /api/data HTTP/1.1\r\n"
        "Host: api.example.com\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 25\r\n"
        "\r\n"
        "{\"name\":\"test\",\"id\":123}";
    
    std::vector<uint8_t> data(request_data.begin(), request_data.end());
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result.has_value());
    auto [request, consumed] = *result;
    
    EXPECT_EQ(request.method(), http::Method::POST);
    EXPECT_EQ(request.uri(), "/api/data");
    EXPECT_EQ(request.get_header("Content-Type"), "application/json");
    EXPECT_EQ(request.get_header("Content-Length"), "25");
    
    std::string body(request.body().begin(), request.body().end());
    EXPECT_EQ(body, "{\"name\":\"test\",\"id\":123}");
}

TEST_F(HttpParserTest, ParseChunkedRequest) {
    std::string request_data = 
        "POST /upload HTTP/1.1\r\n"
        "Host: upload.example.com\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "7\r\n"
        "Mozilla\r\n"
        "9\r\n"
        "Developer\r\n"
        "7\r\n"
        "Network\r\n"
        "0\r\n"
        "\r\n";
    
    std::vector<uint8_t> data(request_data.begin(), request_data.end());
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result.has_value());
    auto [request, consumed] = *result;
    
    EXPECT_EQ(request.method(), http::Method::POST);
    EXPECT_EQ(request.get_header("Transfer-Encoding"), "chunked");
    
    std::string body(request.body().begin(), request.body().end());
    EXPECT_EQ(body, "MozillaDeveloperNetwork");
}

TEST_F(HttpParserTest, ParseIncompleteRequest) {
    std::string partial_data = "GET /hello HTTP/1.1\r\nHost: exa";
    
    std::vector<uint8_t> data(partial_data.begin(), partial_data.end());
    auto result = parser.parse(data);
    
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), http::ParseError::INCOMPLETE);
}

TEST_F(HttpParserTest, ParseInvalidMethod) {
    std::string invalid_data = "INVALID /hello HTTP/1.1\r\n\r\n";
    
    std::vector<uint8_t> data(invalid_data.begin(), invalid_data.end());
    auto result = parser.parse(data);
    
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), http::ParseError::INVALID_FORMAT);
}

TEST_F(HttpParserTest, HeaderCaseInsensitive) {
    http::HeaderMap headers;
    headers.set("content-type", "application/json");
    headers.set("Content-Length", "100");
    headers.set("HOST", "example.com");
    
    EXPECT_EQ(headers.get("Content-Type"), "application/json");
    EXPECT_EQ(headers.get("content-length"), "100");
    EXPECT_EQ(headers.get("host"), "example.com");
}

// ================================================================================================
// WebSocket Frame Parser Tests
// ================================================================================================

class WebSocketFrameTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(WebSocketFrameTest, ParseSimpleTextFrame) {
    // Create a simple text frame: FIN=1, TEXT opcode, no mask, "Hello" payload
    std::vector<uint8_t> frame_data = {
        0x81,        // FIN=1, opcode=1 (TEXT)
        0x05,        // MASK=0, len=5
        'H', 'e', 'l', 'l', 'o'
    };
    
    auto result = websocket::FrameParser::parse_frame(frame_data);
    
    ASSERT_TRUE(result.has_value());
    auto [frame, consumed] = *result;
    
    EXPECT_TRUE(frame.fin);
    EXPECT_EQ(frame.opcode, websocket::Opcode::TEXT);
    EXPECT_FALSE(frame.masked);
    EXPECT_EQ(frame.get_text(), "Hello");
    EXPECT_EQ(consumed, frame_data.size());
}

TEST_F(WebSocketFrameTest, ParseMaskedTextFrame) {
    // Create masked text frame
    std::vector<uint8_t> frame_data = {
        0x81,                    // FIN=1, opcode=1 (TEXT)
        0x85,                    // MASK=1, len=5
        0x37, 0xfa, 0x21, 0x3d, // Mask key
        0x7f, 0x9f, 0x4d, 0x51, 0x58  // Masked "Hello"
    };
    
    auto result = websocket::FrameParser::parse_frame(frame_data);
    
    ASSERT_TRUE(result.has_value());
    auto [frame, consumed] = *result;
    
    EXPECT_TRUE(frame.fin);
    EXPECT_EQ(frame.opcode, websocket::Opcode::TEXT);
    EXPECT_TRUE(frame.masked);
    EXPECT_EQ(frame.get_text(), "Hello");
}

TEST_F(WebSocketFrameTest, ParseExtendedLengthFrame) {
    // Create frame with 16-bit length
    std::vector<uint8_t> frame_data = {
        0x81,        // FIN=1, opcode=1 (TEXT)
        0x7E,        // MASK=0, len=126 (use 16-bit length)
        0x01, 0x00   // Length = 256
    };
    
    // Add 256 'A' characters
    for (int i = 0; i < 256; ++i) {
        frame_data.push_back('A');
    }
    
    auto result = websocket::FrameParser::parse_frame(frame_data);
    
    ASSERT_TRUE(result.has_value());
    auto [frame, consumed] = *result;
    
    EXPECT_TRUE(frame.fin);
    EXPECT_EQ(frame.opcode, websocket::Opcode::TEXT);
    EXPECT_EQ(frame.payload.size(), 256);
    EXPECT_EQ(frame.get_text(), std::string(256, 'A'));
}

TEST_F(WebSocketFrameTest, ParseCloseFrame) {
    // Create close frame with code 1000 and reason "Normal"
    std::vector<uint8_t> frame_data = {
        0x88,        // FIN=1, opcode=8 (CLOSE)
        0x08,        // MASK=0, len=8
        0x03, 0xE8,  // Close code 1000 (Normal closure)
        'N', 'o', 'r', 'm', 'a', 'l'  // Reason
    };
    
    auto result = websocket::FrameParser::parse_frame(frame_data);
    
    ASSERT_TRUE(result.has_value());
    auto [frame, consumed] = *result;
    
    EXPECT_TRUE(frame.fin);
    EXPECT_EQ(frame.opcode, websocket::Opcode::CLOSE);
    
    auto [close_code, reason] = frame.get_close_info();
    EXPECT_EQ(close_code, websocket::CloseCode::NORMAL_CLOSURE);
    EXPECT_EQ(reason, "Normal");
}

TEST_F(WebSocketFrameTest, ParsePingFrame) {
    std::vector<uint8_t> frame_data = {
        0x89,        // FIN=1, opcode=9 (PING)
        0x04,        // MASK=0, len=4
        'p', 'i', 'n', 'g'
    };
    
    auto result = websocket::FrameParser::parse_frame(frame_data);
    
    ASSERT_TRUE(result.has_value());
    auto [frame, consumed] = *result;
    
    EXPECT_TRUE(frame.fin);
    EXPECT_EQ(frame.opcode, websocket::Opcode::PING);
    EXPECT_EQ(frame.get_text(), "ping");
}

TEST_F(WebSocketFrameTest, SerializeTextFrame) {
    auto frame = websocket::FrameParser::create_text_frame("Hello, World!");
    auto serialized = websocket::FrameParser::serialize_frame(frame);
    
    // Should be: 0x81 (FIN=1, TEXT) + 0x0D (len=13) + payload
    std::vector<uint8_t> expected = {0x81, 0x0D};
    std::string text = "Hello, World!";
    expected.insert(expected.end(), text.begin(), text.end());
    
    EXPECT_EQ(serialized, expected);
}

TEST_F(WebSocketFrameTest, SerializeMaskedFrame) {
    auto frame = websocket::FrameParser::create_text_frame("Test");
    auto serialized = websocket::FrameParser::serialize_frame(frame, true); // Force masking
    
    // Parse it back to verify
    auto result = websocket::FrameParser::parse_frame(serialized);
    ASSERT_TRUE(result.has_value());
    
    auto [parsed_frame, consumed] = *result;
    EXPECT_EQ(parsed_frame.get_text(), "Test");
    EXPECT_TRUE(parsed_frame.masked);
}

TEST_F(WebSocketFrameTest, InvalidOpcodeReturnsError) {
    std::vector<uint8_t> frame_data = {
        0x83,        // FIN=1, opcode=3 (reserved/invalid)
        0x00         // MASK=0, len=0
    };
    
    auto result = websocket::FrameParser::parse_frame(frame_data);
    
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), websocket::ParseError::INVALID_OPCODE);
}

TEST_F(WebSocketFrameTest, ControlFrameFragmentationError) {
    std::vector<uint8_t> frame_data = {
        0x08,        // FIN=0, opcode=8 (CLOSE) - fragmented control frame
        0x00         // MASK=0, len=0
    };
    
    auto result = websocket::FrameParser::parse_frame(frame_data);
    
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), websocket::ParseError::FRAGMENTED_CONTROL_FRAME);
}

// ================================================================================================
// Message Reassembly Tests
// ================================================================================================

TEST_F(WebSocketFrameTest, MessageReassembly) {
    websocket::MessageReassembler reassembler;
    
    // Create fragmented message: "Hello" + " " + "World!"
    auto frame1 = websocket::FrameParser::create_text_frame("Hello", false); // FIN=0
    frame1.opcode = websocket::Opcode::TEXT;
    
    websocket::Frame frame2;
    frame2.fin = false;
    frame2.opcode = websocket::Opcode::CONTINUATION;
    frame2.set_text(" ");
    
    websocket::Frame frame3;
    frame3.fin = true;
    frame3.opcode = websocket::Opcode::CONTINUATION;
    frame3.set_text("World!");
    
    // Add frames to reassembler
    auto result1 = reassembler.add_frame(std::move(frame1));
    EXPECT_FALSE(result1.has_value()); // Not complete yet
    
    auto result2 = reassembler.add_frame(std::move(frame2));
    EXPECT_FALSE(result2.has_value()); // Still not complete
    
    auto result3 = reassembler.add_frame(std::move(frame3));
    ASSERT_TRUE(result3.has_value()); // Should be complete
    
    EXPECT_EQ(result3->get_text(), "Hello World!");
    EXPECT_EQ(result3->opcode, websocket::Opcode::TEXT);
    EXPECT_TRUE(result3->fin);
}

// ================================================================================================
// Utility Function Tests
// ================================================================================================

TEST(HttpUtilityTest, UrlDecoding) {
    EXPECT_EQ(http::url_decode("hello%20world"), "hello world");
    EXPECT_EQ(http::url_decode("test%2Bvalue"), "test+value");
    EXPECT_EQ(http::url_decode("caf%C3%A9"), "café"); // UTF-8
    EXPECT_EQ(http::url_decode("normal+text"), "normal text"); // + to space
}

TEST(HttpUtilityTest, UrlEncoding) {
    EXPECT_EQ(http::url_encode("hello world"), "hello%20world");
    EXPECT_EQ(http::url_encode("test+value"), "test%2Bvalue");
    EXPECT_EQ(http::url_encode("café"), "caf%C3%A9");
}

TEST(HttpUtilityTest, QueryStringParsing) {
    auto params = http::parse_query_string("?name=John&age=30&city=New%20York");
    
    EXPECT_EQ(params.size(), 3);
    EXPECT_EQ(params["name"], "John");
    EXPECT_EQ(params["age"], "30");
    EXPECT_EQ(params["city"], "New York");
}

TEST(WebSocketUtilityTest, AcceptKeyGeneration) {
    // Test vector from RFC 6455
    std::string client_key = "dGhlIHNhbXBsZSBub25jZQ==";
    std::string expected = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";
    
    std::string accept_key = websocket::generate_accept_key(client_key);
    EXPECT_EQ(accept_key, expected);
}

TEST(WebSocketUtilityTest, KeyValidation) {
    EXPECT_TRUE(websocket::is_valid_websocket_key("dGhlIHNhbXBsZSBub25jZQ=="));
    EXPECT_FALSE(websocket::is_valid_websocket_key("invalid_key"));
    EXPECT_FALSE(websocket::is_valid_websocket_key("too_short=="));
    EXPECT_FALSE(websocket::is_valid_websocket_key("dGhlIHNhbXBsZSBub25jZQ=!")); // Invalid char
}

// ================================================================================================
// Integration Tests
// ================================================================================================

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Could set up test server here
    }
};

TEST_F(IntegrationTest, HttpRequestRoundTrip) {
    // Create request
    http::Request request(http::Method::POST, "/api/test", http::Version{1, 1});
    request.set_header("Host", "example.com");
    request.set_header("Content-Type", "application/json");
    request.set_body("{\"test\":true}");
    
    // Serialize to string
    std::string request_str = request.to_string();
    
    // Parse it back
    http::RequestParser parser;
    std::vector<uint8_t> data(request_str.begin(), request_str.end());
    auto result = parser.parse(data);
    
    ASSERT_TRUE(result.has_value());
    auto [parsed_request, consumed] = *result;
    
    // Verify all fields match
    EXPECT_EQ(parsed_request.method(), request.method());
    EXPECT_EQ(parsed_request.uri(), request.uri());
    EXPECT_EQ(parsed_request.get_header("Host"), request.get_header("Host"));
    EXPECT_EQ(parsed_request.get_header("Content-Type"), request.get_header("Content-Type"));
    EXPECT_EQ(parsed_request.body(), request.body());
}

TEST_F(IntegrationTest, WebSocketFrameRoundTrip) {
    // Create various frame types
    auto text_frame = websocket::FrameParser::create_text_frame("Hello, WebSocket!");
    auto binary_frame = websocket::FrameParser::create_binary_frame(
        std::vector<uint8_t>{0x01, 0x02, 0x03, 0x04});
    auto close_frame = websocket::FrameParser::create_close_frame(
        websocket::CloseCode::NORMAL_CLOSURE, "Goodbye");
    auto ping_frame = websocket::FrameParser::create_ping_frame(
        std::vector<uint8_t>{'p', 'i', 'n', 'g'});
    
    // Test each frame type
    for (auto& frame : {text_frame, binary_frame, close_frame, ping_frame}) {
        // Serialize
        auto serialized = websocket::FrameParser::serialize_frame(frame);
        
        // Parse back
        auto result = websocket::FrameParser::parse_frame(serialized);
        ASSERT_TRUE(result.has_value());
        
        auto [parsed_frame, consumed] = *result;
        
        // Verify fields match
        EXPECT_EQ(parsed_frame.fin, frame.fin);
        EXPECT_EQ(parsed_frame.opcode, frame.opcode);
        EXPECT_EQ(parsed_frame.payload, frame.payload);
        EXPECT_EQ(consumed, serialized.size());
    }
}

// ================================================================================================
// Performance Tests
// ================================================================================================

class PerformanceTest : public ::testing::Test {
protected:
    static constexpr int ITERATIONS = 10000;
};

TEST_F(PerformanceTest, HttpParsingSpeed) {
    std::string request_data = 
        "GET /api/v1/users/123?include=profile,settings HTTP/1.1\r\n"
        "Host: api.example.com\r\n"
        "User-Agent: TestClient/1.0\r\n"
        "Accept: application/json\r\n"
        "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9\r\n"
        "Connection: keep-alive\r\n"
        "\r\n";
    
    std::vector<uint8_t> data(request_data.begin(), request_data.end());
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < ITERATIONS; ++i) {
        http::RequestParser parser;
        auto result = parser.parse(data);
        ASSERT_TRUE(result.has_value());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double requests_per_second = (ITERATIONS * 1000000.0) / duration.count();
    std::cout << "HTTP parsing performance: " << requests_per_second 
              << " requests/second" << std::endl;
    
    // Should be able to parse at least 100K requests per second
    EXPECT_GT(requests_per_second, 100000);
}

TEST_F(PerformanceTest, WebSocketFrameParsingSpeed) {
    auto frame = websocket::FrameParser::create_text_frame("Performance test message");
    auto serialized = websocket::FrameParser::serialize_frame(frame);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < ITERATIONS; ++i) {
        auto result = websocket::FrameParser::parse_frame(serialized);
        ASSERT_TRUE(result.has_value());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double frames_per_second = (ITERATIONS * 1000000.0) / duration.count();
    std::cout << "WebSocket frame parsing performance: " << frames_per_second 
              << " frames/second" << std::endl;
    
    // Should be able to parse at least 500K frames per second
    EXPECT_GT(frames_per_second, 500000);
}

// ================================================================================================
// Main Test Runner
// ================================================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
