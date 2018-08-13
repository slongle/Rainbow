#ifndef __FRAME_H
#define __FRAME_H

#include "vector.h"

RAINBOW_NAMESPACE_BEGIN

class Frame {
public:
	Frame(Normal3f _n):n(_n) {
		if (std::abs(n.x) > std::abs(n.y))
			s = Vector3f(-n.z, 0, n.x) / std::sqrt(n.z*n.z + n.x*n.x);
		else
			s = Vector3f(0, -n.z, n.y) / std::sqrt(n.z*n.z + n.y*n.y);
		t = Cross(n, s);
	}

	Normal3f n;
	Vector3f s, t;
};

RAINBOW_NAMESPACE_END

#endif // !__FRAME_H
