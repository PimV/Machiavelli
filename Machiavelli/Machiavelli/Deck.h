#include <iostream>

template<typename T>
class Deck {
public:
	Deck(); // default constructor
	Deck(size_t size, CardType type); // preferred constructor
	Deck(const Deck& other); // copy constructor
	Deck(Deck&& other); // move constructor

	~Array3D(); // destructor (not virtual because this class is not meant to be subclassed)

	Array3D<T>& operator=(const Array3D& other); // copy assignment
	Array3D<T>& operator=(Array3D&& other); // move assignment

	// storing & retrieving values
	void put(const T& val, size_t x, size_t y, size_t z) { p[index(x, y, z)] = val; }
	T get(size_t x, size_t y, size_t z) const { return p[index(x, y, z)]; }

	// number of elements
	size_t size() const { return size; }

private:
	size_t x_size, y_size, z_size;
	T* p;

	// translation from (x, y, z) to flat index
	size_t index(size_t x, size_t y, size_t z) const { return y_size * x_size * z + x_size * y + x; }

	// copy initializer, used by copy constructor and copy assignment
	void init_storage(const Array3D& other);

	// cleanup, used by destructor and copy assignment
	void cleanup_storage();
};