// Специализация для void

template<typename ...Args>
class Pool<void, Args...> :Base {
private:
	queue<pair<
		function<void(Args...)>,
		tuple<Args...>
		>> tasks;
	virtual void waitTasks() override;

	static Pool<void, Args...>* p_instance;
	Pool(int count);
public:
	static Pool<void, Args...>* getInstance(int count = thread::hardware_concurrency() - 1);
	~Pool();
	virtual int size() override;

	template< class Function >
	void addTask(Function f, Args... args);
};

template<class... Args >
Pool<void, Args...>* Pool<void, Args...>::p_instance = nullptr;

template<class ...Args>
Pool<void, Args...>::Pool(int count)
{
	constructor_common_code(count, &Pool::waitTasks, this);
}

template<typename ...Args>
Pool<void, Args...>::~Pool()
{
	destructor_common_code();
}

template<class ...Args>
template<class Function>
void Pool<void, Args...>::addTask(Function f, Args ...args)
{
	mt.lock();
	tasks.push(make_pair(
		f,
		make_tuple(args...)
	));
	cv.notify_one();
	mt.unlock();
}

template<class ...Args>
void Pool<void, Args...>::waitTasks()
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
		tuple<Args...> params = get<1>(tasks.front());
		tasks.pop();
		mt.unlock();
		apply(function, params);
	}
}

template<class ...Args>
Pool<void, Args...>*
Pool<void, Args...>::getInstance(int count)
{
	if (!p_instance)
		p_instance = new Pool<void, Args...>(count);
	return p_instance;
}


template<class ...Args>
int Pool<void, Args...>::size()
{
	return tasks.size();
}
