/*********************************************************************
 *  Author  : Himangshu Saikia
 *  Init    : Wednesday, September 20, 2017 - 12:04:15
 *
 *  Project : KTH Inviwo Modules
 *
 *  License : Follows the Inviwo BSD license model
 *********************************************************************
 */

#include <labtopo/integrator.h>
#include <labtopo/interpolator.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/image/imageram.h>
#include <inviwo/core/datastructures/volume/volumeram.h>

namespace inviwo {

Integrator::Integrator() {}

// TODO: Implementation of the functions defined in the header file integrator.h
vec2 Integrator::findzeropossibility(const Volume* vol, const vec2& position, float distance){
    vec2 current_point = position;
	vec2 returnvector = current_point;
    for(int i=0; i<2; i++){
        current_point.x = current_point.x+distance*i;
        current_point.y = position.y;
        for(int j=0; j<2; j++){
            current_point.y = current_point.y+distance*j;
            vec2 point00 = Interpolator::sampleFromField(vol, vec2(current_point.x,current_point.y));
            vec2 point10 = Interpolator::sampleFromField(vol, vec2(current_point.x+distance,current_point.y));
            vec2 point01 = Interpolator::sampleFromField(vol, vec2(current_point.x,current_point.y+distance));
            vec2 point11 = Interpolator::sampleFromField(vol, vec2(current_point.x+distance,current_point.y+distance));
            if((point00[0]>=0&&point10[0]>=0&&point01[0]>=0&&point11[0]>=0) || (point00[0]<=0&&point10[0]<=0&&point01[0]<=0&&point11[0]<=0) || (point00[1]>=0&&point10[1]>=0&&point01[1]>=0&&point11[1]>=0) || (point00[1]<=0&&point10[1]<=0&&point01[1]<=0&&point11[1]<=0))
            {}else{
                returnvector = current_point;
            }
        }
    }
	return returnvector;
}

vec2 Integrator::RK4(const Volume* vr, const vec2& currentPoint, float stepsize, bool directionfield)
{
	vec2 nextpoint;
	vec2 v1 = Interpolator::sampleFromField(vr, currentPoint);

	if (v1 == vec2(0, 0)) {
		nextpoint = vec2(0, 0); //Instructs downstream to break for loop
	}
	else if (!directionfield) {
		float step = stepsize / 2.0;
		vec2 v2 = Interpolator::sampleFromField(vr, currentPoint + step*v1);
		vec2 v3 = Interpolator::sampleFromField(vr, currentPoint + step*v2);
		vec2 v4 = Interpolator::sampleFromField(vr, currentPoint + step*v3);
		vec2 vaverage = stepsize*(v1 / float(6.0) + v2 / float(3.0) + v3 / float(3.0) + v4 / float(6.0));

		nextpoint = currentPoint + vaverage;
	}
	else {
		float vlength = sqrt((v1[0] * v1[0]) + (v1[1] * v1[1]));
		v1 = v1 / vlength;

		float step = stepsize / 2.0;

		vec2 v2 = Interpolator::sampleFromField(vr, currentPoint + step*v1);
		vlength = sqrt((v2[0] * v2[0]) + (v2[1] * v2[1]));
		if (vlength != 0)
			v2 = v2 / vlength;

		vec2 v3 = Interpolator::sampleFromField(vr, currentPoint + step*v2);
		vlength = sqrt((v3[0] * v3[0]) + (v3[1] * v3[1]));
		if (vlength != 0)
			v3 = v3 / vlength;

		vec2 v4 = Interpolator::sampleFromField(vr, currentPoint + step*v3);
		vlength = sqrt((v4[0] * v4[0]) + (v4[1] * v4[1]));
		if (vlength != 0)
			v4 = v4 / vlength;

		vec2 vaverage = stepsize*(v1 / float(6.0) + v2 / float(3.0) + v3 / float(3.0) + v4 / float(6.0));

		nextpoint = currentPoint + vaverage;
	}

	return nextpoint;
}

void Integrator::boxkernalpixel(const Volume* vr, const ImageRAM* tr, const vec2& currentPoint, const vec2& scale, float stepsize, int numberofsteps, float minArcLength, bool directionfield, int** PixelArray) {

	//List of Vec3s that store position and pixel data, begin with the pixel in the current position
	std::list<vec3> listofPixels;
	listofPixels.push_back(vec3(currentPoint.x, currentPoint.y, Interpolator::sampleFromGrayscaleImage(tr, currentPoint)));

	//Jane: Using RK4 method to draw stream lines
	//Hara: currentPoint is the position on the image matrix, but this must ba adjusted for the dimensions of the vector field.
	vec2 currentPoint_RK4 = currentPoint*scale;
	vec2 currentPoint_image = currentPoint;

	//Jane: if postion is outside of domain, ampleFromField function will return vec2[0,0]
	//      In the meantime, if vector is [0,0], integration should stop
	vec2 StopPoint = vec2(0, 0);

	vec2 priorPoint = currentPoint_RK4; // Needed to calculate length for velocity check.
	for (int i = 0; i < 100; i++) {
		currentPoint_RK4 = Integrator::RK4(vr, currentPoint_RK4, stepsize, directionfield);
		currentPoint_image = currentPoint_RK4 / scale;
		double steppixel = Interpolator::sampleFromGrayscaleImage(tr, currentPoint_image); //divide by scale to return to image coordinate system
		if (steppixel == -1.0) break; //streamline has gone beyond bounds of image
		listofPixels.push_back(vec3(currentPoint_image.x, currentPoint_image.y, steppixel));
		if (Integrator::arclength(priorPoint, currentPoint_RK4) < minArcLength) break;//break loop if the velocity is too slow
		priorPoint = currentPoint_RK4; //Set the priorpoint for the next loop (remember the prior point of the current loop was the current point at the end of previous loop.

									   //Jane: Stop integration at the boundry of domain where currentPoint_RK4 == vec2[0,0]
									   //      as well as at the point where vector is [0,0]
		if (currentPoint_RK4 == StopPoint) break;
	}

	//Repeat Entire Code With Modifications to Handle Backward Propogation

	//Hara: Reset Variables
	currentPoint_RK4 = currentPoint*scale;
	priorPoint = currentPoint_RK4;
	currentPoint_image = currentPoint;

	for (int i = 0; i < 100; i++) {
		currentPoint_RK4 = Integrator::RK4(vr, currentPoint_RK4, -1 * stepsize, directionfield);
		currentPoint_image = currentPoint_RK4 / scale;
		double steppixel = Interpolator::sampleFromGrayscaleImage(tr, currentPoint_image); //divide by scale to return to image coordinate system
		if (steppixel == -1.0) break; //streamline has gone beyond bounds of image
		listofPixels.push_front(vec3(currentPoint_image.x, currentPoint_image.y, steppixel));
		if (Integrator::arclength(priorPoint, currentPoint_RK4) < minArcLength) break;//break loop if the velocity is too slow
		priorPoint = currentPoint_RK4; //Set the priorpoint for the next loop (remember the prior point of the current loop was the current point at the end of previous loop.

									   //Jane: Stop integration at the boundry of domain where currentPoint_RK4 == vec2[0,0]
									   //      as well as at the point where vector is [0,0]
		if (currentPoint_RK4 == StopPoint) break;
	}

	auto position = listofPixels.begin();
	while (position != listofPixels.end()) {
		auto calculatingposition = position;
		double pixel = 0.0; //sumation for box kernal
		int size = 0; //number of values in pixel

					  //Sum Pixels in Forward Propogation
		for (int a = 0; a <= numberofsteps; a++) { //equal sign so it takes into account the pixel itself
			vec3 pixelhere = *calculatingposition;
			pixel += pixelhere.z;
			size++;
			if (calculatingposition == listofPixels.end()) break;
			calculatingposition++;
		}

		//Reset Calculating Position to Be One Step Before Current Pixel
		calculatingposition = position;

		if (calculatingposition != listofPixels.begin()) {
			calculatingposition--;

			//Sum Pixels in Backward Propogation
			for (int b = 0; b < numberofsteps; b++) {
				vec3 pixelhere = *calculatingposition;
				pixel += pixelhere.z;
				size++;
				if (calculatingposition == listofPixels.begin()) break;
				calculatingposition--;
			}
		}

		//Now assign value to PixelArray by Dividing the value of Pixel With Number of Steps
		vec3 pixelhere = *position;
		int x = pixelhere.x;
		int y = pixelhere.y;
		PixelArray[x][y] = pixel / size;
		position++;
	}

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
