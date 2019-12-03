#include "Base.cpp"

template< class R, class... Args >
class Pool: Base {
private:
	queue<tuple<
		function<R(Args...)>,
		tuple<Args...>,
		promise<R>*
		>> tasks;
	virtual void waitTasks() override;

	static Pool<R, Args...>* p_instance;
	Pool(int count);
public:
	static Pool<R, Args...>* getInstance(int count = thread::hardware_concurrency() - 1);
	~Pool();
	virtual int size() override;

	template< class Function >
	future<R> addTask(Function f, Args... args);
};

template< class R, class... Args >
Pool<R, Args...>* Pool<R, Args...>::p_instance = nullptr;

template<class R, class ...Args>
Pool<R, Args...>::Pool(int count)
{
	constructor_common_code(count, &Pool::waitTasks, this);
}

template<class R, typename ...Args>
Pool<R, Args...>::~Pool()
{
	destructor_common_code();
}

template<class R, class ...Args>
template<class Function>
future<R> Pool<R, Args...>::addTask(Function f, Args ...args)
{
	mt.lock();
	tasks.push(make_tuple(
		f,
		make_tuple(args...),
		new promise<R>
	));
	future<R> ret = get<2>(tasks.back())->get_future();
	cv.notify_one();
	mt.unlock();
	return ret;
}

template<class R, class ...Args>
void Pool<R, Args...>::waitTasks()
{
	while (true) {
		mt.lock();
		if (!(notEnd || tasks.size() > 0)) {
			mt.unlock();
			return;
		}
		cv.wait(mt, [this]() {
			return !notEnd || size();
		});
		if (tasks.size() == 0) {
			mt.unlock();
			return;
		}
		auto function = get<0>(tasks.front());
		//auto sw= get<1>(tasks.front());
		tuple<Args...> params = get<1>(tasks.front());
		promise<R>* p = get<2>(tasks.front());
		tasks.pop();
		mt.unlock();

		p->set_value(apply(function, params));
		delete p;
	}
}

template<class R, class ...Args>
Pool<R, Args...>*
Pool<R, Args...>::getInstance(int count)
{
	if (!p_instance)
		p_instance = new Pool<R, Args...>(count);
	return p_instance;
}

template<class R, class ...Args>
int Pool<R, Args...>::size()
{
	return tasks.size();
}

template<class R, class ...Args>
unique_ptr< Pool<R, Args...> >
make_pool(int count = thread::hardware_concurrency() - 1) {
	static bool isEntered = false;
	unique_ptr<Pool<R, Args...>> upr;
	if (isEntered)
		upr.reset(nullptr);
	else
		upr.reset(Pool<R, Args...>::getInstance(count));
	isEntered = true;
	return upr;
}



