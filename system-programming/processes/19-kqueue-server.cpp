#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sys/event.h>
#include <fcntl.h>

#define MAX_EVENTS 10

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8081);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); return 1; }
    listen(server_fd, SOMAXCONN);
    set_nonblocking(server_fd);

    int kq = kqueue();
    if (kq < 0) { perror("kqueue"); return 1; }

    struct kevent change;
    EV_SET(&change, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(kq, &change, 1, NULL, 0, NULL);

    std::cout << "kqueue server listening on port 8081...\n";
    while (true) {
        struct kevent events[MAX_EVENTS];
        int nev = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        for (int i = 0; i < nev; ++i) {
            if (events[i].ident == server_fd) {
                int client_fd = accept(server_fd, nullptr, nullptr);
                if (client_fd >= 0) {
                    set_nonblocking(client_fd);
                    struct kevent client_event;
                    EV_SET(&client_event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                    kevent(kq, &client_event, 1, NULL, 0, NULL);
                    std::cout << "Accepted client " << client_fd << std::endl;
                }
            } else {
                char buf[1024];
                ssize_t n = read(events[i].ident, buf, sizeof(buf));
                if (n > 0) {
                    write(events[i].ident, buf, n); // Echo back
                } else {
                    close(events[i].ident);
                    struct kevent del_event;
                    EV_SET(&del_event, events[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(kq, &del_event, 1, NULL, 0, NULL);
                    std::cout << "Closed client " << (int)events[i].ident << std::endl;
                }
            }
        }
    }
    close(server_fd);
    close(kq);
    return 0;
}