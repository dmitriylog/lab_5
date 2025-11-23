/*

#include <iostream>
#include <string>

using namespace std;

// Операция возведения в любую степень (не только квадрат) 
//                  4          5

*/

float Degree(float num, float deg) {
	int k = 1; float sum = num;
	while (k < deg) {
		sum *= num;
		k += 1;
	}
	return sum;
}

/*
* 
int main() {
	setlocale(LC_ALL, "RU");

	float a, b;
	cout << "Input number: "; cin >> a;// cout << endl;
	cout << "Input degree: "; cin >> b;// cout << endl;
	cout << Degree(a, b);

	return 0;
} 

*/