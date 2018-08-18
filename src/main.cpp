#include "core/common.h"
#include "core/vector.h"
#include "core/ray.h"
#include "core/transform.h"
using namespace rainbow;

using std::cin;
using std::cout;
using std::endl;

int f(int a) {
	return a * a;
}

template<typename T>
std::ostream &print(std::ostream &os, const T &t) {
	os << t;
	return os;
}

template<typename T,typename... Args>
std::ostream &print(std::ostream &os, const T &t, const Args& ... rest) {
	os << t << '\t';
	return print(os, f(rest) ...);
}

template<typename T>
void myPrint(const T &t) {
	tfm::printf("hhhhh %s\n", t);
}

int main(int argc, char *argv[]) {
	//myPrint(3);
	//print(cout, 1, 2, 3, 4);
	//myPrint(a);
	Matrix4x4 a;
	cin >> a;
	cout << a.toString() << endl;
	cout << Inverse(a).toString() << endl;
	return 0;
}