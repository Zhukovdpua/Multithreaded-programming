#include <functional>
#include <queue>
#include <vector>
#include <thread>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <tuple>
#include <future>
#include <memory>
using namespace std;

class Base {
	virtual void waitTasks() = 0;
	virtual int size() = 0;
protected:
	bool notEnd;
	vector<thread> threads;
	condition_variable_any cv;
	mutex mt;
	Base() :notEnd(true) {}
	inline void destructor_common_code();

	template<typename Function, typename Derived>
	inline void constructor_common_code(int count, Function f, Derived d);
};

