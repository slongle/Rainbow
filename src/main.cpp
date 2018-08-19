#include "core/common.h"
#include "core/vector.h"
#include "core/bbox.h"
#include "core/ray.h"
#include "core/transform.h"
using namespace rainbow;

using std::cin;
using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
	Bounds2f a;
	cout << a.toString() << endl;
	Matrix4x4 b;
	cout << b.toString() << endl;
	Point2f c;
	cout << c.toString() << endl;
	return 0;
}