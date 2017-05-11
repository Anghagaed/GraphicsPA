
// Ensure the header file is included only once in multi-file projects
#ifndef SO_JOINT_H
#define SO_JOINT_H

// Include needed header files
# include <gsim/gs_mat.h>
# include <gsim/gs_light.h>
# include <gsim/gs_color.h>
# include <gsim/gs_material.h>
# include <gsim/gs_array.h>
# include "ogl_tools.h"
# include <gsim/gs_image.h>
# include <math.h>
# include <vector>
# include <string>

# define PI 3.14159265359
# define TWOPI 6.28318530718

// Scene objects should be implemented in their own classes; and
// here is an example of how to organize a scene object in a class.
// Scene object axis:
class SoJoint : public GlObjects
{
private:
	GlShader _vshphong, _fshphong;
	GlProgram _progphong;
	GsArray<GsVec> P;	// coordinates
	GsArray<GsVec> N;	// normals, inherently is smooth
	std::vector<GsVec2>	T;	// texture coords
	GsMaterial _mtl;
	int _numpoints;     // just saves the number of points
	int _nfaces;
	float x, y, z;
	float r, l;
	GLuint _texid;
	std::vector < float > dx, dz;
	std::string imagePath;
private:
	void buildBody();
	void buildHat();
	void calculateParameters();
public:
public:
	SoJoint();
	void init(float x, float y, float z, std::string imagePath);
	void build(float r, float l, int _nfaces);
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs);

	GsVec firstJointPts();
	GsVec secondJointPts();
};

#endif // SO_JOINT_H
