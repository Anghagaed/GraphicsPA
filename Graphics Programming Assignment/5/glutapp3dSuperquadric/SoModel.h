
// Ensure the header file is included only once in multi-file projects
#ifndef SO_CYLINDER_H
#define SO_CYLINDER_H

// Include needed header files
# include <gsim/gs_mat.h>
# include <gsim/gs_light.h>
# include <gsim/gs_color.h>
# include <gsim/gs_material.h>
# include <gsim/gs_array.h>
# include <vector>
# include "ogl_tools.h"
# include "model.h"

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
	int _numpoints;
	Model _model;
private:
	
public:
public:
	SoCylinder();
	void init();
	void build(const char * filename);
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l);
};

#endif // SO_CYLINDER_H
