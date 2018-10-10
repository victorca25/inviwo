/*********************************************************************
*  Author  : Himangshu Saikia
*  Init    : Wednesday, September 20, 2017 - 12:04:15
*
*  Project : KTH Inviwo Modules
*
*  License : Follows the Inviwo BSD license model
*********************************************************************
*/

#include "integrator.h"

namespace inviwo {

	Integrator::Integrator() {}

	// TODO: Implementation of the functions defined in the header file integrator.h

	vec2 Integrator::sampleFromField(const Volume * vol, const vec2 & position) {

		auto vr = vol->getRepresentation<VolumeRAM>();
		auto dims = vr->getDimensions();

		auto base = vol->getBasis();
		vec2 cellSize(base[0][0] / dims[0], base[1][1] / dims[1]);

		// Sampled outside the domain!
		if (position[0] < 0 || position[0] > dims[0] - 1 || position[1] < 0 ||
			position[1] > dims[1] - 1) {
			return vec2(0, 0);
		}
		size2_t p((size_t)position[0], (size_t)position[1]);

		// Leads to accessing only inside the volume
		// Coefficients computation takes care of using the correct values
		for (int d = 0; d < 2; ++d) p[d] = std::min(p[d], dims[d] - 2);

		const auto f00 = vr->getAsDVec2(size3_t(p[0], p[1], 0));
		const auto f10 = vr->getAsDVec2(size3_t(p[0] + 1, p[1], 0));
		const auto f01 = vr->getAsDVec2(size3_t(p[0], p[1] + 1, 0));
		const auto f11 = vr->getAsDVec2(size3_t(p[0] + 1, p[1] + 1, 0));

		const float x = position[0] - p[0];
		const float y = position[1] - p[1];

		vec2 f;

		for (int i = 0; i < 2; i++) {
			f[i] = f00[i] * (1 - x) * (1 - y) + f01[i] * (1 - x) * y + f10[i] * x * (1 - y) +
				f11[i] * x * y;

			// Bring vector back to grid space.
			f[i] /= cellSize[i];
		}

		return f;
	}

	vec2 Integrator::RK4(const Volume* vr, const vec2& currentPoint, float stepsize, bool directionfield)
	{
		vec2 nextpoint;
		vec2 v1 = Integrator::sampleFromField(vr, currentPoint);

		if (v1 == vec2(0, 0)) {
			nextpoint = vec2(0, 0); //Instructs downstream to break for loop
		}
		else if (!directionfield) {
			float step = stepsize / 2.0;
			vec2 v2 = Integrator::sampleFromField(vr, currentPoint + step*v1);
			vec2 v3 = Integrator::sampleFromField(vr, currentPoint + step*v2);
			vec2 v4 = Integrator::sampleFromField(vr, currentPoint + step*v3);
			vec2 vaverage = stepsize*(v1 / float(6.0) + v2 / float(3.0) + v3 / float(3.0) + v4 / float(6.0));

			nextpoint = currentPoint + vaverage;
		}
		else {
			float vlength = sqrt((v1[0] * v1[0]) + (v1[1] * v1[1]));
			v1 = v1 / vlength;

			float step = stepsize / 2.0;

			vec2 v2 = Integrator::sampleFromField(vr, currentPoint + step*v1);
			vlength = sqrt((v2[0] * v2[0]) + (v2[1] * v2[1]));
			if (vlength != 0)
				v2 = v2 / vlength;

			vec2 v3 = Integrator::sampleFromField(vr, currentPoint + step*v2);
			vlength = sqrt((v3[0] * v3[0]) + (v3[1] * v3[1]));
			if (vlength != 0)
				v3 = v3 / vlength;

			vec2 v4 = Integrator::sampleFromField(vr, currentPoint + step*v3);
			vlength = sqrt((v4[0] * v4[0]) + (v4[1] * v4[1]));
			if (vlength != 0)
				v4 = v4 / vlength;

			vec2 vaverage = stepsize*(v1 / float(6.0) + v2 / float(3.0) + v3 / float(3.0) + v4 / float(6.0));

			nextpoint = currentPoint + vaverage;
		}

		return nextpoint;
	}

	//Hara: arc length calculation
	double Integrator::arclength(const vec2 & position, const vec2 & position2)
	{
		double a = position.x - position2.x;
		double b = position.y - position2.y;
		double c = sqrt((a*a) + (b*b));
		return c;
	}

}  // namespace inviwo
