#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <atomic>
#include <mutex>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>


/*
* ============================================================================
* Configuration
* ============================================================================
*/

struct TestConfig
{
int         workers;
int         test_cases;

std::string host;
int         port;

std::string method;
std::string request_uri;
std::string version;

std::string connection;
std::string cookie;
std::string content_type;

bool        has_body;
bool        body_from_file;

std::string body_file;
size_t      body_size;

TestConfig()
    : workers(1),
        test_cases(1),
        host("127.0.0.1"),
        port(8080),
        method("GET"),
        connection("close"),
        cookie(),
        content_type(),
        has_body(false),
        body_from_file(false),
        body_file(),
        body_size(0)
{
}
};


/*
* ============================================================================
* Test case / result
* ============================================================================
*/

struct TestCase
{
std::string request;
};

struct TestResult
{
bool        success;
int         status_code;
size_t      bytes_sent;
size_t      bytes_received;

TestResult()
    : success(false),
        status_code(0),
        bytes_sent(0),
        bytes_received(0)
{
}
};


/*
* ============================================================================
* Statistics
* ============================================================================
*/

struct TestStatistics
{
size_t total;
size_t successful;
size_t failed;
size_t bytes_sent;
size_t bytes_received;

TestStatistics()
    : total(0),
        successful(0),
        failed(0),
        bytes_sent(0),
        bytes_received(0)
{
}

void add(const TestResult& result)
{
    ++total;

    if (result.success)
        ++successful;
    else
        ++failed;

    bytes_sent += result.bytes_sent;
    bytes_received += result.bytes_received;
}
};


/*
* ============================================================================
* Input helpers
* ============================================================================
*/

static int ask_int(const std::string& prompt)
{
int value;

while (true)
{
    std::cout << prompt;

    if (std::cin >> value && value > 0)
        return value;

    std::cin.clear();
    std::cin.ignore(10000, '\n');

    std::cout << "Please enter a positive number.\n";
}
}

static bool ask_yes_no(const std::string& prompt)
{
char answer;

while (true)
{
    std::cout << prompt << " [y/n]: ";
    std::cin >> answer;

    if (answer == 'y' || answer == 'Y')
        return true;

    if (answer == 'n' || answer == 'N')
        return false;

    std::cout << "Please enter y or n.\n";
}
}

static std::string ask_string(const std::string& prompt)
{
std::string value;

while (true)
{
    std::cout << prompt;
    std::cin >> value;

    if (!value.empty())
        return value;

    std::cout << "Value cannot be empty.\n";
}
}

static std::string ask_method()
{
std::string method;

while (true)
{
    std::cout
        << "Request method "
        << "(GET/POST/PUT/DELETE/PATCH): ";

    std::cin >> method;

    if (method == "GET" ||
        method == "POST" ||
        method == "PUT" ||
        method == "DELETE" ||
        method == "PATCH")
    {
        return method;
    }

    std::cout << "Unsupported method.\n";
}
}

static std::string ask_request_uri() {
    
    while (true) {
        std::cout << "Request URI: ";
        std::string uri;
        std::cin >> uri;
        if (!uri.empty())  {
            return uri;
        }
    }
    
    return "";
}


/*
* ============================================================================
* Body generation
* ============================================================================
*/

static std::string read_file(const std::string& path)
{
std::ifstream file(
    path.c_str(),
    std::ios::in | std::ios::binary
);

if (!file)
{
    std::cerr
        << "Cannot open body file: "
        << path
        << '\n';

    return "";
}

std::ostringstream buffer;

buffer << file.rdbuf();

return buffer.str();
}

static std::string generate_random_body(size_t size) {
    
static const char chars[] =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";

std::string body;

body.reserve(size);

for (size_t i = 0; i < size; ++i)
{
    body += chars[
        std::rand() % (sizeof(chars) - 1)
    ];
}

return body;
}


/*
* ============================================================================
* Configuration
* ============================================================================
*/

static void configure_body(TestConfig& config)
{
config.has_body = false;
config.body_from_file = false;
config.body_file.clear();
config.body_size = 0;

if (config.method == "GET" ||
    config.method == "DELETE")
{
    return;
}

config.has_body =
    ask_yes_no("Include request body?");

if (!config.has_body)
    return;

std::cout << "\n";
std::cout << "Body source:\n";
std::cout << "  1. File\n";
std::cout << "  2. Generate random body\n";

int choice;

while (true)
{
    std::cout << "Choice: ";
    std::cin >> choice;

    if (choice == 1 || choice == 2)
        break;

    std::cout << "Please choose 1 or 2.\n";
}

if (choice == 1)
{
    config.body_from_file = true;

    config.body_file =
        ask_string("Body file: ");
}
else
{
    config.body_from_file = false;

    config.body_size =
        static_cast<size_t>(
            ask_int("Random body size (bytes): ")
        );
}
}

