#include <vector>
#include <algorithm>

template<class T>
class Deck {
public:
	Deck(const std::initializer_list<T>& i) :elem(i) {}
	Deck() {}
	//Getters
	int size() const { return elem.size(); }
	bool empty() const { return elem.empty(); }

	//Stack operations
	void push(const T&);
	T pop();
	T peek();

	//Deck operations
	void shuffle();
private:
	std::vector<T> elem;
};


template<class T>
void Deck<T>::push(const T& t) {
	elem.push_back(t);
}

template<class T>
T Deck<T>::pop() {
	if (empty()) {
		throw std::out_of_range("underflow");
	}
	auto x = elem.back();
	elem.pop_back();
	return x;
}

template<class T>
T Deck<T>::peek() {
	if (empty()) {
		throw std::out_of_range("underflow");
	}
	return elem.back();
}

template<class T>
void Deck<T>::shuffle() {
	if (empty()) {
		throw std::out_of_range("underflow");
	}

	std::random_shuffle(elem.begin(), elem.end());
}