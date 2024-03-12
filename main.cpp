#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0 /* IP */);

    int opts = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opts,
                   sizeof(opts)) < 0) {
        std::cout << "Failed to set opts: " << strerror(errno) << std::endl;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8000);
    if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr))) {
        std::cout << "Failed to bind: " << strerror(errno) << std::endl;
        exit(1);
    }

    if (listen(socket_fd, 3)) {
        std::cout << "Failed to listen: " << strerror(errno) << std::endl;
        exit(1);
    }

    sockaddr_in addr_c;
    socklen_t addr_c_len = sizeof(addr_c);
    int socket_c_fd;
    if ((socket_c_fd =
             accept(socket_fd, (struct sockaddr *)&addr, &addr_c_len)) < 0) {
        std::cout << "Failed to accept: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::cout << "Client Port:" << addr_c.sin_port << std::endl;

    int buf_len = 1024;
    char buf[buf_len];
    // read(socket_c_fd, buf, buf_len);
    recv(socket_c_fd, buf, buf_len, MSG_DONTWAIT);
    std::cout << buf << std::endl;

    char msg[] = "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\nHello World!";
    // write(socket_c_fd, msg, strlen(msg));
    std::cout << "sending" << std::endl;
    send(socket_c_fd, msg, strlen(msg), 0);
    std::cout << "sent" << std::endl;

    // recv(socket_c_fd, buf, buf_len, MSG_CMSG_CLOEXEC);

    std::cout << "closing" << std::endl;

    if (close(socket_fd) < 0) {
        std::cout << "Failed to close: " << strerror(errno) << std::endl;
        exit(1);
    }

    return 0;
}
