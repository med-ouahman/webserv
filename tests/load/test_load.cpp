#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <mutex>

static std::mutex log_mutex;

static void log_msg(int id, const std::string& msg)
{
    std::lock_guard<std::mutex> lock(log_mutex);
    std::cout << "[Client " << id << "] " << msg << std::endl;
}

static std::string random_string(size_t len)
{
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    static thread_local std::mt19937 rng(std::random_device{}());
    static thread_local std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string s;
    s.reserve(len);
    for (size_t i = 0; i < len; i++)
        s += charset[dist(rng)];
    return s;
}

static void set_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void client_worker(const std::string& host, int port, int id)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

    set_non_blocking(sock);

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> size_dist(10, 200);
    std::uniform_int_distribution<> sleep_dist(1, 20);

    char buffer[4096];

    while (true)
    {
        // send random payload
        std::string msg = random_string(size_dist(rng));

        if (send(sock, msg.c_str(), msg.size(), 0) <= 0) {
            log_msg(id, "send failed");
            break;
        }

        // 🔥 RECEIVE + LOG HERE
        ssize_t r = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (r > 0)
        {
            buffer[r] = '\0'; // safe print

            log_msg(id,
                "received " + std::to_string(r) + " bytes: " +
                std::string(buffer, r)
            );
        }
        else if (r == 0)
        {
            log_msg(id, "server closed connection");
            break;
        }
        else
        {
            // non-blocking case: no data available
            if (errno != EAGAIN && errno != EWOULDBLOCK)
                log_msg(id, "recv error: " + std::string(strerror(errno)));
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(sleep_dist(rng))
        );
    }

    close(sock);
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        std::cout << "Usage: ./load_test <ip> <port> <num_clients>\n";
        return 1;
    }

    std::string ip = argv[1];
    int port = std::stoi(argv[2]);
    int clients = std::stoi(argv[3]);

    std::vector<std::thread> threads;

    std::cout << "Starting " << clients << " clients...\n";

    for (int i = 0; i < clients; i++)
        threads.emplace_back(client_worker, ip, port, i);

    for (auto& t : threads)
        t.join();
}
