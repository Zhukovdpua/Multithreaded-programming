#pragma once
#include "Matrix.h"
#include <atomic>
#include <future>
#include <thread>

class MultapleMatrix
{
private:
	atomic<int> count;
	atomic<int> thread_ended;
	promise<Matrix> p;
	Matrix result;
	vector<thread> threads;
	long long vectorMultaple(Matrix& a, Matrix& b, int row, int col);
	void compute(Matrix& a, Matrix& b, int thread_count = thread::hardware_concurrency() - 1);

public:
	future<Matrix> multaple(Matrix& a, Matrix& b, int thread_count = thread::hardware_concurrency() - 1);
	~MultapleMatrix();
};

