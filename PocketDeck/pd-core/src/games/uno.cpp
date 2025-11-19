#include <games/uno.hpp>
#include <vector>

using namespace cards;
namespace json = boost::json;
using boost::asio::awaitable;

enum class uno::color
{
	RED=0, YELLOW, GREEN, BLUE, BLACK
};
enum class uno::card
{
	RED_0=0           , YELLOW_0             , GREEN_0             , BLUE_0             ,
	RED_1             , YELLOW_1             , GREEN_1             , BLUE_1             ,
	RED_2             , YELLOW_2             , GREEN_2             , BLUE_2             ,
	RED_3             , YELLOW_3             , GREEN_3             , BLUE_3             ,
	RED_4             , YELLOW_4             , GREEN_4             , BLUE_4             ,
	RED_5             , YELLOW_5             , GREEN_5             , BLUE_5             ,
	RED_6             , YELLOW_6             , GREEN_6             , BLUE_6             ,
	RED_7             , YELLOW_7             , GREEN_7             , BLUE_7             ,
	RED_8             , YELLOW_8             , GREEN_8             , BLUE_8             ,
	RED_9             , YELLOW_9             , GREEN_9             , BLUE_9             ,
	RED_SKIP          , YELLOW_SKIP          , GREEN_SKIP          , BLUE_SKIP          ,
	RED_REVERSE       , YELLOW_REVERSE       , GREEN_REVERSE       , BLUE_REVERSE       ,
	RED_DRAW_TWO      , YELLOW_DRAW_TWO      , GREEN_DRAW_TWO      , BLUE_DRAW_TWO      ,
	RED_WILD          , YELLOW_WILD          , GREEN_WILD          , BLUE_WILD          ,
	WILD,
	RED_WILD_DRAW_FOUR, YELLOW_WILD_DRAW_FOUR, GREEN_WILD_DRAW_FOUR, BLUE_WILD_DRAW_FOUR,
	WILD_DRAW_FOUR,
};
enum class uno::symbol
{
	ZERO           = static_cast<int>(uno::card::RED_0),
	ONE            = static_cast<int>(uno::card::RED_1),
	TWO            = static_cast<int>(uno::card::RED_2),
	THREE          = static_cast<int>(uno::card::RED_3),
	FOUR           = static_cast<int>(uno::card::RED_4),
	FIVE           = static_cast<int>(uno::card::RED_5),
	SIX            = static_cast<int>(uno::card::RED_6),
	SEVEN          = static_cast<int>(uno::card::RED_7),
	EIGHT          = static_cast<int>(uno::card::RED_8),
	NINE           = static_cast<int>(uno::card::RED_9),
	SKIP           = static_cast<int>(uno::card::RED_SKIP),
	REVERSE        = static_cast<int>(uno::card::RED_REVERSE),
	DRAW_TWO       = static_cast<int>(uno::card::RED_DRAW_TWO),
	WILD           = static_cast<int>(uno::card::RED_WILD),
	WILD_DRAW_FOUR = static_cast<int>(uno::card::RED_WILD_DRAW_FOUR)
};

