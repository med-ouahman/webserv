#include "SessionManager.hpp"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cstdlib>

namespace http
{

/* ------------------------------------------------------------------ */
/*  Singleton access                                                   */
/* ------------------------------------------------------------------ */

SessionManager& SessionManager::instance()
{
	static SessionManager instance_;
	return instance_;
}

SessionManager::SessionManager()
	: cookie_name_(""), timeout_seconds_(0), initialized_(false)
{
}

SessionManager::~SessionManager()
{
}

/* ------------------------------------------------------------------ */
/*  Initialization                                                     */
/* ------------------------------------------------------------------ */

void SessionManager::init(const std::string& cookie_name, std::size_t timeout_seconds)
{
	cookie_name_     = cookie_name;
	timeout_seconds_ = timeout_seconds;
	initialized_     = true;
}

bool SessionManager::is_initialized() const
{
	return initialized_;
}

/* ------------------------------------------------------------------ */
/*  Session lifecycle                                                  */
/* ------------------------------------------------------------------ */

std::string SessionManager::create_session()
{
	assert(initialized_ && "SessionManager::create_session() called before init()");

	std::string id = generate_session_id();

	SessionData session;
	session.creation_time = std::time(NULL);
	session.last_touch    = session.creation_time;

	sessions_[id] = session;
	return id;
}

bool SessionManager::has_session(const std::string& id) const
{
	if (id.empty())
		return false;

	std::map<std::string, SessionData>::const_iterator it = sessions_.find(id);
	if (it == sessions_.end())
		return false;

	return !is_expired(it->second.last_touch, std::time(NULL));
}

void SessionManager::touch_session(const std::string& id)
{
	std::map<std::string, SessionData>::iterator it = sessions_.find(id);
	if (it == sessions_.end())
		return;

	it->second.last_touch = std::time(NULL);
}

void SessionManager::delete_session(const std::string& id)
{
	sessions_.erase(id);
}

/* ------------------------------------------------------------------ */
/*  Session data storage                                               */
/* ------------------------------------------------------------------ */

void SessionManager::set_session_data(const std::string& id,
                                       const std::string& key,
                                       const std::string& value)
{
	std::map<std::string, SessionData>::iterator it = sessions_.find(id);
	if (it == sessions_.end())
		return;

	it->second.data[key] = value;
}

std::string SessionManager::get_session_data(const std::string& id,
                                              const std::string& key) const
{
	std::map<std::string, SessionData>::const_iterator it = sessions_.find(id);
	if (it == sessions_.end())
		return "";

	std::map<std::string, std::string>::const_iterator data_it = it->second.data.find(key);
	if (data_it == it->second.data.end())
		return "";

	return data_it->second;
}

bool SessionManager::has_session_data(const std::string& id,
                                       const std::string& key) const
{
	std::map<std::string, SessionData>::const_iterator it = sessions_.find(id);
	if (it == sessions_.end())
		return false;

	return it->second.data.find(key) != it->second.data.end();
}

/* ------------------------------------------------------------------ */
/*  Maintenance                                                        */
/* ------------------------------------------------------------------ */

void SessionManager::cleanup()
{
	std::time_t now = std::time(NULL);

	std::map<std::string, SessionData>::iterator it = sessions_.begin();
	while (it != sessions_.end())
	{
		if (is_expired(it->second.last_touch, now))
		{
			std::map<std::string, SessionData>::iterator to_erase = it;
			++it;
			sessions_.erase(to_erase);
		}
		else
		{
			++it;
		}
	}
}

std::size_t SessionManager::get_session_count() const
{
	return sessions_.size();
}

/* ------------------------------------------------------------------ */
/*  Config access                                                      */
/* ------------------------------------------------------------------ */

const std::string& SessionManager::get_cookie_name() const
{
	return cookie_name_;
}

/* ------------------------------------------------------------------ */
/*  Internals                                                          */
/* ------------------------------------------------------------------ */

bool SessionManager::is_expired(std::time_t last_touch, std::time_t now) const
{
	if (now < last_touch)
		return false; // clock skew guard: never treat as expired

	return static_cast<std::size_t>(now - last_touch) > timeout_seconds_;
}

std::string SessionManager::generate_session_id()
{
	// Read random bytes from the kernel CSPRNG rather than std::rand(),
	// which is predictable and unsuitable for session identifiers.
	unsigned char raw[16];

	std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);
	if (urandom.is_open())
	{
		urandom.read(reinterpret_cast<char*>(raw), sizeof(raw));
		urandom.close();
	}
	else
	{
		// Fallback if /dev/urandom is unavailable for some reason.
		// Not cryptographically strong, but keeps the server functional.
		for (std::size_t i = 0; i < sizeof(raw); ++i)
			raw[i] = static_cast<unsigned char>(std::rand() % 256);
	}

	std::ostringstream oss;
	oss << std::hex << std::setfill('0');
	for (std::size_t i = 0; i < sizeof(raw); ++i)
		oss << std::setw(2) << static_cast<unsigned int>(raw[i]);

	std::string id = oss.str();

	// Extremely unlikely collision guard: regenerate on the rare chance
	// the id is already in use.
	while (sessions_.find(id) != sessions_.end())
	{
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
