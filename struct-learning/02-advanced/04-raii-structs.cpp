/*
 * =============================================================================
 * Advanced Struct Techniques: RAII with Structs
 * Production grade resource acquisition and release patterns
 * =============================================================================
 */

#include <iostream>
#include <cstdio>
#include <cstring>
#include <stdexcept>

// FileHandle: RAII wrapper around FILE*
struct FileHandle {
    FILE* fp;

    explicit FileHandle(const char* path, const char* mode) : fp(std::fopen(path, mode)) {
        if (!fp) throw std::runtime_error("failed to open file");
    }

    ~FileHandle() {
        if (fp) std::fclose(fp);
    }

    // non copyable
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    // movable
    FileHandle(FileHandle&& other) noexcept : fp(other.fp) { other.fp = nullptr; }
    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this != &other) {
            if (fp) std::fclose(fp);
            fp = other.fp;
            other.fp = nullptr;
        }
        return *this;
    }
};

// Buffer: RAII for heap memory
struct Buffer {
    char* data;
    size_t size;

    explicit Buffer(size_t n) : data(static_cast<char*>(std::malloc(n))), size(n) {
        if (!data) throw std::bad_alloc();
        std::memset(data, 0, n);
    }

    ~Buffer() {
        std::free(data);
    }

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr; other.size = 0;
    }
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            std::free(data);
            data = other.data; size = other.size;
            other.data = nullptr; other.size = 0;
        }
        return *this;
    }
};

void demo_file_and_buffer() {
    std::cout << "\n=== RAII: FILE AND BUFFER ===" << std::endl;
    // path kept local to avoid accidental writes
    const char* path = "./_raii_demo.tmp";
    {
        FileHandle fh(path, "wb");
        Buffer buf(64);
        std::strcpy(buf.data, "hello raii");
        std::fwrite(buf.data, 1, std::strlen(buf.data), fh.fp);
    } // resources released automatically
    // cleanup best done via OS temp dirs in production
    std::remove(path);
}

int main() {
    try {
        demo_file_and_buffer();
        std::cout << "\n=== RAII STRUCTS COMPLETED SUCCESSFULLY ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl; return 1;
    }
    return 0;
}
