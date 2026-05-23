#include <vector>
#include <string>
#include <cstring>

class CStringArray {

private:
    std::vector<std::string> storage;
    mutable std::vector<char*> cache;
    mutable bool dirty;

    void rebuild() const {

        cache.clear();
        cache.reserve(storage.size() + 1);

        for (size_t i = 0; i < storage.size(); ++i)
            cache.push_back(const_cast<char*>(storage[i].c_str()));

        cache.push_back(NULL);
        dirty = false;
    }

public:
    CStringArray() : dirty(false) {}

    void push(const std::string& s) {
        storage.push_back(s);
        dirty = true;
    }

    void push(const char* s) {
        storage.push_back(s);
        dirty = true;
    }

    size_t size() const {
        return storage.size();
    }

    const std::vector<std::string>& data() const {
        return storage;
    }

    char** argv() const {
        if (dirty || cache.empty())
            rebuild();

        return cache.data();
    }
};