uno::symbol uno::symbol_of(card c)
{
	switch (c)
	{
	case card::RED_0: case card::YELLOW_0: case card::GREEN_0: case card::BLUE_0: return symbol::ZERO;
	case card::RED_1: case card::YELLOW_1: case card::GREEN_1: case card::BLUE_1: return symbol::ONE;
	case card::RED_2: case card::YELLOW_2: case card::GREEN_2: case card::BLUE_2: return symbol::TWO;
	case card::RED_3: case card::YELLOW_3: case card::GREEN_3: case card::BLUE_3: return symbol::THREE;
	case card::RED_4: case card::YELLOW_4: case card::GREEN_4: case card::BLUE_4: return symbol::FOUR;
	case card::RED_5: case card::YELLOW_5: case card::GREEN_5: case card::BLUE_5: return symbol::FIVE;
	case card::RED_6: case card::YELLOW_6: case card::GREEN_6: case card::BLUE_6: return symbol::SIX;
	case card::RED_7: case card::YELLOW_7: case card::GREEN_7: case card::BLUE_7: return symbol::SEVEN;
	case card::RED_8: case card::YELLOW_8: case card::GREEN_8: case card::BLUE_8: return symbol::EIGHT;
	case card::RED_9: case card::YELLOW_9: case card::GREEN_9: case card::BLUE_9: return symbol::NINE;
	case card::RED_SKIP: case card::YELLOW_SKIP: case card::GREEN_SKIP: case card::BLUE_SKIP: return symbol::SKIP;
	case card::RED_REVERSE: case card::YELLOW_REVERSE: case card::GREEN_REVERSE: case card::BLUE_REVERSE: return symbol::REVERSE;
	case card::RED_DRAW_TWO: case card::YELLOW_DRAW_TWO: case card::GREEN_DRAW_TWO: case card::BLUE_DRAW_TWO: return symbol::DRAW_TWO;
	case card::WILD: case card::RED_WILD: case card::YELLOW_WILD: case card::GREEN_WILD: case card::BLUE_WILD: return symbol::WILD;
	case card::WILD_DRAW_FOUR: case card::RED_WILD_DRAW_FOUR: case card::YELLOW_WILD_DRAW_FOUR: case card::GREEN_WILD_DRAW_FOUR: case card::BLUE_WILD_DRAW_FOUR: return symbol::WILD_DRAW_FOUR;
	}

	throw std::exception();
}

uno::color uno::color_of(card c)
{
	switch (c)
	{
	case card::RED_0: case card::RED_1: case card::RED_2: case card::RED_3: case card::RED_4: case card::RED_5: case card::RED_6: case card::RED_7: case card::RED_8: case card::RED_9: case card::RED_SKIP: case card::RED_REVERSE: case card::RED_DRAW_TWO: case card::RED_WILD: case card::RED_WILD_DRAW_FOUR: return color::RED;
	case card::YELLOW_0: case card::YELLOW_1: case card::YELLOW_2: case card::YELLOW_3: case card::YELLOW_4: case card::YELLOW_5: case card::YELLOW_6: case card::YELLOW_7: case card::YELLOW_8: case card::YELLOW_9: case card::YELLOW_SKIP: case card::YELLOW_REVERSE: case card::YELLOW_DRAW_TWO: case card::YELLOW_WILD: case card::YELLOW_WILD_DRAW_FOUR: return color::YELLOW;
	case card::GREEN_0: case card::GREEN_1: case card::GREEN_2: case card::GREEN_3: case card::GREEN_4: case card::GREEN_5: case card::GREEN_6: case card::GREEN_7: case card::GREEN_8: case card::GREEN_9: case card::GREEN_SKIP: case card::GREEN_REVERSE: case card::GREEN_DRAW_TWO: case card::GREEN_WILD: case card::GREEN_WILD_DRAW_FOUR: return color::GREEN;
	case card::BLUE_0: case card::BLUE_1: case card::BLUE_2: case card::BLUE_3: case card::BLUE_4: case card::BLUE_5: case card::BLUE_6: case card::BLUE_7: case card::BLUE_8: case card::BLUE_9: case card::BLUE_SKIP: case card::BLUE_REVERSE: case card::BLUE_DRAW_TWO: case card::BLUE_WILD: case card::BLUE_WILD_DRAW_FOUR: return color::BLUE;
	case card::WILD: case card::WILD_DRAW_FOUR: return color::BLACK;
	}

	throw std::exception();
}

uno::card uno::card_of(color c, symbol s)
{
	if (c == color::BLACK and s != symbol::WILD and s != symbol::WILD_DRAW_FOUR)
		throw std::invalid_argument("only wild and draw-four cards can be black");

	return static_cast<card>(static_cast<int>(s) + static_cast<int>(c));
}

