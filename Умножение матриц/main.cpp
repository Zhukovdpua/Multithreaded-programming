#include "Matrix.h"
#include "MultapleMatrix.h"
using namespace std;

int main() {
	srand(time(0));
	Matrix a("a.txt"), b("b.txt");
	MultapleMatrix mm;
	auto future = mm.multaple(a, b);
	auto result = future.get();
}
