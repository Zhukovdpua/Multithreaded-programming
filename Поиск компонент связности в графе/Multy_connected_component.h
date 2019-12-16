#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <future>
#include <unordered_set>
using namespace std;

class Multy_connected_component {
private:
	class Pool {
	private:
		unordered_set<int> tasks;
		vector<thread> threads;
		condition_variable_any cv;
		mutex mt;
		void waitTasks(const int thread_count, promise<int>* promise, condition_variable_any& end);
		Multy_connected_component* g;
		atomic<int> ready_thread_count;
	public:
		void construct_Pool(int count, Multy_connected_component* g, promise<int>* promise, condition_variable_any& end);
		Pool() {};
		~Pool();
		void addTask(int vertex);
		int size();
		int get_ready_thread_count();
	};
	Pool* pool;

	vector<atomic<bool>> alr;
	vector<vector<int>> adj_list;
	condition_variable_any cv;
	void dfs(int vertex); 
	atomic<int> start;
	vector<int> smallests;
	void set_start(int _start);
	int get_start();
	void push_to_smallests(int index);
	int smallests_size();
	int get_smallest();
	int isAll(int i, int step);
	int vertex_count();
	bool end;

public:
	future<int> compute(int thread_count);
	Multy_connected_component(int vertex_count, string fileName);
	~Multy_connected_component();
};

