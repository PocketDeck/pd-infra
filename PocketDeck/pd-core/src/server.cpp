#include "game.hpp"
#include <boost/asio/awaitable.hpp>
#include <unordered_map>
#include <utility>
#include <memory>
#include <server.hpp>
#include <iostream>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/beast.hpp>

using namespace cards;
namespace asio = boost::asio;
namespace beast = boost::beast;
namespace json = boost::json;
using tcp = boost::asio::ip::tcp;
using asio::awaitable;
using asio::use_awaitable;
using asio::co_spawn;

// user_session
awaitable<void> user_session::send(const json::value& message) const
{
	co_await socket_->async_write(asio::buffer(serialize(message)), asio::use_awaitable);
}

awaitable<void> user_session::run(void)
{
	try
	{
		co_await socket_->async_accept(use_awaitable);

		while (true)
		{
			beast::flat_buffer buffer;
			co_await socket_->async_read(buffer, use_awaitable);
			std::string msg = beast::buffers_to_string(buffer.data());

			co_await handle_action(msg);
		}
	} catch (const std::exception& e)
	{
		std::cerr << "WebSocket error: " << e.what() << std::endl;
	}
}

awaitable<void> user_session::handle_action(std::string msg)
{
	auto o = json::parse(msg).as_object();
	auto type = o.at("action");

	if (type == "create" && state_ == state::STRAY)
	{
		std::string name    = o.at("name").as_string().c_str();
		game::type game     = game_utils::string_to_game_type(o.at("game").as_string().c_str());
		json::object config = o.at("config").as_object();

		lobby_ = &server_.create_lobby(game, config);
		id_ = co_await lobby_->add_player(name, weak_from_this());
		state_ = state::BOUND;
		co_await send({ {"action", "create"}, {"room", lobby_->id_} });
	}
	else if (type == "join" && state_ == state::STRAY)
	{
		std::string name = o.at("name").as_string().c_str();
		lobby_id id      = json::value_to<lobby_id>(o.at("room"));

		lobby_ = server_.find_lobby(id);
		if (lobby_)
		{
			id_ = co_await lobby_->add_player(name, weak_from_this());
			state_ = state::BOUND;
			co_await send({ {"action", "join"} });
		}
		else
			co_await send({ {"error", "join"} });
	}
	else if (type == "leave" && state_ == state::BOUND)
	{
		co_await lobby_->remove_player(id_);
		state_ = state::STRAY;
		co_await send({ {"action", "leave"} });
	}
	else if (type == "ready" && state_ == state::BOUND)
	{
		co_await lobby_->ready_player(id_);
		co_await send({ {"action", "ready"} });
	}
	else if (type == "unready" && state_ == state::BOUND)
	{
		co_await lobby_->unready_player(id_);
		co_await send({ {"action", "unready"} });
	}
	else if (type == "status" && state_ == state::BOUND)
	{
		json::value status = lobby_->fetch_status(id_);
		co_await send({ {"action", "status"}, {"message", status} });
	}
	else if (type == "game" && state_ == state::BOUND)
	{
		co_await lobby_->game_action(id_, o.at("action").as_object());
	}
	co_return;
}


// lobby
lobby::lobby(lobby_id id, game::type game, json::object config) :
	id_(id),
	game_type_(game),
	game_config_(config)
{ }

awaitable<user_id> lobby::add_player(const std::string &name, std::weak_ptr<user_session> session)
{
	user_id id = next_user_id_++;
	player_ids_.insert(id);
	players_.emplace(id, (player) {name, session});
	co_await broadcast_others(id, { {"event", "join"}, {"id", id}, {"name", name} });
	co_return id;
}

awaitable<void> lobby::remove_player(user_id id)
{
	player_ids_.erase(id);
	players_.erase(id);
	players_ready_.erase(id);
	disconnected_.erase(id);
	co_await broadcast_others(id, { {"event", "leave"}, {"id", id} });
}

awaitable<void> lobby::ready_player(user_id id)
{
	players_ready_.insert(id);
	co_await broadcast_others(id, { {"event", "ready"}, {"id", id} });

	if (players_ready_.size() == players_.size())
	{
		squash_user_ids();
		game_config_["num_players"] = players_.size();
		game_instance_ = game::create(game_type_, game_config_, *this);
		state_ = state::RUNNING;
		std::cout << "Game starting..." << std::endl;
		co_await broadcast({ {"event", "start"}, {"page", game_utils::game_path(game_type_)} });
	}
}

