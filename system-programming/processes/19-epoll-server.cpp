#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sys/epoll.h>
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

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) { perror("epoll_create1"); return 1; }

    epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    std::cout << "epoll server listening on port 8081...\n";
    while (true) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == server_fd) {
                int client_fd = accept(server_fd, nullptr, nullptr);
                if (client_fd >= 0) {
                    set_nonblocking(client_fd);
                    ev.events = EPOLLIN;
                    ev.data.fd = client_fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
                    std::cout << "Accepted client " << client_fd << std::endl;
                }
            } else {
                char buf[1024];
                ssize_t n = read(events[i].data.fd, buf, sizeof(buf));
                if (n > 0) {
                    write(events[i].data.fd, buf, n); // Echo back
                } else {
                    close(events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                    std::cout << "Closed client " << events[i].data.fd << std::endl;
                }
            }
        }
    }
    close(server_fd);
    close(epoll_fd);
    return 0;
}