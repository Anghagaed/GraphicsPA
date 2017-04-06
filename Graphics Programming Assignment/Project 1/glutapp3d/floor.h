
// Ensure the header file is included only once in multi-file projects
#ifndef SO_FLOOR_H
#define SO_FLOOR_H

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

# define PIOVER2 1.57079632679
# define PI 3.14159265359
# define TWOPI 6.28318530718

// Scene objects should be implemented in their own classes; and
// here is an example of how to organize a scene object in a class.
// Scene object axis:
class SoFloor : public GlObjects
{
private:
	GlShader _vshphong, _fshphong;
	GlProgram _progphong;
	GsArray<GsVec> P;	// coordinates
	GsArray<GsVec> N;	// normals, inherently is smooth
	std::vector<GsVec2>	T;	// texture coords
	GsMaterial _mtl;
	int _numpoints;     // just saves the number of points
	float x, y, z, l, w;
	GLuint _texid1;
	GsMaterial _mtl;
	
	std::string imagePath;
private:
	void buildSquare();
public:

public:
	SoFloor();
	void init(float x, float y, float z, float l, float w);
	void build();
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l);
};

#endif SO_FLOOR_H
