#include "SessionManager.hpp"
#include <iostream>
#include <unistd.h>

int main()
{
    http::SessionManager& sm = http::SessionManager::instance();

    /*
     * TEST 1: init()
     *
     * Configure:
     * - cookie name = SESSIONID
     * - timeout = 3 seconds
     */
    std::cout << "===== TEST init() =====" << std::endl;

    sm.init("SESSIONID", 3);

    std::cout << "init() completed" << std::endl;


    /*
     * TEST 2: create_session()
     */
    std::cout << "\n===== TEST create_session() =====" << std::endl;

    std::string id1 = sm.create_session();
    std::string id2 = sm.create_session();

    std::cout << "Session 1: " << id1 << std::endl;
    std::cout << "Session 2: " << id2 << std::endl;

    if (id1 != id2)
        std::cout << "PASS: IDs are different" << std::endl;
    else
        std::cout << "FAIL: IDs are the same" << std::endl;


    /*
     * TEST 3: has_session()
     *
     * Test valid sessions.
     */
    std::cout << "\n===== TEST has_session() =====" << std::endl;

    if (sm.has_session(id1))
        std::cout << "PASS: Session 1 exists" << std::endl;
    else
        std::cout << "FAIL: Session 1 does not exist" << std::endl;

    if (sm.has_session(id2))
        std::cout << "PASS: Session 2 exists" << std::endl;
    else
        std::cout << "FAIL: Session 2 does not exist" << std::endl;


    /*
     * TEST 4: has_session() with invalid ID
     */
    std::cout << "\n===== TEST invalid session =====" << std::endl;

    if (!sm.has_session("does_not_exist"))
        std::cout << "PASS: Fake session does not exist" << std::endl;
    else
        std::cout << "FAIL: Fake session exists" << std::endl;


    /*
     * TEST 5: timeout / expiration
     *
     * timeout = 3 seconds
     */
    std::cout << "\n===== TEST session expiration =====" << std::endl;

    std::string id3 = sm.create_session();

    std::cout << "New session: " << id3 << std::endl;

    if (sm.has_session(id3))
        std::cout << "PASS: Session initially exists" << std::endl;
    else
        std::cout << "FAIL: Session initially does not exist" << std::endl;

    std::cout << "Waiting 4 seconds..." << std::endl;

    sleep(4);

    if (!sm.has_session(id3))
        std::cout << "PASS: Session expired" << std::endl;
    else
        std::cout << "FAIL: Session did not expire" << std::endl;


    /*
     * TEST 6: cleanup()
     */
    std::cout << "\n===== TEST cleanup() =====" << std::endl;

    std::string id4 = sm.create_session();
    std::string id5 = sm.create_session();

    std::cout << "Created two sessions." << std::endl;

    std::cout << "Waiting 4 seconds..." << std::endl;

    sleep(4);

    sm.cleanup();

    std::cout << "cleanup() executed." << std::endl;

    /*
     * After cleanup(), expired sessions should not exist.
     */
    if (!sm.has_session(id4))
        std::cout << "PASS: Session 4 removed" << std::endl;
    else
        std::cout << "FAIL: Session 4 still exists" << std::endl;

    if (!sm.has_session(id5))
        std::cout << "PASS: Session 5 removed" << std::endl;
    else
        std::cout << "FAIL: Session 5 still exists" << std::endl;


    /*
     * TEST 7: create_session() after cleanup()
     */
    std::cout << "\n===== TEST create_session() after cleanup ====="
              << std::endl;

    std::string id6 = sm.create_session();

    if (sm.has_session(id6))
        std::cout << "PASS: New session works" << std::endl;
    else
        std::cout << "FAIL: New session does not work" << std::endl;


    /*
     * Final
     */
    std::cout << "\n===== ALL TESTS FINISHED =====" << std::endl;

    return 0;
}