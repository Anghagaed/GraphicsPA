/*
*	Author:		Hang V Liang
*	Purpose:	Codes for implementing a 3-d Stop Watch for Programming Assignment 3. CSE 170 Computer Graphics Class at UC Merced
*/

#ifndef SO_WATCH_H
#define SO_WATCH_H

#define PI 3.141592654

# include <vector>
# include <gsim/gs_color.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"
# include <math.h>
# include <iostream>

class SoWatch : public GlObjects {
private:
	std::vector<GsVec>   P; // coordinates for Lines
	std::vector<GsVec>   PP; // coordinates for Lines
	//std::vector<GsVec>   T;	// coordinates for Triangles
	std::vector<GsColor> C; // colors
	std::vector<GsColor> CC; // colors
	int _numpoints;         // saves the number of points
	float x, y, z;			// Circular center point of the stopwatch
	float r;				// Radius from center point -> edge of stopwatch
	std::vector<float> Mangles;	// 60 Angle goes from 0 to 2 PI
	std::vector<float> Sangles;	// 120 Angle goes from 0 to 2 PI

	void buildCircle();
	void buildTicky();
	void buildHand(int mqC, int sqC);
	void createParameters();
public:
	SoWatch();
	void init(const GlProgram& prog);
	void build(float r, int mqC, int sqC);

	void draw(GsMat& tr, GsMat& pr);
};

#endif // SO_WATCH_H
