/*
*	Author:		Hang V Liang
*	Purpose:	Codes for drawing light source point for Programming Assignment 3. CSE 170 Computer Graphics Class at UC Merced
*/

#ifndef LIGHT_SOURCE_CPP
#define LIGHT_SOURCE_CPP

# include <vector>
# include <gsim/gs_color.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"
# include <math.h>
# include <iostream>

class LightSource : public GlObjects {
private:
	std::vector<GsVec>   P; // coordinates for Lines
	std::vector<GsColor> C; // colors
	int _numpoints;         // saves the number of points

public:
	LightSource();
	void init(const GlProgram& prog);
	void build(float x, float y, float z);

	void draw(GsMat& tr, GsMat& pr);
};

#endif // SO_WATCH_H