namespace
{
	std::vector<uno::card> initial_deck =
	{
		uno::card::RED_0, uno::card::YELLOW_0, uno::card::GREEN_0, uno::card::BLUE_0,
		uno::card::RED_1, uno::card::YELLOW_1, uno::card::GREEN_1, uno::card::BLUE_1, uno::card::RED_1, uno::card::YELLOW_1, uno::card::GREEN_1, uno::card::BLUE_1,
		uno::card::RED_2, uno::card::YELLOW_2, uno::card::GREEN_2, uno::card::BLUE_2, uno::card::RED_2, uno::card::YELLOW_2, uno::card::GREEN_2, uno::card::BLUE_2,
		uno::card::RED_3, uno::card::YELLOW_3, uno::card::GREEN_3, uno::card::BLUE_3, uno::card::RED_3, uno::card::YELLOW_3, uno::card::GREEN_3, uno::card::BLUE_3,
		uno::card::RED_4, uno::card::YELLOW_4, uno::card::GREEN_4, uno::card::BLUE_4, uno::card::RED_4, uno::card::YELLOW_4, uno::card::GREEN_4, uno::card::BLUE_4,
		uno::card::RED_5, uno::card::YELLOW_5, uno::card::GREEN_5, uno::card::BLUE_5, uno::card::RED_5, uno::card::YELLOW_5, uno::card::GREEN_5, uno::card::BLUE_5,
		uno::card::RED_6, uno::card::YELLOW_6, uno::card::GREEN_6, uno::card::BLUE_6, uno::card::RED_6, uno::card::YELLOW_6, uno::card::GREEN_6, uno::card::BLUE_6,
		uno::card::RED_7, uno::card::YELLOW_7, uno::card::GREEN_7, uno::card::BLUE_7, uno::card::RED_7, uno::card::YELLOW_7, uno::card::GREEN_7, uno::card::BLUE_7,
		uno::card::RED_8, uno::card::YELLOW_8, uno::card::GREEN_8, uno::card::BLUE_8, uno::card::RED_8, uno::card::YELLOW_8, uno::card::GREEN_8, uno::card::BLUE_8,
		uno::card::RED_9, uno::card::YELLOW_9, uno::card::GREEN_9, uno::card::BLUE_9, uno::card::RED_9, uno::card::YELLOW_9, uno::card::GREEN_9, uno::card::BLUE_9,
		uno::card::RED_SKIP, uno::card::YELLOW_SKIP, uno::card::GREEN_SKIP, uno::card::BLUE_SKIP, uno::card::RED_SKIP, uno::card::YELLOW_SKIP, uno::card::GREEN_SKIP, uno::card::BLUE_SKIP,
		uno::card::RED_REVERSE, uno::card::YELLOW_REVERSE, uno::card::GREEN_REVERSE, uno::card::BLUE_REVERSE, uno::card::RED_REVERSE, uno::card::YELLOW_REVERSE, uno::card::GREEN_REVERSE, uno::card::BLUE_REVERSE,
		uno::card::RED_DRAW_TWO, uno::card::YELLOW_DRAW_TWO, uno::card::GREEN_DRAW_TWO, uno::card::BLUE_DRAW_TWO, uno::card::RED_DRAW_TWO, uno::card::YELLOW_DRAW_TWO, uno::card::GREEN_DRAW_TWO, uno::card::BLUE_DRAW_TWO,
		uno::card::WILD, uno::card::WILD, uno::card::WILD, uno::card::WILD,
		uno::card::WILD_DRAW_FOUR, uno::card::WILD_DRAW_FOUR, uno::card::WILD_DRAW_FOUR, uno::card::WILD_DRAW_FOUR,
	};
}

uno::uno(const boost::json::object& config, cards::message_sink& ms) :
	game(ms),
	draw_pile_(initial_deck)
{
	size_t initial_hand_size = value_to<size_t>(config.at("initial_hand_size"));
	size_t num_players = value_to<size_t>(config.at("num_players"));

	for (size_t i = 0; i < num_players; ++i)
	{
		std::vector<card> stockpile;
		for (size_t j = 0; j < initial_hand_size; ++j)
			stockpile.push_back(draw_pile_.draw()); // TODO: exception
		players_.emplace_back(std::move(stockpile), *this);
	}

	discard_pile_.push_back(draw_pile_.draw()); // TODO: exception
}

user_id uno::next_player(void)
{
	return (current_player_index_ + direction_) % players_.size();
}

awaitable<void> uno::advance_player(void)
{
	user_id next = next_player();
	direction_ = direction_ > 0 ? 1 : -1;
	co_await message_sink_.broadcast({{"action", "next"}, {"id", next}});
	current_player_index_ = next;
	co_await message_sink_.broadcast({{"action", "ack"}});
}

