#include <boost/asio/awaitable.hpp>
#include <game.hpp>
#include <games/uno.hpp>
#include <memory>

using namespace cards;
namespace json = boost::json;
using boost::asio::awaitable;

class nogame : public game
{
public:
	nogame(const json::object& config, message_sink& ms) :
		game(ms)
	{ (void) config; }

	awaitable<void> handle_action(user_id id, const json::object& action) override
	{
		(void) id;
		(void) action;
		return { };
	}

	json::value get_status(user_id id) override
	{
		(void) id;
		return { };
	}
};

std::unique_ptr<game> game::create(const game::type game, const json::object& config, message_sink& ms)
{
	switch (game)
	{
	//case game::type::SKIPBO: return std::make_unique<skipbo>(std::move(config), ms);
	case type::UNO:          return std::make_unique<uno>   (std::move(config), ms);
	case type::NOGAME:
	default:                 return std::make_unique<nogame>(std::move(config), ms);
	}
}

namespace cards::game_utils
{

namespace
{
const std::unordered_map<std::string, game::type> game_type_map =
{
	{"skipbo", game::type::SKIPBO},
	{"uno"   , game::type::UNO}   ,
};
const std::unordered_map<game::type, std::string> game_path_map =
{
	{game::type::SKIPBO, "games/skipbo"},
	{game::type::UNO   , "games/uno"}   ,
};
} //

game::type string_to_game_type(const std::string& name)
{
	return game_type_map.contains(name) ? game_type_map.at(name) : game::type::NOGAME;
}

const std::string game_path(const game::type game)
{
	return game_path_map.contains(game) ? game_path_map.at(game) : "";
}

} // game_utils
