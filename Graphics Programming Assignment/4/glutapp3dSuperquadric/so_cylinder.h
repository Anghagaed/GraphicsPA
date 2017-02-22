
// Ensure the header file is included only once in multi-file projects
#ifndef SO_CYLINDER_H
#define SO_CYLINDER_H

// Include needed header files
# include <gsim/gs_mat.h>
# include <gsim/gs_light.h>
# include <gsim/gs_color.h>
# include <gsim/gs_material.h>
# include <gsim/gs_array.h>
# include "ogl_tools.h"
# include <math.h>
# include <vector>
# define PI 3.14159265359

// Scene objects should be implemented in their own classes; and
// here is an example of how to organize a scene object in a class.
// Scene object axis:
class SoCylinder : public GlObjects
{
private:
	GlShader _vshphong, _fshphong;
	GlProgram _progphong;
	GsArray<GsVec>   P; // coordinates
	GsArray<GsColor> C; // diffuse colors per face
	GsArray<GsVec>   N; // normals
	GsMaterial _mtl;
	int _numpoints;     // just saves the number of points
	int _nfaces;
	float x, y, z;
	float r, l;
	bool _flatn;
	std::vector < float > dx, dz;
private:
	void buildBody();
	void buildTop();
	void buildBottom();
	void calculateParameters();
	void normalize(GsPnt a, GsPnt b, GsPnt c);
public:
	GsArray<GsVec> NL; // normal lines computed in last build
public:
	SoCylinder();
	void init();
	void flat(bool b) { _flatn = b; }
	void build(float r, float l, int _nfaces);
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l);
};

#endif // SO_CYLINDER_H
