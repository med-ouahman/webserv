#include "SessionManager.hpp"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cstdlib>

namespace http {

SessionManager::SessionManager(const std::string& name,
	size_t timeout,
	const std::string& store)
	: cookie_name_(name),
	timeout_seconds_(timeout),
	store_(store) {}

SessionManager::~SessionManager() {}

std::string SessionManager::create_session() {

	std::string id = generate_session_id();

	SessionData session;
	session.creation_time = std::time(NULL);
	session.last_touch    = session.creation_time;

	sessions_[id] = session;
	return id;
}

bool SessionManager::has_session(const std::string& id) const {
	if (id.empty())
		return false;

	SessionStore::const_iterator it = sessions_.find(id);
	if (it == sessions_.end())
		return false;

	return !is_expired(it->second.last_touch, std::time(NULL));
}

void SessionManager::touch_session(const std::string& id) {
	SessionStore::iterator it = sessions_.find(id);
	if (it == sessions_.end())
		return;

	it->second.last_touch = std::time(NULL);
}

void SessionManager::delete_session(const std::string& id) {
	sessions_.erase(id);
}


void SessionManager::set_session_data(const std::string& id,
	const std::string& key,
	const std::string& value) {

	SessionStore::iterator it = sessions_.find(id);
	if (it == sessions_.end())
		return;

	it->second.data[key] = value;
}

std::string SessionManager::get_session_data(const std::string& id,
	const std::string& key) const {

	SessionStore::const_iterator it = sessions_.find(id);
	if (it == sessions_.end())
		return "";

	std::map<std::string, std::string>::const_iterator data_it = it->second.data.find(key);
	if (data_it == it->second.data.end())
		return "";

	return data_it->second;
}

bool SessionManager::has_session_data(const std::string& id,
	const std::string& key) const {

	SessionStore::const_iterator it = sessions_.find(id);
	if (it == sessions_.end())
		return false;

	return it->second.data.find(key) != it->second.data.end();
}


void SessionManager::cleanup() {
	std::time_t now = std::time(NULL);

	SessionStore::iterator it = sessions_.begin();
	while (it != sessions_.end()) {
		if (is_expired(it->second.last_touch, now)) {
			SessionStore::iterator to_erase = it;
			++it;
			sessions_.erase(to_erase);
		} else {
			++it;
		}
	}
}

std::size_t SessionManager::get_session_count() const {
	return sessions_.size();
}

const std::string& SessionManager::get_cookie_name() const {
	return cookie_name_;
}

bool SessionManager::is_expired(std::time_t last_touch, std::time_t now) const {
	if (now < last_touch)
		return false;

	return static_cast<std::size_t>(now - last_touch) > timeout_seconds_;
}

std::string SessionManager::generate_session_id() {
	
	unsigned char raw[16];

	std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);
	if (urandom.is_open()) {
		urandom.read(reinterpret_cast<char*>(raw), sizeof(raw));
		urandom.close();
	} else {
		for (std::size_t i = 0; i < sizeof(raw); ++i)
			raw[i] = static_cast<unsigned char>(std::rand() % 256);
	}

	std::ostringstream oss;
	oss << std::hex << std::setfill('0');
	for (std::size_t i = 0; i < sizeof(raw); ++i)
		oss << std::setw(2) << static_cast<unsigned int>(raw[i]);

	std::string id = oss.str();

	while (sessions_.find(id) != sessions_.end()) {
		for (std::size_t i = 0; i < sizeof(raw); ++i)
			raw[i] = static_cast<unsigned char>(std::rand() % 256);

		std::ostringstream retry;
		retry << std::hex << std::setfill('0');
		for (std::size_t i = 0; i < sizeof(raw); ++i)
			retry << std::setw(2) << static_cast<unsigned int>(raw[i]);
		id = retry.str();
	}

	return id;
}

} // namespace http
