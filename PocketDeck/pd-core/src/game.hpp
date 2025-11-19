#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/json.hpp>
#include <type_traits>
#include <types.hpp>
#include <games/skipbo.hpp>
#include <vector>
#include <random>

namespace cards
{

class message_sink
{
public:
	virtual boost::asio::awaitable<void> send_to_user(user_id id, boost::json::value message) = 0;
	virtual boost::asio::awaitable<void> broadcast_others(user_id id, boost::json::value message) = 0;
	virtual boost::asio::awaitable<void> broadcast(boost::json::value message) = 0;
	virtual ~message_sink() = default;
};

class game
{
public:
	enum class type
	{
		SKIPBO,
		UNO,
		NOGAME,
	};

	template<typename T> class deck
	{
		static_assert(std::is_enum_v<T>, "deck<T> requires T to be an enum type");

	public:
		explicit deck(const std::vector<T>&& cards);
		explicit deck(const std::vector<T>& cards);
		void reset(const std::vector<T>&& cards);
		void shuffle(void);
		T draw(void);
		T peek(void) const;
		size_t size(void) const { return stack_.size(); };
		bool empty(void) const { return stack_.empty(); };
	private:
		std::vector<T> stack_;
		std::mt19937 rng_;
	};

	explicit game(message_sink& ms) :
		message_sink_(ms)
	{ }

	static std::unique_ptr<game> create(const game::type game, const boost::json::object& config, message_sink& ms);
	virtual boost::asio::awaitable<void> handle_action(user_id player, const boost::json::object& action) = 0;
	virtual boost::json::value get_status(user_id player) = 0;
	virtual ~game(void) = default;
	bool is_over(void) { return over_; }

protected:
	bool over_ = false;
	message_sink& message_sink_;
};

namespace game_utils
{

game::type string_to_game_type(const std::string& name);
const std::string game_path(const game::type game);

} // game_utils

} // cards

#include <deck.tpp>
