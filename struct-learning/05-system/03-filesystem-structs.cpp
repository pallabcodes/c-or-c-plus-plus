/*
 * =============================================================================
 * System Programming: File System Structs
 * Inode and directory entry examples with simple traversal
 * =============================================================================
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <array>

struct Inode {
    uint32_t id;
    uint16_t mode;      // permission bits
    uint16_t links;
    uint32_t owner;
    uint32_t group;
    uint64_t size;
    uint64_t blocks[4]; // simple direct blocks
};

struct DirEntry {
    uint32_t inode_id;
    char name[32];
};

struct DirPage {
    std::array<DirEntry, 8> entries;
};

void demo_fs() {
    std::cout << "\n=== SYSTEM: FILE SYSTEM STRUCTS ===" << std::endl;
    Inode i{1001u, 0644u, 1u, 1000u, 1000u, 4096u, {1,2,3,4}};
    std::cout << "inode id=" << i.id << " size=" << i.size << std::endl;

    DirPage page{};
    page.entries[0] = DirEntry{1001u, "file.txt"};
    page.entries[1] = DirEntry{1002u, "notes.md"};

    for (const auto& e : page.entries) {
        if (e.inode_id == 0) continue;
        std::cout << e.inode_id << " " << e.name << std::endl;
    }
}

int main() {
    try { demo_fs(); std::cout << "\n=== FILE SYSTEM STRUCTS COMPLETED SUCCESSFULLY ===" << std::endl; }
    catch (...) { std::cerr << "error" << std::endl; return 1; }
    return 0;
}
