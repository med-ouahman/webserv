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
	SessionManager(const std::string& name, size_t timeout);
	~SessionManager();
	std::string create_session();
	bool has_session(const std::string& id) const;
	void touch_session(const std::string& id);
	void delete_session(const std::string& id);

	void set_session_data(const std::string& id,
		const std::string& key,
		const std::string& value);
	std::string get_session_data(const std::string& id,
		const std::string& key) const;

	void cleanup();

	const std::string& get_cookie_name() const;

private:
	SessionManager(const SessionManager&);
	SessionManager& operator=(const SessionManager&);

	std::string generate_session_id();
	bool is_expired(std::time_t last_touch, std::time_t now) const;

	struct SessionData {
			std::time_t                        last_touch;
		std::map<std::string, std::string> data;
	};

	std::string cookie_name_;
	size_t		timeout_seconds_;
	
	SessionStore   	sessions_;
};

}
