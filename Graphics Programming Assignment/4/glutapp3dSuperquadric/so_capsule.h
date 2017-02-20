/*
*	Author:		Hang V Liang
*	Purpose:	Codes for implementing a 3-d Capsule for Programming Assignment 2. CSE 170 Computer Graphics Class at UC Merced
*/

# ifndef SO_CAPSULE_H
# define SO_CAPSULE_H
# define _USE_MATH_DEFINES

# include <gsim/gs_color.h>
# include <gsim/gs_array.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"
# include <math.h>
# include <vector>

class SoCapsule : public GlObjects {
private:
	// Parameters for building
	GsArray<GsVec>   P; // coordinates
	GsArray<GsColor> C; // colors
	int _numpoints;         // saves the number of points
	int _numfaces;			// saves the number of faces
	double x, y, z;			// midpoints
	float len, rt, rb;		
	std::vector<double> dx, dy, dz;

	// Functions for building
	void buildTop();
	void buildMiddle();
	void buildBottom();
	void calculateParameters();

	// Gl Objects for shaders and drawing
	GlShader _vsh, _fsh;
	GlShader _vshgou, _fshgou, _vshphong, _fshphong;
	GlProgram _proggouraud, _progphong;
	GlProgram _prog;

public:
	SoCapsule();
	void init();
	// len		= length of the "tube section" of the capsule along the vertical Y axis
	// rt		= radius of the top semi-sphere
	// rb		= radius of the bottom semi-sphere
	// nfaces	= number of faces approximating the curved faces 
	void build(float len, float rt, float rb, int nfaces);
	void draw(GsMat& tr, GsMat& pr);
};

# endif // SO_CAPSULE_H