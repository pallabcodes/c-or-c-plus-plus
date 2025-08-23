#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

// Demonstrates System V message queue IPC

struct msgbuf {
    long mtype;
    char mtext[100];
};

int main() {
    key_t key = ftok("file", 65); // Use an existing file for key
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid < 0) { perror("msgget"); return 1; }

    pid_t pid = fork();
    if (pid == 0) {
        // Child: send message
        msgbuf msg;
        msg.mtype = 1;
        strcpy(msg.mtext, "Hello from child via msgqueue!");
        msgsnd(msgid, &msg, sizeof(msg.mtext), 0);
        _exit(0);
    } else if (pid > 0) {
        // Parent: receive message
        msgbuf msg;
        msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);
        std::cout << "Parent received: " << msg.mtext << std::endl;
        waitpid(pid, nullptr, 0);
        msgctl(msgid, IPC_RMID, nullptr); // Remove queue
    } else {
        perror("fork");
        return 1;
    }
    return 0;
}