static void configure_headers(TestConfig& config)
{
std::cout << "\n";
std::cout << "==============================\n";
std::cout << "       SPECIAL HEADERS        \n";
std::cout << "==============================\n";

config.host =
    ask_string("Host: ");

while (true)
{
    config.connection =
        ask_string(
            "Connection (keep-alive/close): "
        );

    if (config.connection == "keep-alive" ||
        config.connection == "close")
    {
        break;
    }

    std::cout
        << "Must be 'keep-alive' or 'close'.\n";
}

if (ask_yes_no("Include Cookie header?"))
{
    config.cookie =
        ask_string("Cookie: ");
}

if (config.has_body)
{
    std::cout << "\n";
    std::cout << "Content-Type:\n";
    std::cout
        << "  1. application/x-www-form-urlencoded\n";
    std::cout
        << "  2. application/json\n";
    std::cout
        << "  3. text/plain\n";
    std::cout
        << "  4. application/octet-stream\n";

    int choice;

    while (true)
    {
        std::cout << "Choice: ";
        std::cin >> choice;

        if (choice >= 1 && choice <= 4)
            break;

        std::cout << "Please choose 1-4.\n";
    }

    if (choice == 1)
    {
        config.content_type =
            "application/x-www-form-urlencoded";
    }
    else if (choice == 2)
    {
        config.content_type =
            "application/json";
    }
    else if (choice == 3)
    {
        config.content_type =
            "text/plain";
    }
    else
    {
        config.content_type =
            "application/octet-stream";
    }
}
}

static void configure(TestConfig& config)
{
std::cout
    << "==============================\n"
    << "       HTTP TEST SUITE        \n"
    << "==============================\n\n";

config.workers =
    ask_int("Number of workers: ");

config.test_cases =
    ask_int("Number of test cases: ");

config.host =
    ask_string("Server host [127.0.0.1]: ");

config.port =
    ask_int("Server port: ");

config.method =
    ask_method();
config.request_uri = ask_request_uri();

configure_body(config);
configure_headers(config);
}


/*
* ============================================================================
* HTTP request generation
* ============================================================================
*/

static std::string generate_body(
const TestConfig& config)
{
if (!config.has_body)
    return "";

if (config.body_from_file)
    return read_file(config.body_file);

return generate_random_body(
    config.body_size
);
}

static std::string generate_request(
const TestConfig& config)
{
std::string body =
    generate_body(config);

std::ostringstream request;

request
    << config.method
    << " "
    << config.request_uri
    << " HTTP/1.1\r\n";

request
    << "Host: "
    << config.host
    << "\r\n";

request
    << "Connection: "
    << config.connection
    << "\r\n";

if (!config.cookie.empty())
{
    request
        << "Cookie: "
        << config.cookie
        << "\r\n";
}

if (config.has_body)
{
    request
        << "Content-Type: "
        << config.content_type
        << "\r\n";

    request
        << "Content-Length: "
        << body.size()
        << "\r\n";
}

request << "\r\n";

if (config.has_body)
    request << body;

return request.str();
}


/*
* ============================================================================
* Test generation
* ============================================================================
*/

static std::vector<TestCase>
generate_tests(const TestConfig& config)
{
std::vector<TestCase> tests;

tests.reserve(
    static_cast<size_t>(config.test_cases)
);

for (int i = 0;
        i < config.test_cases;
        ++i)
{
    TestCase test;

    test.request =
        generate_request(config);

    tests.push_back(test);
}

return tests;
}


/*
* ============================================================================
* HTTP client
* ============================================================================
*/

static bool parse_status_code(
const std::string& response,
int& status_code)
{
/*
    * Expected:
    *
    * HTTP/1.1 200 OK
    */
if (response.size() < 12)
    return false;

if (response.compare(0, 5, "HTTP/") != 0)
    return false;

size_t space = response.find(' ');

if (space == std::string::npos)
    return false;

if (space + 4 > response.size())
    return false;

status_code =
    std::atoi(
        response.substr(
            space + 1,
            3
        ).c_str()
    );

return status_code > 0;
}

