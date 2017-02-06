/*
 *	Author:		Hang V Liang
 *	Purpose:	Codes for implementing a 3-d Capsule for Programming Assignment 2. CSE 170 Computer Graphics Class at UC Merced
*/

# ifndef SO_CAPSULE_H
# define SO_CAPSULE_H
# define _USE_MATH_DEFINES

# include <vector>
# include <gsim/gs_color.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"
# include <math.h>

class SoCapsule : public GlObjects {
private:
	std::vector<GsVec>   P; // coordinates
	std::vector<GsColor> C; // colors
	int _numpoints;         // saves the number of points
	int _numfaces;			// saves the number of faces
	double x;				// Midpoint
	double y;				// Midpoint
	double z;				// Midpoint
	float len;				
	float rt;				 
	float rb;
	std::vector<double> dxz;	// Circle
	std::vector<double> dy;		// Layers

public:
	SoCapsule();
	void init(const GlProgram& prog);
	// len		= length of the "tube section" of the capsule along the vertical Y axis
	// rt		= radius of the top semi-sphere
	// rb		= radius of the bottom semi-sphere
	// nfaces	= number of faces approximating the curved faces 
	void build(float len, float rt, float rb, int nfaces);
	void draw(GsMat& tr, GsMat& pr);

	void drawTop();
	void drawMiddle();
	void drawBottom();
};

# endif // SO_CAPSULE_H