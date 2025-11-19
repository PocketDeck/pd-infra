#pragma once

#include <boost/asio/awaitable.hpp>
#include <string>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <memory>
#include <random>
#include <boost/asio/io_context.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <types.hpp>
#include <game.hpp>

namespace cards
{

using websocket = boost::beast::websocket::stream<boost::asio::ip::tcp::socket>;

class server;
class lobby;

struct user_session : public std::enable_shared_from_this<user_session>
{
	std::shared_ptr<websocket> socket_; // TODO: why shared_ptr
	server& server_;
	lobby* lobby_;
	user_id id_;
	enum class state { STRAY, BOUND } state_ = state::STRAY;

	user_session(std::shared_ptr<websocket> socket, server& server):
		socket_(socket),
		server_(server),
		lobby_(nullptr)
	{ }

	boost::asio::awaitable<void> send(const boost::json::value& message) const;
	boost::asio::awaitable<void> run(void);
	boost::asio::awaitable<void> handle_action(std::string msg);
};

class player
{
public:
	const std::string name_;
	std::weak_ptr<user_session> session_;
	long long points_;

	player(const std::string& name, std::weak_ptr<user_session> session) :
		name_(name),
		session_(session)
	{ }
};

class lobby: public message_sink
{
public:
	explicit lobby(lobby_id id, game::type game, boost::json::object config);
	boost::asio::awaitable<user_id> add_player(const std::string &name, std::weak_ptr<user_session> session);
	boost::asio::awaitable<void> remove_player(user_id id);
	boost::asio::awaitable<void> ready_player(user_id id);
	boost::asio::awaitable<void> unready_player(user_id id);
	boost::asio::awaitable<void> game_action(user_id id, boost::json::object action);
	boost::json::value fetch_status(user_id id);

	// message_sink interface
	boost::asio::awaitable<void> send_to_user(user_id id, boost::json::value message);
	boost::asio::awaitable<void> broadcast_others(user_id exclude, boost::json::value message);
	boost::asio::awaitable<void> broadcast(boost::json::value message);

	const lobby_id id_;

private:
	std::map<user_id, player> players_;
	std::unordered_set<user_id> disconnected_, players_ready_, player_ids_;
	std::unique_ptr<game> game_instance_;
	enum class state {INIT, RUNNING };
	state state_ = state::INIT;
	user_id next_user_id_ = 0;
	void squash_user_ids(void);

	const game::type game_type_;
	boost::json::object game_config_;
};

class server
{
public:
	server(boost::asio::io_context& io, unsigned short port);

	lobby& create_lobby(game::type game, boost::json::object config);
	lobby* find_lobby(lobby_id id);

private:
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::awaitable<void> accept_clients(void);
	std::unordered_map<lobby_id, std::unique_ptr<lobby>> lobbies_;

	std::mt19937 rng_;
	std::string new_lobby_id(void);
};

} // cards
