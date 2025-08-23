#include <iostream>
#include <unistd.h>
#include <fcntl.h>

// Demonstrates dup and dup2 for redirecting output

int main() {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return 1; }

    int saved_stdout = dup(STDOUT_FILENO); // Save original stdout
    dup2(fd, STDOUT_FILENO); // Redirect stdout to file

    std::cout << "This goes to output.txt via dup2!" << std::endl;

    dup2(saved_stdout, STDOUT_FILENO); // Restore stdout
    close(fd);
    close(saved_stdout);

    std::cout << "This goes to the terminal again!" << std::endl;
    return 0;
}