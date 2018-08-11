#include "core/common.h"
#include "core/vector.h"
#include "core/ray.h"
using namespace rainbow;

using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
	Vector3f a(1, 2, 3);
	Point3f b(4, 3, 2);
	Ray c(b, a);
	cout << c(3) << endl;
	cout << c << endl;
	return 0;
}