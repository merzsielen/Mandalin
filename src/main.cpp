/*
	main.cpp

	The unmoved mover....
*/

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "rendering/camera.h"

#define VERSION 0.01

void main()
{
	/*
		Let's get some meta-details straight.
		Mandalin version, OpenGL version, etc.
	*/
	std::ostringstream o;
	o << std::setprecision(2) << std::noshowpoint << VERSION;
	std::cout << "Running Mandalin, version: " + o.str() + "." << std::endl;

	/*
		Now we're going to fire up rendering:
		camera, renderer, etc.
	*/
	Mandalin::Camera* camera = new Mandalin::Camera({ 0, 0, 0 }, { 1, 0, 0, 0 }, 1.0f);
}