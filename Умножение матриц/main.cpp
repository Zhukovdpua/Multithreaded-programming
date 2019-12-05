#include <vector>
#include <iostream>
using namespace std;
#include "Matrix.h"
#include "MultapleMatrix.h"

#include <chrono> // дл€ функций из std::chrono

class Timer
{
private:
	// ѕсевдонимы типов используютс€ дл€ удобного доступа к вложенным типам
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<clock_t> m_beg;

public:
	Timer() : m_beg(clock_t::now())
	{
	}

	void reset()
	{
		m_beg = clock_t::now();
	}

	double elapsed() const
	{
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
};

using V = vector<long long>;
using V2d = vector<V>;

long long vs(const V& a, const V2d& b, const int fixCol) {
	long long sum = 0;
	for (int i = 0; i < a.size(); i++) 
		sum += a[i] * b[i][fixCol];
	return sum;
}

V2d matrixMultaple(const V2d& a, const V2d& b) {
	if (a[0].size() != b.size())
		throw 1;
	V2d result(a.size(), V(b[0].size()));
	for (int i = 0; i < a.size(); i++) {
		for (int j = 0; j < b[0].size(); j++) {
			result[i][j] = vs(a[i], b, j);
		}
	}
	return result;
}

int main() {
	srand(time(0));
	//Matrix a("c.txt"), b("d.txt");
	//auto av = a.extract();
	//auto bv = b.extract();

	//matrixMultaple(av, bv);

	//Matrix a;
	//a.generate_random_matrix(700, 700, 1000);
	//a.write_to_file("d.txt");

	Matrix a("a.txt"), b("b.txt");
	MultapleMatrix mm;

	Timer t;
	auto f = mm.multaple(a, b);
	auto w = f.get();
	std::cout << "Time elapsed: " << t.elapsed() << '\n';

	system("pause");
}