static TestResult run_test(
const TestConfig& config,
const TestCase& test)
{
TestResult result;

/*
    * Resolve host.
    */
struct addrinfo hints;
struct addrinfo* address;

std::memset(&hints, 0, sizeof(hints));

hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;

std::ostringstream port_stream;
port_stream << config.port;

if (getaddrinfo(
        config.host.c_str(),
        port_stream.str().c_str(),
        &hints,
        &address) != 0)
{
    return result;
}

/*
    * Create socket.
    */
int fd =
    socket(
        address->ai_family,
        address->ai_socktype,
        address->ai_protocol
    );

if (fd < 0)
{
    freeaddrinfo(address);
    return result;
}

/*
    * Connect.
    */
if (connect(
        fd,
        address->ai_addr,
        address->ai_addrlen) < 0)
{
    close(fd);
    freeaddrinfo(address);
    return result;
}

freeaddrinfo(address);

/*
    * Send request.
    */
const char* data = test.request.data();
size_t remaining = test.request.size();

while (remaining > 0)
{
    ssize_t sent =
        send(
            fd,
            data,
            remaining,
            0
        );

    if (sent <= 0)
    {
        close(fd);
        return result;
    }

    result.bytes_sent +=
        static_cast<size_t>(sent);

    data += sent;
    remaining -=
        static_cast<size_t>(sent);
}

/*
    * Receive response.
    */
char buffer[8192];

std::string response;

while (true)
{
    ssize_t received =
        recv(
            fd,
            buffer,
            sizeof(buffer),
            0
        );

    if (received == 0)
        break;

    if (received < 0)
    {
        if (errno == EINTR)
            continue;

        break;
    }

    result.bytes_received +=
        static_cast<size_t>(received);

    response.append(
        buffer,
        static_cast<size_t>(received)
    );

    /*
        * For now we only need the HTTP status.
        *
        * Once we have parsed it, we don't need
        * to read the entire response.
        */
    if (response.find("\r\n") !=
        std::string::npos)
    {
        break;
    }
}

close(fd);

/*
    * Validate response.
    */
if (!parse_status_code(
        response,
        result.status_code))
{
    return result;
}

/*
    * Consider 2xx and 3xx successful.
    */
if (result.status_code >= 200 &&
    result.status_code < 400)
{
    result.success = true;
}

return result;
}


/*
* ============================================================================
* Test suite
* ============================================================================
*/

class TestSuite
{
private:

TestConfig config_;

std::vector<TestCase> tests_;

std::atomic<size_t> next_test_;

TestStatistics statistics_;

std::mutex statistics_mutex_;

public:

TestSuite()
    : config_(),
        tests_(),
        next_test_(0),
        statistics_(),
        statistics_mutex_()
{
}

void configure()
{
    ::configure(config_);
}

void generate()
{
    std::cout
        << "\nGenerating test cases...\n";

    tests_ =
        generate_tests(config_);

    std::cout
        << "Generated "
        << tests_.size()
        << " test cases.\n";
}

void worker()
{
    while (true)
    {
        /*
            * Atomically claim the next test.
            */
        size_t index =
            next_test_.fetch_add(1);

        if (index >= tests_.size())
            break;

        TestResult result =
            run_test(
                config_,
                tests_[index]
            );

        /*
            * Statistics are shared between
            * all workers.
            */
        {
            std::lock_guard<std::mutex>
                lock(statistics_mutex_);

            statistics_.add(result);
        }
    }
}

void run()
{
    std::cout
        << "\nRunning "
        << tests_.size()
        << " tests using "
        << config_.workers
        << " workers...\n";

    std::vector<std::thread> workers;

    workers.reserve(
        static_cast<size_t>(config_.workers)
    );

    for (int i = 0;
            i < config_.workers;
            ++i)
    {
        workers.push_back(
            std::thread(
                &TestSuite::worker,
                this
            )
        );
    }

    for (size_t i = 0;
            i < workers.size();
            ++i)
    {
        workers[i].join();
    }
}

void print_results() const
{
    std::cout
        << "\n"
        << "==============================\n"
        << "          RESULTS             \n"
        << "==============================\n";

    std::cout
        << "Total:           "
        << statistics_.total
        << '\n';

    std::cout
        << "Successful:      "
        << statistics_.successful
        << '\n';

    std::cout
        << "Failed:          "
        << statistics_.failed
        << '\n';

    std::cout
        << "Bytes sent:      "
        << statistics_.bytes_sent
        << '\n';

    std::cout
        << "Bytes received:  "
        << statistics_.bytes_received
        << '\n';

    std::cout
        << "==============================\n";
}
};


/*
* ============================================================================
* Main
* ============================================================================
*/

int main()
{
    std::srand(
        static_cast<unsigned int>(
            std::time(NULL)
        )
    );

    TestSuite suite;

    suite.configure();

    suite.generate();

    suite.run();

    suite.print_results();

    return 0;
}