#include "Base.h"

template<typename Function, typename Derived>
inline void Base::constructor_common_code(int count, Function f, Derived d)
{
	for (int i = 0; i < count; i++)
		threads.push_back(thread(f, d));
}

inline void Base::destructor_common_code()
{
	mt.lock();
	notEnd = false;
	cv.notify_all();
	mt.unlock();

	for (auto& i : threads) 
		i.join();
}

