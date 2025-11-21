# File Operations

## Scope
Applies to file I/O, memory mapped I/O, asynchronous I/O, high performance file operations, and file system interfaces.

## File I/O Basics

### open() and close()
* Always check open() return value
* Use appropriate flags (O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_TRUNC)
* Set file permissions with mode_t (S_IRUSR, S_IWUSR, etc.)
* Always close file descriptors with close()
* Check close() return value in critical code

### read() and write()
* Handle partial reads and writes
* Check return values (may read/write less than requested)
* Loop until all data is read/written
* Handle EINTR errors (interrupted system calls)
* Use pread() and pwrite() for atomic positioned I/O

### File Positioning
* Use lseek() for file positioning
* Understand SEEK_SET, SEEK_CUR, SEEK_END
* Check lseek() return value
* Document file positioning requirements

## Memory Mapped I/O

### mmap() for Files
* Use mmap() for large file access
* MAP_SHARED for shared modifications
* MAP_PRIVATE for copy on write
* Check return value against MAP_FAILED
* Unmap with munmap() before close

### Advantages
* Direct memory access to file data
* Efficient for large files
* Shared memory between processes
* Kernel handles page caching

### Considerations
* File must be opened before mmap()
* Mapping size should match file size
* Handle file truncation and extension
* Document mapping ownership

## Asynchronous I/O

### Linux AIO (libaio)
* Use io_setup(), io_submit(), io_getevents()
* Non blocking I/O operations
* Efficient for high throughput I/O
* Handle AIO completion appropriately

### POSIX AIO
* Use aio_read(), aio_write(), aio_suspend()
* More portable than Linux AIO
* Handle aio_error() and aio_return()
* Clean up AIO control blocks

### io_uring (Linux 5.1+)
* Modern high performance I/O interface
* Zero copy operations
* Batch I/O operations
* Very high throughput

## High Performance Patterns

### Zero Copy I/O
* Use sendfile() for file to socket transfers
* Use splice() and vmsplice() for zero copy
* Minimize data copying
* Document zero copy usage

### Direct I/O
* Use O_DIRECT flag for direct I/O
* Bypass page cache
* Requires aligned buffers and sizes
* Use only when page cache is not beneficial

### Non Blocking I/O
* Use O_NONBLOCK flag
* Handle EAGAIN/EWOULDBLOCK errors
* Use select(), poll(), epoll() with non blocking I/O
* Document non blocking I/O usage

## File Locking

### Advisory Locks (flock, fcntl)
* Use flock() for whole file locking
* Use fcntl() F_SETLK/F_SETLKW for record locking
* Handle lock conflicts appropriately
* Release locks before close (or use close on exec)

### Lock Patterns
* Shared locks for reading
* Exclusive locks for writing
* Avoid deadlocks with lock ordering
* Use timeout locks (F_SETLKW) when appropriate

## File System Operations

### Directory Operations
* Use opendir(), readdir(), closedir()
* Handle directory traversal errors
* Use rewinddir() to reset directory stream
* Document directory operation patterns

### File Metadata
* Use stat(), fstat(), lstat() for file information
* Check return values
* Understand struct stat fields
* Handle symbolic links appropriately (lstat vs stat)

### File Permissions
* Use chmod() to change permissions
* Use umask() to set default permissions
* Check permissions with access() or faccessat()
* Document permission requirements

## Implementation Standards

### Error Handling
* Check all file operation return values
* Use perror() or strerror() with context
* Handle EINTR for interrupted system calls
* Map errno to clear error messages

### Resource Management
* Close file descriptors in all code paths
* Unmap memory mappings before close
* Clean up AIO control blocks
* Document file descriptor ownership

### Performance
* Profile I/O operations
* Choose appropriate I/O model (blocking, non blocking, async)
* Optimize buffer sizes
* Document performance characteristics

## Code Examples

### Safe File Reading
```cpp
// Thread-safety: Not thread-safe (file descriptor)
// Ownership: Borrows fd, caller owns file descriptor
// Invariants: fd valid, buf not null, size > 0
// Failure modes: Partial reads, EINTR, EOF
ssize_t safe_read(int fd, void* buf, size_t size) {
    ssize_t total = 0;
    while (total < static_cast<ssize_t>(size)) {
        ssize_t n = read(fd, 
                         static_cast<char*>(buf) + total, 
                         size - total);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) break;  // EOF
        total += n;
    }
    return total;
}
```

### Memory Mapped File
```cpp
// Thread-safety: Thread-safe for concurrent reads
// Ownership: Caller owns mapping, must munmap
// Invariants: fd valid, size > 0
// Failure modes: Returns nullptr on error
void* map_file_readonly(int fd, size_t size) {
    void* addr = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap failed");
        return nullptr;
    }
    return addr;
}
```

## Testing Requirements
* Test file read and write operations
* Test partial read/write handling
* Test memory mapped I/O
* Test asynchronous I/O
* Test file locking
* Test error handling for all operations
* Verify file descriptor cleanup

## Related Topics
* Process Management: File backed memory mappings
* Network Programming: File to socket transfers (sendfile)
* Platform-Specific: Platform-specific file I/O APIs
* Performance Optimization: I/O performance profiling
* Synchronization: File locking and concurrent access

