
// Ensure the header file is included only once in multi-file projects
#ifndef SO_TEXTUREDCYLINDER_H
#define SO_TEXTUREDCYLINDER_H

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
# include <string>

# define PI 3.14159265359
# define TWOPI 6.28318530718

// Scene objects should be implemented in their own classes; and
// here is an example of how to organize a scene object in a class.
// Scene object axis:
class SoTexturedCylinder : public GlObjects
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
	GLuint _texid1, _texid2;
	std::vector < float > dx, dz;
	std::string imagePath, shadowPath;
	GsMat initialPos, myTrans, shadow, glbmov, jointMovement;

private:
	void buildBody();
	void buildHat();
	void calculateParameters();
public:

public:
	SoTexturedCylinder();
	void init(float x, float y, float z, std::string& imagePath);
	void build(float r, float l, int _nfaces);
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l);
	void drawShadow(const GsMat& tr, const GsMat& pr, const GsLight& l);

	void resetMyTrans();
	void setInitialPos(const GsMat& posMat);
	void setMyTrans(const GsMat& transMat);
	void setShadow(const GsMat& shadowMat);
	void setGlbMov(const GsMat& glbmov);
	void setJntMov(const GsMat& jntMat);

	void applyInitialPos(const GsMat& posMat);
	void applyMyTrans(const GsMat& transMat);

};

#endif // SO_TEXTUREDCYLINDER_H
