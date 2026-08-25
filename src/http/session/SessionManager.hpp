#pragma once

#include <map>
#include <string>
#include <ctime>
#include <cstddef>

namespace http {

class SessionManager {
private:
	struct SessionData;
	typedef std::map<std::string, SessionData> SessionStore;
public:

	static SessionManager& instance();

	// Initialization
	void init(const std::string& cookie_name, std::size_t timeout_seconds);
	bool is_initialized() const;

	// Session lifecycle
	std::string create_session();
	bool has_session(const std::string& id) const;
	void touch_session(const std::string& id);
	void delete_session(const std::string& id);

	// Session data storage
	void set_session_data(const std::string& id,
	                       const std::string& key,
	                       const std::string& value);
	std::string get_session_data(const std::string& id,
	                              const std::string& key) const;
	bool has_session_data(const std::string& id,
	                       const std::string& key) const;

	// Maintenance
	void cleanup();
	std::size_t get_session_count() const;

	// Config access
	const std::string& get_cookie_name() const;

private:
	SessionManager();
	~SessionManager();

	// Not implemented: singleton must never be copied.
	// Declared private with no definition (C++98-style noncopyable idiom).
	SessionManager(const SessionManager& other);
	SessionManager& operator=(const SessionManager& other);

	std::string generate_session_id();
	bool is_expired(std::time_t last_touch, std::time_t now) const;

	struct SessionData {
		std::time_t                        creation_time;
		std::time_t                        last_touch;
		std::map<std::string, std::string> data;
	};

	/* Global config */
	std::string cookie_name_;
	size_t		timeout_seconds_;
	bool		initialized_;
	/* Session Storage */
	SessionStore   	sessions_;
};

}