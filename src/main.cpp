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
	cout << 1 / 2 << endl;
	Float f = 1 / 2;
	cout << f << endl;
	Float e = 0;
	cout << 1 / e << endl;
	Bounds3f a(Point3f(1,4,3),Point3f(9,1,7));
	cout << a.toString() << endl;
	Matrix4x4 b(1,2,3,7,
		        29,23,5,2,
		        13,17, 19,11,
		        0,0,0,1);
	cout << b.toString() << endl;
	cout << Inverse(b) << endl;
	Vector3f c;
	cout << c.toString() << endl;	
	Transform d(b);//= Translate(Vector3f(1, 2, 3)) * RotateX(90);
	cout << d(a) << endl;
	return 0;
}