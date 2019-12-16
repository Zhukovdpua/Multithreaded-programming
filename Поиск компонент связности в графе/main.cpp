#include <iostream>
#include <fstream>
#include "Multy_connected_component.h"


int main() {
	ifstream in;
	in.open("graph.txt");
	string s;
	in >> s;
	int tof = stoi(s);
	in.close();

	Multy_connected_component g(tof, "graph.txt");
	auto future = g.compute(thread::hardware_concurrency() - 1);
	cout << endl << "Connected component count: " << future.get() << endl;
	return 0;
}