awaitable<void> lobby::unready_player(user_id id)
{
	players_ready_.erase(id);
	co_await broadcast_others(id, { {"event", "unready"}, {"id", id} });
}

awaitable<void> lobby::game_action(user_id id, json::object action)
{
	if (state_ == state::RUNNING)
		co_await game_instance_->handle_action(id, action);
}

json::value lobby::fetch_status(user_id id)
{
	switch (state_)
	{
	case state::INIT:
		{
			json::array players;
			for (const user_id uid : player_ids_)
			{
				bool ready = players_ready_.contains(uid);
				players.push_back({ {"id", uid}, {"name", players_.at(uid).name_}, {"ready", ready} });
			}
			json::value response = { {"state", "init"}, {"room", id_}, {"players", players}, {"my-id", id} };
			return response;
		}
	case state::RUNNING:
		{
			json::array players;
			for (const user_id uid : player_ids_)
			{
				players.push_back({ {"id", uid}, {"name", players_.at(uid).name_} });
			}
			json::value game_status = game_instance_->get_status(id);
			json::value response = { {"state", "running"}, {"game", game_status}, {"players", players}, {"my-id", id} };
			return response;
		}
	}

	return { };
}

awaitable<void> lobby::send_to_user(user_id id, json::value message)
{
	co_await players_.at(id).session_.lock()->send(message);
}

awaitable<void> lobby::broadcast_others(user_id exclude, json::value message)
{
	json::value data = { {"action", "broadcast"}, {"message", message} };

	for (const user_id id : player_ids_)
	{
		if (id == exclude) continue;
		co_await send_to_user(id, data);
	}
}

awaitable<void> lobby::broadcast(json::value message)
{
	json::value data = { {"action", "broadcast"}, {"message", message} };

	for (const user_id id : player_ids_)
		co_await send_to_user(id, data);
}

void lobby::squash_user_ids(void)
{
	user_id new_id = 0;

	// TODO: maybe remove const
	for (const auto& entry : players_)
	{
		entry.second.session_.lock()->id_ = new_id;
		if (disconnected_.contains(entry.first))
		{
			disconnected_.erase(entry.first);
			disconnected_.insert(new_id);
		}
		if (players_ready_.contains(entry.first))
		{
			players_ready_.erase(entry.first);
			players_ready_.insert(new_id);
		}
		++new_id;
	}

	player_ids_.clear();
	for (size_t i = 0; i < new_id; ++i)
		player_ids_.insert(i);
}


// server
asio::awaitable<void> server::accept_clients(void)
{
	while (true)
	{
		tcp::socket socket = co_await acceptor_.async_accept(use_awaitable);
		co_spawn(
			acceptor_.get_executor(),
			[socket = std::move(socket), this]() mutable -> awaitable<void>
			{
				auto ws = std::make_shared<websocket>(std::move(socket));
				auto session = std::make_shared<user_session>(ws, *this);
				co_await session->run();
			},
			asio::detached
		);
	}
}

server::server(asio::io_context& io, unsigned short port):
	acceptor_(io, tcp::endpoint(tcp::v4(), port)),
	rng_(std::random_device()())
{
	std::cout << "Serving WebSocket on " << acceptor_.local_endpoint().address().to_string()
		<< " port " << acceptor_.local_endpoint().port()
		<< " (ws://" << acceptor_.local_endpoint().address().to_string()
		<< ":" << acceptor_.local_endpoint().port()
		<< ") ..."<< std::endl;
	co_spawn(io, accept_clients(), asio::detached);
	io.run();
}

std::string server::new_lobby_id(void)
{
	const size_t num_pos = 6;
	static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

	std::string id;
	do
	{
		id.clear();
		for (size_t i = 0; i < num_pos; ++i)
			id += alphanum[dis(rng_)];
	} while (lobbies_.contains(id));

	return id;
}

lobby& server::create_lobby(game::type game, json::object config)
{
	lobby_id id = new_lobby_id();
	lobbies_[id] = std::make_unique<lobby>(lobby{id, game, config}); // TODO: maybe make not unique_ptr?
	return *lobbies_[id];
}

lobby* server::find_lobby(lobby_id id)
{
	return lobbies_.contains(id) ? lobbies_[id].get() : nullptr;
}
