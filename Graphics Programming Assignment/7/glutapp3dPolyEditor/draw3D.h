
// Ensure the header file is included only once in multi-file projects
#ifndef DRAW3D_H
#define DRAW3D_H

// Include needed header files
# include <gsim/gs_mat.h>
# include <gsim/gs_color.h>
# include <gsim/gs_array.h>
# include <gsim/gs_color.h>
# include <gsim/gs_array.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"
# include <vector>
# include <math.h>

// Scene objects should be implemented in their own classes; and
// here is an example of how to organize a scene object in a class.
// Scene object axis:

using namespace std;

enum zKey {D_NONE = 0, D_ONCE};
enum xKey { D_OFF = 0, D_ON };

class draw3D : public GlObjects
{
private:
	GlShader vsh, fsh;
	GlProgram prog;
	GsArray<GsVec>   P; // coordinates
	GsArray<GsColor> C; // diffuse colors per face
	GsArray<GsVec>   N; // normals
	int numPoints;
	GsColor color;
private:
public:
	draw3D();
	void init();
	void draw(const GsMat& tr, const GsMat& pr);
	void build(const GsArray<GsVec>& P);
};

#endif // SO_CURVE_H
