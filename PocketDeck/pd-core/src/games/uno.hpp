#pragma once

#include <boost/asio/awaitable.hpp>
#include <optional>
#include <game.hpp>
#include <vector>

namespace cards
{

class uno : public game
{
public:
	uno(const boost::json::object& config, cards::message_sink& ms);
	boost::asio::awaitable<void> handle_action(user_id player, const boost::json::object& action) override;
	boost::json::value get_status(user_id player) override;

	enum class card;
	enum class symbol;
	enum class color;

private:
	class player
	{
	private:
		std::vector<card> hand_;
		uno& game_;

	public:
		explicit player(std::vector<card> hand, uno& game);
		std::optional<card> draw(void);
		bool play_hand(size_t hand_index);
		bool color_wild(size_t hand_index, color c);
		void sort_hand(size_t from, size_t to);

		size_t hand_size(void) const { return hand_.size(); }

		boost::json::value describe_self(void);
		boost::json::value describe_other(void);
	};

	struct config
	{
		bool play_after_draw = true;
		bool aggregate_draws = true;
		bool black_on_black = true;
	} config_;

	std::vector<card> discard_pile_;
	deck<card> draw_pile_;
	std::vector<player> players_;
	size_t draw_counter_ = 0;
	int direction_ = 1;
	user_id current_player_index_, winner_;
	std::optional<size_t> play_after_draw_index_;

	bool play(card c);
	bool can_play(card c);
	std::optional<card> draw(void);
	user_id next_player(void);
	boost::asio::awaitable<void> advance_player(void);
	static symbol symbol_of(card c);
	static color color_of(card c);
	static card card_of(color c, symbol s);
	boost::asio::awaitable<void> handle_play(user_id player, size_t from);
	boost::asio::awaitable<void> handle_draw(user_id player);
};

} // cards
