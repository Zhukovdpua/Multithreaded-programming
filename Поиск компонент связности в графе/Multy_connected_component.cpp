#pragma once
#include "Multy_connected_component.h"
#include <fstream>
#include <algorithm>
#include <stack>
#include <iostream>

void Multy_connected_component::dfs(int start) {
	stack<pair<int, int>> st;
	st.push(
		make_pair(start, 0)
	);
	while (true) {
		if (alr[st.top().first].exchange(true)) {
			st.pop();
			if (st.size() == 0)
				break;
		}

		while (true) {
			bool New = false;
			for (int self = st.top().second, vertex = st.top().first; self < adj_list[vertex].size();) {
				while (
					self < adj_list[vertex].size() &&
					alr[adj_list[vertex][self]].load()
					)
					self++;
				if (self >= adj_list[vertex].size()) 
					break;
				int ready_thread_count = pool->get_ready_thread_count();
				int another = self + 1;
				bool is_ready_thread = false;
				while (ready_thread_count--) {
					is_ready_thread = true;
					while (
						another < adj_list[vertex].size() &&
						alr[adj_list[vertex][another]]
						)
						another++;
					if (another < adj_list[vertex].size())
						pool->addTask(adj_list[vertex][another]);
					else
						break;
					another++;
					if (pool->get_ready_thread_count() == 0)
						break;
				}
				if (is_ready_thread)
					st.top().second = another;
				else
					st.top().second = self + 1;
				st.push(
					make_pair(adj_list[vertex][self], 0)
				);
				New = true;
				break;
			}
			if (New)
				break;
			st.pop();
			if (st.size() == 0)
				break;
		}
		if (st.size() == 0)
			break;
	}
}

void Multy_connected_component::push_to_smallests(int index) {
	smallests.push_back(index);
}

int Multy_connected_component::smallests_size() {
	return smallests.size();
}

int Multy_connected_component::get_smallest() {
	int smallest = *(min_element(smallests.begin(), smallests.end()));
	smallests.resize(0);
	smallests.clear();
	return smallest;
}

int Multy_connected_component::isAll(int i, int step) {
	for (; i < alr.size(); i += step) {
		if (alr[i].load() == false)
			return i;
	}
	return i;
}

int Multy_connected_component::vertex_count() {
	return adj_list.size();
}

future<int> Multy_connected_component::compute(int thread_count) {
	end = false;
	pool = new Pool;
	start.store(0);
	pool->addTask(0);
	promise<int>* _promise = new promise<int>;
	future<int> future = _promise->get_future();
	pool->construct_Pool(thread_count, this, _promise, cv);
	return future;
}

Multy_connected_component::Multy_connected_component(int vertex_count, string fileName) : alr(vertex_count), end(true) {
	ifstream in;
	in.open(fileName);
	string s;
	in >> s;
	for (int i = 0, size = stoi(s); i < size; i++) {
		adj_list.emplace_back();
		while (in >> s) {
			if (s == "n")
				break;
			adj_list[i].push_back(stoi(s));
		}
	}
}

Multy_connected_component::~Multy_connected_component() {
	mutex mt;
	mt.lock();
	cv.wait(mt, [this]() {
		return end;
	});
	mt.unlock();
	delete pool;
}

int Multy_connected_component::get_start() {
	return start.fetch_add(1);
}

void Multy_connected_component::set_start(int _start) {
	start.store(_start);
}

void Multy_connected_component::Pool::waitTasks(const int thread_count, promise<int>* promise, condition_variable_any& end) {
	static int connection_count = 1;
	static atomic<int> thread_to_end;
	static bool end_flag = false;
	while (true) {
		while (true) {
			mt.lock();
			cv.wait(mt, [thread_count, this]() {
				if ((ready_thread_count != thread_count) && size() == 0)
					return false;
				else {
					ready_thread_count--;
					return true;
				}
			});
			if (tasks.size() == 0) {
				ready_thread_count++;
				if (thread_to_end.fetch_add(1) == 0)
					cv.notify_all();
				mt.unlock();
				break;
			}
			int vertex = *(tasks.begin());
			tasks.erase(tasks.begin());
			mt.unlock();
			g->dfs(vertex);
			ready_thread_count++;
		}

		int index = g->isAll(g->get_start(), thread_count);
		cout << " "; // Без этой строки в Release сборке при более чем одном потоке программа может работать не корректно, в том числе и аварийно завершится. Почему не понятно.
		mt.lock();
		g->push_to_smallests(index);
		if (g->smallests_size() == thread_count) {		
			thread_to_end.store(0);
			mt.unlock();
			int smallest = g->get_smallest();
			g->set_start(smallest);
			if (smallest >= g->vertex_count()) {
				promise->set_value(connection_count);
				delete promise;
				if (thread_count == 1) {
					g->end = true;
					end.notify_one();
				}
				else {
					end_flag = true;
					cv.notify_all();
				}
				break;
			}
			connection_count++;
			addTask(smallest);
			cv.notify_all();
		}
		else {
			cv.wait(mt);
			mt.unlock();
			if (end_flag) {
				if (thread_to_end.fetch_add(1) == thread_count - 2) {
					g->end = true; 
					end.notify_one();
				}
				break;
			}
		}
	}
}

Multy_connected_component::Pool::~Pool() {
	for (auto& i : threads)
		i.join();
}

void Multy_connected_component::Pool::construct_Pool(int count, Multy_connected_component* _g, promise<int>* promise, condition_variable_any& end) {
	ready_thread_count = count;
	g = _g;
	for (int i = 0; i < count; i++)
		threads.emplace_back(&Pool::waitTasks, this, count, promise, ref(end));
}

void Multy_connected_component::Pool::addTask(int vertex) {
	mt.lock();
	tasks.insert(vertex);
	cv.notify_one(); 
	mt.unlock();
}

int Multy_connected_component::Pool::size() {
	return tasks.size();
}

int Multy_connected_component::Pool::get_ready_thread_count() {
	return ready_thread_count.load();
}
