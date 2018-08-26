#include "core/common.h"
#include "core/vector.h"
#include "core/bbox.h"
#include "core/ray.h"
#include "core/transform.h"
#include "core/imageio.h"
using namespace rainbow;

int main(int argc, char *argv[]) {
	/*int height = 100;
	int width = 100;
	Image a(width, height);
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			*a(i,j) = RGBSpectrum(static_cast<Float>(i) / width, static_cast<Float>(j) / height, 1);
	a.save("first.png");*/

	int a = 3;
	auto f=[](int a, int b) -> int {return a + b; };
	auto g = [a](int b) mutable {a++; return a + b;  };
	auto h = [&]() {a++; };
	auto q = [a]() mutable -> bool {if (a>0) a--; return a > 0; };
	cout << a << endl;
	h();
	cout << a << endl;
	h();
	cout << a << endl;
	//cout << f(1, 5) << endl;
	//cout << a << endl;
	//cout << g(9) << endl;
	//cout << a << endl;
	//a = 100;
	//cout << g(9) << endl;
	//cout << g(9) << endl;

	std::vector<std::string> x{ "aaa","slongle","popoqqq","fuck","yuki"};

	/*sort(x.begin(), x.end(), 
		[] (const std::string &s1, const std::string &s2) -> bool
		{return s1 > s2; }
	);*/


	/*int SZ = 4;
	sort(x.begin(), x.end(),
		[SZ](const std::string &s1) -> bool
	{return s1.length() < SZ; }
	);*/

	for (const std::string &s : x) {
		cout << s << endl;
	}
	return 0;
}