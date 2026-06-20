#pragma once

struct TCPSocket
{
    int fd = -1;

    explicit TCPSocket(const char* host, uint16_t port)
    {
        fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) return;

        struct timeval tv{ .tv_sec = 3, .tv_usec = 0 };
        ::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);

        struct sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(static_cast<uint16_t>(port));
        addr.sin_addr.s_addr = ::inet_addr(host);

        if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof addr) < 0) {
            ::close(fd);
            fd = -1;
        }
    }

    ~TCPSocket() { if (fd >= 0) ::close(fd); }

    bool good() const { return fd >= 0; }

    bool send_all(const std::string& data)
    {
        size_t sent = 0;
        while (sent < data.size()) {
            auto n = ::send(fd, data.data() + sent, data.size() - sent, 0);
            if (n <= 0) return false;
            sent += static_cast<size_t>(n);
        }
        return true;
    }

    std::string recv_all()
    {
        std::string result;
        char buf[4096];
        ssize_t n;
        while ((n = ::recv(fd, buf, sizeof buf, 0)) > 0)
            result.append(buf, static_cast<size_t>(n));
        return result;
    }

    // Non-copyable
    TCPSocket(const TCPSocket&)            = delete;
    TCPSocket& operator=(const TCPSocket&) = delete;
};

