#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <ctime> 
using namespace std;
class Matrix
{
	using V = vector<long long>;
public:
	using V2d = vector<V>;
	long long& operator() (int i, int j) ;
	int colSize() const;
	int rowSize() const;
	void generate_random_matrix(int i, int j, int limit);
	void write_to_file(string fileName);
	V2d extract();
	Matrix();
	Matrix(string name);
	void prepare(int i, int j);
private:
	V2d matrix;
};

