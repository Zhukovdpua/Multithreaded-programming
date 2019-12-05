#include "Matrix.h"

Matrix::Matrix()
{
}

Matrix::Matrix(string name)
{
	string s;
	ifstream in;
	in.open(name);
	in >> s;
	int row = stoi(s);
	in >> s;
	int col = stoi(s);
	V v;
	int k = 0;
	while (!in.eof()) {
		if (k++ < col) {
			in >> s;
			v.push_back(stoi(s));
		}
		else {
			matrix.push_back(move(v));
			k = 0;
		}
	}
	matrix.push_back(move(v));
	in.close();
}

void Matrix::prepare(int i, int j) {
	matrix.resize(i);
	for (auto& i : matrix) 
		i.resize(j);
}

long long& Matrix::operator()(int i, int j)  {
	return matrix[i][j];
}

int Matrix::colSize() const {
	return matrix[0].size();
}

int Matrix::rowSize() const {
	return matrix.size();
}

void Matrix::generate_random_matrix(int i, int j, int limit) {
	prepare(i, j);
	for (auto& i : matrix) {
		for (auto& j : i)
			j = 1 + rand() % limit;
	}
}

void Matrix::write_to_file(string fileName) {
	ofstream file;
	file.open(fileName);
	file << colSize() << " " << rowSize() << endl;
	for (auto& i : matrix) {
		for (auto& j : i)
			file << j << " ";
		file << endl;
	}
	file.close();
}

Matrix::V2d Matrix::extract() {
	return matrix;
}
