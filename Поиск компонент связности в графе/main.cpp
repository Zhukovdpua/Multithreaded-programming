#include <iostream>
#include <fstream>
#include "Multy_connected_component.h"


int main() {
	string name = "graph.txt";
	ifstream in;
	in.open(name);
	string s;
	in >> s;
	int tof = stoi(s);
	in.close();

	Multy_connected_component g(tof, name);
	auto future = g.compute(thread::hardware_concurrency() - 1);
	cout << endl << "Connected component count: " << future.get() << endl;
	return 0;
}