#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main () {

    // last value i.e. an octal value = 0755 is file permission

    // Each of these represents my permission to a specific user group

    // first = 0 (it is a special user group that indicates specific permissions)
    // second = 7 (indicates to a user permission that's more associated with the owner of the file)
    // third = 5 (indicates the group permission)
    // fourth = 5 (indicates the other users permission) 

    // specific permission: 1 bit
    // second = 3 bits (owner permission)
    // third = 3 bits (group permission)
    // fourth = 3 bits (other users permission)

    // so it can be 124, 145, 154, 415, 451, 514, etc.

    // create modes:
    // 1. rwxr-xr-x (0755)
    // 2. rw-r--r-- (0644)
    // 3. rwxrwxrwx (0777)

    // READ: 4
    // WRITE: 2
    // EXECUTE: 1

    // first = 0 (special user group) 1 bit
    // second = 7 (owner permission) 3 bits
    // third = 5 (group permission) 3 bits
    // fourth = 5 (other users permission) 3 bits
    // total = 1 + 3 + 3 + 3 = 10 bits

    // Let's say we needed to give a certain user a read and write permission for a file

    // 6 = RW (read and write i.e. 4 + 2)
    // 7 = RWX (read, write and execute i.e. 4 + 2 + 1)
    // 2 = W (write only i.e. 2)

    // open("hello.txt", O_RDWR, 0755)

    // # file modes (show different permission types when opening a file)
    // 1. rwxr-xr-x (0755)
    // 2. rw-r--r-- (0644)
    // 3. rwxrwxrwx (0777)

    // man open / man 2 open

    int file;

    // pathname, flags, mode
    // O_RDONLY: Open for reading only
    // O_WRONLY: Open for writing only
    // O_RDWR: Open for reading and writing
    // O_CREAT: Create file if it does not exist
    // O_EXCL: Ensure that this call creates the file, and fails if it already exists
    // O_TRUNC: Truncate file to zero length if it already exists
    // O_APPEND: Append data to the end of the file
    // O_NONBLOCK: Open in non-blocking mode
    // O_SYNC: Open for synchronous I/O
    // O_DSYNC: Open for data integrity I/O
    // O_RSYNC: Open for read integrity I/O
    // O_NOCTTY: Do not assign the file as the controlling terminal for the process
    // O_NOFOLLOW: Do not follow symbolic links
    // O_DIRECTORY: Fail if the file is not a directory
    // O_CLOEXEC: Close the file descriptor on exec
    // O_ASYNC: Enable signal-driven I/O
    // O_DIRECT: Minimize cache effects of I/O to the file
    // O_LARGEFILE: Allow large files to be opened
    // O_NDELAY: Open in non-blocking mode (same as O_NONBLOCK)
    // O_BINARY: Open in binary mode (not applicable on POSIX systems)
    // O_TEXT: Open in text mode (not applicable on POSIX systems)

    // Open file with read, write and execute permissions for owner, and read and execute for group and others
    // to check whether 0755 did what we wanted, just run the command: ls -l hello.txt
    // but if used e.g. 0000, it would mean no permissions for anyone 
    // or if used 0700, it would mean only the owner has read, write and execute permissions
    // for permission calculation, unix permission calculator in google for any adeaute sites
    file = open("hello.txt", O_RDWR | O_CREAT, 0755);

    if (file < 0) {
        perror("Error opening the file\n");
        exit(1);
    }

    int x;

    // close the file descriptor and save the return value and if successful, it will return 0
    // if not successful, it will return -1 and set errno to indicate the error
    x = close(file);

    if (x < 0) {
        perror("Error closing the file\n");
        exit(1);
    }

    int y;

    y = remove("hello.txt");

    if (y < 0) {
        perror("Error deleting the file\n");
        exit(1);
    }

    return 0;
}