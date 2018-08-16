#include "core/common.h"
#include "core/vector.h"
#include "core/ray.h"
#include "core/transform.h"
using namespace rainbow;

using std::cin;
using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
	Matrix4x4 a;
	cin >> a;
	cout << a << endl;
	cout << Inverse(a) << endl;
	return 0;
}