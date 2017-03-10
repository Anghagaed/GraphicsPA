
// Ensure the header file is included only once in multi-file projects
#ifndef SO_TEXTUREDTUBE_H
#define SO_TEXTUREDTUBE_H

// Include needed header files
# include <gsim/gs_mat.h>
# include <gsim/gs_light.h>
# include <gsim/gs_color.h>
# include <gsim/gs_material.h>
# include <gsim/gs_array.h>
# include "ogl_tools.h"
# include "gsim/gs_image.h"
# include <math.h>
# include <vector>
# define PI 3.14159265359

// Scene objects should be implemented in their own classes; and
// here is an example of how to organize a scene object in a class.
// Scene object axis:
class SoTexturedTube : public GlObjects
{
private:
	GlShader _vshphong, _fshphong;
	GlProgram _progphong;
	GsArray<GsVec> P;	// coordinates
	GsArray<GsVec> N;	// normals
	std::vector<GsVec2>	T;	// texture coords
	GsMaterial _mtl;
	int _numpoints;     // just saves the number of points
	int _nfaces;
	float x, y, z;
	float r1, r2, l;
	bool _flatn;
	GLuint _texid;
	std::vector < float > dx, dz;
private:
	void buildBody();
	void calculateParameters();
public:
public:
	SoTexturedTube();
	void init();
	void flat(bool b) { _flatn = b; }
	void build(float r1, float r2, float l, int _nfaces);
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& x);
};

#endif // SO_TEXTUREDTUBE_H