bool uno::can_play(card c)
{
	if (color_of(c) == color::BLACK) return false;

	if (draw_counter_)
	{
		if (not config_.aggregate_draws)
			return false;
		if (symbol_of(discard_pile_.back()) == symbol::DRAW_TWO)
			return symbol_of(c) == symbol::DRAW_TWO;
		if (symbol_of(discard_pile_.back()) == symbol::WILD_DRAW_FOUR)
			return symbol_of(c) == symbol::WILD_DRAW_FOUR and config_.black_on_black;
		return false;
	}

	if (symbol_of(c) == symbol::WILD or symbol_of(c) == symbol::WILD_DRAW_FOUR)
	{
		symbol discard_symbol = symbol_of(discard_pile_.back());
		return
			(discard_symbol != symbol::WILD and
			 discard_symbol != symbol::WILD_DRAW_FOUR) or
			config_.black_on_black;
	}

	return
		color_of(c) == color_of(discard_pile_.back()) or
		symbol_of(c) == symbol_of(discard_pile_.back());
}

bool uno::play(card c)
{
	if (not can_play(c)) return false;

	// restore black color
	symbol discard_symbol = symbol_of(discard_pile_.back());
	if (discard_symbol == symbol::WILD or discard_symbol == symbol::WILD_DRAW_FOUR)
		discard_pile_.back() = card_of(color::BLACK, discard_symbol);

	discard_pile_.push_back(c);

	switch (symbol_of(c))
	{
	case symbol::SKIP          : direction_ = direction_ > 0 ? 2 : -2; break;
	case symbol::REVERSE       : direction_ *= -1; break;
	case symbol::DRAW_TWO      : draw_counter_ += 2; break;
	case symbol::WILD_DRAW_FOUR: draw_counter_ += 4; break;
	default: break;
	}

	return true;
}

std::optional<uno::card> uno::draw(void)
{
	if (draw_pile_.empty())
	{
		if (discard_pile_.size() <= 1)
			return std::nullopt;

		card top = discard_pile_.back();
		discard_pile_.pop_back();

		draw_pile_.reset(std::move(discard_pile_));

		discard_pile_.clear();
		discard_pile_.push_back(top);
	}

	return draw_pile_.draw();
}


// player
uno::player::player(std::vector<uno::card> hand, uno& game) :
	hand_(std::move(hand)),
	game_(game)
{ };

std::optional<uno::card> uno::player::draw(void)
{
	auto c = game_.draw();
	if (c) hand_.push_back(*c);
	return c;
}

bool uno::player::play_hand(size_t hand_index)
{
	if (hand_index >= hand_.size() or not game_.play(hand_.at(hand_index)))
		return false;

	hand_.erase(hand_.begin() + hand_index);
	return true;
}

bool uno::player::color_wild(size_t hand_index, color c)
{
	if (hand_index >= hand_.size() or c == color::BLACK)
		return false;

	card& wild = hand_[hand_index];
	if (symbol_of(wild) != symbol::WILD and symbol_of(wild) != symbol::WILD_DRAW_FOUR)
		return false;

	wild = card_of(c, symbol_of(wild));
	return true;
}

void uno::player::sort_hand(size_t from, size_t to)
{
	if (from >= hand_.size() or to >= hand_.size() or from == to)
		return;

	card c = hand_.at(from);
	hand_.erase(hand_.begin() + from);
	hand_.insert(hand_.begin() + to, c);
}

json::value uno::player::describe_self(void)
{
	json::array hand;
	for (card c : hand_)
		hand.push_back(static_cast<int>(c));

	return { {"hand", hand} };
}

json::value uno::player::describe_other(void)
{
	return { {"hand_size", hand_size()} };
}


