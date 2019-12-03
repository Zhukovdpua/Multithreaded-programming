#include <string>
#include "ThreadPool.h"
#include "ThreadPoolVoid.h"

using ulli = unsigned long long int;

ulli tsk1(int a, int b) {
	int sum = 0;
	for (int i = a; i < b; i++) 
		sum += i;
	return sum;
}

ulli tsk2(int a, int b) {
	unsigned long long int mult = 1;
	for (int i = a; i < b; i++)
		mult *= i;
	return mult;
}

void tsk3(string s) {
	this_thread::sleep_for(chrono::seconds(2));
	cout << s << endl;
}

void tsk4(double a, string s){
	this_thread::sleep_for(chrono::seconds(3));
	cout << a << " " << s << endl;
}


void test() {
	auto poolObject1 = make_pool<ulli, int, int>();
	auto poolObject2 = make_pool<ulli, int, int>(1); //не создаст пул, т.к уже существует пул с такой сигнатурой
	auto poolObject3 = make_pool<void, string>(2);
	auto poolObject4 = make_pool<void, double, string>(1);

	auto return_from_pool1 = poolObject1->addTask(tsk1, 1, 1000); // пул возвращает std::future, если возвращаемый тип не void
	auto return_from_pool2 = poolObject1->addTask(tsk2, 56, 91);
	poolObject3->addTask(tsk3, "tsk3");

	cout << return_from_pool1.get() << endl;

	poolObject4->addTask(tsk4, 4.698, "tsk4");

	cout << return_from_pool2.get() << endl;

}


int main() {
	test();
	cout << "\ntest ended\n";
	return 0;
}

