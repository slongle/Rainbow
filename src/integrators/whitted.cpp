#include "whitted.h"

RAINBOW_NAMESPACE_BEGIN	

void WhittedIntegrator::Render(const Scene & scene) {	
	std::shared_ptr<Film> film = camera->film;
	Ray ray;
	
	std::cout << scene.aggregate->primitives.size() << std::endl;	
	//cout << scene.lights.size() << endl;
	
	//cout << camera->CameraToWorld << endl;

	/*for (auto prim : scene.aggregate->primitives) {
		cout << prim->shape->Area() << endl;
	}*/

	/*
	auto a = scene.aggregate;
	Bounds3f bound = a->Bounds();
	cout << bound << endl;
	cout << bound.Center() << endl;
	cout << bound.Diagonal() << endl;
	exit(0);*/

	for (int y = 0; y < film->resolution.y; y++) {
		for (int x = 0; x < film->resolution.x; x++) {

			/*
			camera->GenerateRay(&ray,
				Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5) + sampler->Get2D());
			if (scene.IntersectP(ray)) {
				film->SetPixel(Point2i(x, y), RGBSpectrum(1, 1, 1));
			}
			else
				film->SetPixel(Point2i(x, y), RGBSpectrum(0, 0, 0));

			continue;
			*/
			
			RGBSpectrum L(0.0);
			int SampleNum = 50;
			for (int i = 0; i < SampleNum; i++) {
				camera->GenerateRay(&ray,
					Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5) + sampler->Get2D());
				L += Li(ray, scene, 0);
			}
			L /= SampleNum;
			film->SetPixel(Point2i(x, y), L);
		}
		std::cout << y << std::endl;
	}
	film->SaveImage();
}

RGBSpectrum WhittedIntegrator::Li(const Ray & ray, const Scene & scene, int depth) {
	RGBSpectrum L(0.0);
	SurfaceInteraction intersection;
	if (!scene.Intersect(ray, &intersection)) {
		return L;
	}

	//if (((scene.aggregate->primitives[5]).get() != intersection.primitive) && ((scene.aggregate->primitives[4]).get() != intersection.primitive)){
	//	return L;
	//}


	//std::cout << "Hit!" << std::endl;
	Vector3f wo = intersection.wo;
	Normal3f n = intersection.n;

	/*Direct Light*/
	L += intersection.Le(wo);
	
	intersection.ComputeScatteringFunctions();
	if (!intersection.bxdf) {
		//cout << "HH!" << endl;
		return L;
	}

	for (auto light : scene.lights) {
		Vector3f wi;
		Float pdf;
		RGBSpectrum Li = light->SampleLi(intersection, sampler->Get2D(), &wi, &pdf);
		if (Li.IsBlack() || pdf == 0) continue;		
		RGBSpectrum f = intersection.bxdf->f(wo, wi);
		//return f;
		if (!f.IsBlack()) {
			//cout << light << endl;
			//cout << f << endl;
			//cout << pdf << endl;
			//cout << (f * Li*AbsDot(wi, n) / pdf) << endl;
			
			//cout << AbsDot(wi, n) << endl;

			L += f * Li * AbsDot(wi, n) / pdf;			
		}
	}

	//exit(0);


	if (depth + 1 < maxDep) {
		L += SpecularReflect(ray, scene, depth, intersection);
		//L += SpecularRefract(ray, scene, depth, intersection);
	}

	

	delete(intersection.bxdf);
	
	//cout << L << endl;
	
	return L;
}

RGBSpectrum WhittedIntegrator::SpecularReflect
(const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
	Vector3f wo = intersection.wo, wi;
	Float pdf;

	RGBSpectrum f = intersection.bxdf->sampleF(wo, &wi, sampler->Get2D(), &pdf);
	Normal3f n = intersection.n;	

	//return f;

	if (pdf > 0.f && !f.IsBlack() && Dot(n, wi) != 0.f) {
		Ray r = intersection.SpawnRay(wi);
	
		//return f;
		
		return f * Li(r, scene, depth + 1)*AbsDot(wi,n) / pdf;
	}
	else
		return RGBSpectrum(0.f);
}

RGBSpectrum WhittedIntegrator::SpecularRefract
(const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
	return RGBSpectrum();
}

WhittedIntegrator* CreateWhittedIntegrator(const PropertyList &list) {
	return new WhittedIntegrator(1);
}

RAINBOW_NAMESPACE_END