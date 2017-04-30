// Ensure the header file is included only once in multi-file projects
#ifndef SO_CAPSULE_H
#define SO_CAPSULE_H

// Include needed header files
# include <vector>
# include <gsim/gs_color.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"

// The capsule is divided in three parts: the tube section, the top semi-sphere, 
// and the bottom semi-sphere. Parameter len tells the length of the tube section.
class SoCapsule : public GlObjects
{
private:
	//*** The following variables may change... ***//
	std::vector<GsVec>   P; // coordinates
	std::vector<GsColor> C; // colors
	int _numpoints;         // saves the number of points
	void drawCapsule(int faces);
	void drawTube(double x_l, double x_r, double z_r, double z_l, double height);
	void drawCircle(double x_l, double x_r, double z_r, double z_l, double height, int frame_num, double circle_rate, double radius, double curr_angle_r, double curr_angle_l);

public:
	SoCapsule();
	void init(const GlProgram& prog);
	// nfaces: the number of faces approximating the curved faces
	void build(int nfaces);
	void draw(GsMat& tr, GsMat& pr);
};

#endif // SO_CAPSULE_H
