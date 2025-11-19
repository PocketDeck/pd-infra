namespace cards
{

template<typename T>
game::deck<T>::deck(const std::vector<T>&& cards) :
	rng_(std::random_device()())
{ reset(cards); }

template<typename T>
game::deck<T>::deck(const std::vector<T>& cards) :
	rng_(std::random_device()())
{ reset(std::vector<T>(cards)); }

template<typename T>
void game::deck<T>::reset(const std::vector<T>&& cards)
{
	stack_ = cards;
	shuffle();
}

template<typename T>
void game::deck<T>::shuffle(void)
{ std::shuffle(stack_.begin(), stack_.end(), rng_); }

template<typename T>
T game::deck<T>::draw(void)
{
	if (empty())
		throw std::out_of_range("deck is empty");
	T card = stack_.back();
	stack_.pop_back();
	return card;
}

template<typename T>
T game::deck<T>::peek(void) const
{
	if (empty())
		throw std::out_of_range("deck is empty");
	return stack_.back();
}

} // cards