// event handlers
awaitable<void> uno::handle_play(user_id player, size_t from)
{
	uno::player& p = players_.at(player);

	if (player != current_player_index_ or
		(play_after_draw_index_ and from != *play_after_draw_index_) or
		not p.play_hand(from))
	{
		co_await message_sink_.send_to_user(player, {{"error", "play"}});
		co_return;
	}

	play_after_draw_index_ = std::nullopt;
	co_await message_sink_.send_to_user(player, {{"action", "play"}, {"hand_index", from}});
	co_await message_sink_.broadcast_others(player, {{"action", "play"}, {"hand_index", from}, {"card", static_cast<int>(discard_pile_.back())}, {"id", player}});

	if (p.hand_size() == 0)
	{
		over_ = true;
		winner_ = player;
		co_await message_sink_.broadcast({{"action", "over"}, {"winner", player}});
		co_return;
	}
	if (p.hand_size() == 1)
		co_await message_sink_.broadcast({{"action", "uno"}, {"id", player}});

	co_await advance_player();
}

awaitable<void> uno::handle_draw(user_id player)
{
	uno::player& p = players_.at(player);

	if (player != current_player_index_)
	{
		co_await message_sink_.send_to_user(player, {{"error", "draw"}});
		co_return;
	}

	if (draw_counter_)
	{
		json::array drawn;
		for (; draw_counter_ > 0; --draw_counter_)
		{
			auto c = p.draw();
			if (not c) break;
			drawn.push_back(static_cast<int>(*c));
		}

		co_await message_sink_.send_to_user(player, {{"action", "draw"}, {"cards", drawn}});
		co_await message_sink_.broadcast_others(player, {{"action", "draw"}, {"id", player}, {"cards", drawn.size()}});
	}
	else
	{
		json::array drawn;
		auto c = p.draw();
		if (c) drawn.push_back(static_cast<int>(*c));
		co_await message_sink_.broadcast_others(player, {{"action", "draw"}, {"id", player}, {"cards", drawn.size()}});

		if (c and can_play(*c) and config_.play_after_draw)
		{
			co_await message_sink_.send_to_user(player, {{"action", "keep_or_play"}, {"card", drawn}});
			play_after_draw_index_ = p.hand_size() - 1;
			co_return;
		}

		co_await message_sink_.send_to_user(player, {{"action", "draw"}, {"cards", drawn} });
	}

	co_await advance_player();
}

awaitable<void> uno::handle_action(user_id player, const json::object& action)
{
	if (over_) co_return;

	std::unordered_map<char, uno::color> color_map
	{
		{'y', color::YELLOW},
		{'r', color::RED},
		{'g', color::GREEN},
		{'b', color::BLUE},
	};

	auto type = action.at("action");

	if (type == "sort")
	{
		size_t from = json::value_to<size_t>(action.at("from"));
		size_t to   = json::value_to<size_t>(action.at("to"));
		players_[player].sort_hand(from, to);
		if (player == current_player_index_ && play_after_draw_index_ && from == *play_after_draw_index_)
			play_after_draw_index_ = to;
		co_await message_sink_.send_to_user(player, "sort");
	}
	else if (type == "wild")
	{
		char color_char = *action.at("color").as_string().begin();
		color c = color_map.contains(color_char) ? color_map[color_char] : color::BLACK;
		players_[player].color_wild(json::value_to<size_t>(action.at("index")), c);
	}
	else if (type == "play")
		co_await handle_play(player, json::value_to<size_t>(action.at("from")));
	else if (type == "draw")
		co_await handle_draw(player);
	else if (type == "keep" and player == current_player_index_ and play_after_draw_index_)
	{
		play_after_draw_index_ = std::nullopt;
		co_await advance_player();
	}
}

json::value uno::get_status(user_id player)
{
	json::array discard_pile;
	for (card c : discard_pile_)
		discard_pile.push_back(static_cast<int>(c));

	json::array others;
	for (user_id id = 1; id < players_.size(); ++id)
	{
		user_id other = (id + player) % players_.size();

		others.push_back({
			{"id", other},
			{"player", players_[other].describe_other()}
		});
	}

	json::object me = { {"player", players_[player].describe_self()} };
	json::object pad;
	if (play_after_draw_index_) pad = { {"pad", *play_after_draw_index_} };

	return {
		{"discard", discard_pile},
		{"me", me},
		{"others", others},
		{"draw", draw_pile_.size()},
		{"active", current_player_index_},
		pad
	};
}
