#define _USE_MATH_DEFINES
#include "SoCapsule.h"
#include <math.h>
#include <iostream>
#include <vector>

using namespace std;

SoCapsule::SoCapsule()
{
	_numpoints = 0;
}

void SoCapsule::init(const GlProgram& prog)
{
	// Define buffers needed:
	// I have no idea what these lines do, but they look like I need them. Tehe XD
	set_program(prog);
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(2);       // will use 2 buffers: one for coordinates and one for colors
	uniform_locations(2); // will send 2 variables: the 2 matrices below
	uniform_location(0, "vTransf");
	uniform_location(1, "vProj");
}

// Draws Tube
void SoCapsule::drawTube(double x_l, double x_r, double z_r, double z_l, double height)
{
	// top triangle
	P.push_back(GsVec(x_r, height, z_r)); C.push_back(GsColor::lightblue);
	P.push_back(GsVec(x_l, height, z_l)); C.push_back(GsColor::lightblue);
	P.push_back(GsVec(x_r, -height, z_r)); C.push_back(GsColor::lightblue);

	// bot triangle
	P.push_back(GsVec(x_l, height, z_l)); C.push_back(GsColor::lightblue);
	P.push_back(GsVec(x_r, -height, z_r)); C.push_back(GsColor::lightblue);
	P.push_back(GsVec(x_l, -height, z_l)); C.push_back(GsColor::lightblue);
}

// Draws Circle
void SoCapsule::drawCircle(double x_l, double x_r, double z_r, double z_l, double height, int frame_num, double circle_rate, double radius, double curr_angle_r, double curr_angle_l)
{
	double radius_rate, old_x_r, old_x_l, old_z_l, old_z_r, new_x_r, new_x_l, new_z_r, new_z_l;
	for (int i = 0; i < frame_num; ++i)
	{
		// Update new radius
		radius_rate = radius * cos(circle_rate * (i + 1));
		radius = radius_rate;

		// side of rectanges in the circle
		old_x_r = x_r;
		old_x_l = x_l;
		new_x_r = radius * (double)sin(curr_angle_r);
		new_x_l = radius * (double)sin(curr_angle_l);

		// apothem of the rectangles in the tube section 
		old_z_r = z_r;
		old_z_l = z_l;
		new_z_r = radius * (double)cos(curr_angle_r);
		new_z_l = radius * (double)cos(curr_angle_l);

		// top triangle
		P.push_back(GsVec(new_x_r, height, new_z_r)); C.push_back(GsColor::lightgray);
		P.push_back(GsVec(new_x_l, height, new_z_l)); C.push_back(GsColor::lightgray);
		P.push_back(GsVec(old_x_r, height, old_z_r)); C.push_back(GsColor::lightgray);

		// bot triangle
		P.push_back(GsVec(new_x_l, height, new_z_l)); C.push_back(GsColor::lightgray);
		P.push_back(GsVec(old_x_r, height, old_z_r)); C.push_back(GsColor::lightgray);
		P.push_back(GsVec(old_x_l, height, old_z_l)); C.push_back(GsColor::lightgray);
	}
}

void SoCapsule::drawCapsule(int nfaces)
{
	// Variables
	double radius = 0.5; // default radius (for top and bot circles)
	double height = 0.5; // 1/2 height of the cylinder
	int frame_num = ((nfaces - 1) / 2); // # of frames for top and bot "circles"
	double circle_rate = (M_PI / 2.0) / frame_num; // rate of angle that changes the radius of "circle"

												   // (Degrees) * (Radian Conversion) = Radian
	double angle_rate = 360.0 / nfaces * M_PI / 180; // rate the triangle rotates
	double angle_l = -360.0 / nfaces * M_PI / 180 / 2; // default left side
	double angle_r = 360.0 / nfaces * M_PI / 180 / 2; // default right side

	for (int i = 0; i < nfaces; i++)
	{
		// side of the rectangles in the tube setion
		double x_r = radius * (double)sin(angle_r + (angle_rate * i));
		double x_l = radius * (double)sin(angle_l + (angle_rate * i));

		// apothem of the rectangles in the tube section
		double z_r = radius * (double)cos(angle_r + (angle_rate * i));
		double z_l = radius * (double)cos(angle_l + (angle_rate * i));

		drawTube(x_l, x_r, z_r, z_l, height); // draws tube
		drawCircle(x_l, x_r, z_r, z_l, height, frame_num, circle_rate, radius, angle_r + (angle_rate * i), angle_l + (angle_rate * i)); // draws top "circle"
		drawCircle(x_l, x_r, z_r, z_l, -height, frame_num, circle_rate, radius, angle_r + (angle_rate * i), angle_l + (angle_rate * i)); // draws bot "circle"
	}
}

void SoCapsule::build(int nfaces)
{
	if (nfaces < 3) // Error check
	{
		cout << "nfaces should not be less than 3. nfaces currently is " << nfaces << endl;
		return;
	}
	P.clear(); C.clear(); // set size to zero, just in case

	drawCapsule(nfaces); // draws "Capsule"

	// send data to OpenGL buffers:
	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, P.size() * 3 * sizeof(float), &P[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ARRAY_BUFFER, C.size() * 4 * sizeof(gsbyte), &C[0], GL_STATIC_DRAW);

	// save size so that we can free our buffers and later just draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.resize(0); C.resize(0);
}

void SoCapsule::draw(GsMat& tr, GsMat& pr)
{
	// Draw Triangles:
	// Still have no idea what these lines do. Derp! XD
	glUseProgram(prog);
	glBindVertexArray(va[0]);

	glBindBuffer(GL_ARRAY_BUFFER, buf[0]); // positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buf[1]); // colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);

	glUniformMatrix4fv(uniloc[0], 1, GL_FALSE, tr.e);
	glUniformMatrix4fv(uniloc[1], 1, GL_FALSE, pr.e);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, _numpoints);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

