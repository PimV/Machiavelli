#pragma once
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

	//Vector operations
	T get(size_t i);
	void remove(size_t i);

	//Stack operations
	void push(const T&);
	T pop();
	T peek();

	//Deck operations
	void shuffle();

	//Print operations
	void print();
private:
	std::vector<T> elem;
};

template<class T>
T Deck<T>::get(size_t i) {
	if (i >= 0 && i < size()) {
		return elem.at(i);
	}
	return nullptr;
}

template<class T>
void Deck<T>::remove(size_t i) {
	if (i >= 0 && i < size()) {
		elem.erase(elem.begin() + i);
	}
}

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

template<class T>
void Deck<T>::print() {
	for (size_t i = 0; i < elem.size(); i++) {
		std::cout << elem.at(i)->toString() << std::endl;
	}
}