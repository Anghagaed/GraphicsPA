
// Ensure the header file is included only once in multi-file projects
#ifndef CURVE_H
#define CURVE_H

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

enum curveType {NOCURVE = 0, LAGRANGE, BEZIER};

class SoCurve : public GlObjects
{
private:
	GlShader vsh, fsh;
	GlProgram prog;
	GsArray<GsVec>   P; // coordinates
	GsArray<GsVec>   copy; // coordinates
	GsArray<GsColor> C; // diffuse colors per face
	GsArray<GsVec>   N; // normals
	int numSegs;
	int numPoints;
	curveType type;
	GsColor color;
private: 
	GsVec eval_bezier(float t, const GsArray<GsVec>& ctrlpnts);
	GsVec eval_lagrange(float t, const GsArray<GsVec>& ctrlpnts);
public:
	SoCurve();
	void init();
	void build(int numSeqs, const GsArray<GsVec>& ctrlpnts, curveType type);
	void draw(const GsMat& tr, const GsMat& pr);
	GsArray<GsVec>& getCopy();
};

#endif // SO_CURVE_H
