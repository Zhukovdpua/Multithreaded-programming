#include "MultapleMatrix.h"

long long MultapleMatrix::vectorMultaple(Matrix& a, Matrix& b, int row, int col) {
	long long sum = 0;
	for (int i = 0; i < b.rowSize(); i++)
		sum += a(row, i) * b(i, col);
	return sum;
}

void MultapleMatrix::compute(Matrix& a, Matrix& b, int thread_count) {
	while (true) {
		int index = count.fetch_add(1);
		int i = index / b.colSize();
		if (i >= a.rowSize())
			break;
		int j = index % b.colSize();

		result(i, j) = vectorMultaple(a, b, i, j);
	}
	if (thread_ended.fetch_add(1) == thread_count)
		p.set_value(result);
}

future<Matrix> MultapleMatrix::multaple(Matrix & a, Matrix & b, int thread_count) {
	count = 0;
	thread_ended = 1;
	result.prepare(a.rowSize(), b.colSize());
	future<Matrix> ret = p.get_future();
	for (int i = 0; i < thread_count; i++)
		threads.push_back(thread(&MultapleMatrix::compute, this, ref(a), ref(b), thread_count));
	return ret;
}

MultapleMatrix::~MultapleMatrix() {
	for (auto& i : threads)
		i.join();
}
