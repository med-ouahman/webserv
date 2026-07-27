#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cctype>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

static size_t parse_size(const std::string& s)
{
    if (s.empty())
        return 0;

    size_t i = 0;
    size_t value = 0;

    while (i < s.size() && std::isdigit(s[i]))
    {
        value = value * 10 + (s[i] - '0');
        i++;
    }

    size_t multiplier = 1;

    if (i < s.size())
    {
        char suffix = std::toupper(s[i]);

        if (suffix == 'K')
            multiplier = 1024;
        else if (suffix == 'M')
            multiplier = 1024 * 1024;
        else if (suffix == 'G')
            multiplier = 1024ULL * 1024ULL * 1024ULL;
        else if (suffix == 'B')
            multiplier = 1;
        else
        {
            std::cerr << "Invalid size suffix\n";
            return 0;
        }

        i++;
    }

    if (i != s.size())
    {
        std::cerr << "Invalid size format\n";
        return 0;
    }

    return value * multiplier;
}


static bool send_all(int fd, const char* data, size_t size)
{
    size_t sent = 0;

    while (sent < size)
    {
        ssize_t n = send(fd, data + sent, size - sent, 0);
        sent += n;
    }

    return true;
}

int main(int argc, char** argv)
{
    const char* host = "127.0.0.1";
    int port = 8080;

    // Send 100 MB
    size_t body_size = parse_size(argv[1] == NULL ? "": argv[1]);


    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        return 1;
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        close(fd);
        return 1;
    }

    std::string header =
        "POST /directory/youpi.bla HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/octet-stream\r\n"
        "Content-Length: " + std::to_string(body_size) + "\r\n"
        "\r\n";

    if (!send_all(fd, header.c_str(), header.size()))
    {
        std::cerr << "failed sending headers\n";
        close(fd);
        return 1;
    }

    // Reusable 8 KB buffer
    char buffer[1024*64];
    std::memset(buffer, 'A', sizeof(buffer));

    size_t remaining = body_size;

    while (remaining > 0)
    {
        size_t chunk = remaining < sizeof(buffer)
            ? remaining
            : sizeof(buffer);

        send_all(fd, buffer, chunk);

        remaining -= chunk;

        std::cout << "Sent "
                  << (body_size - remaining) / (1024 * 1024)
                  << " MB\n";
    }

    std::cout << "\nFinished sending\n";

    return 0;
}