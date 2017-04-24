
// Ensure the header file is included only once in multi-file projects
#ifndef SO_TEXTUREDSPHERE_H
#define SO_TEXTUREDSPHERE_H

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
class SoTexturedSphere : public GlObjects
{
private:
	GlShader _vshphong, _fshphong;
	GlProgram _progphong;
	GsArray<GsVec> P;	// coordinates
	GsArray<GsVec> N;	// normals, inherently is smooth
	std::vector<GsVec2>	T;	// texture coords
	GsMaterial _mtl;
	int _numpoints;     // just saves the number of points
	int _lfaces, _layers; // number of layers and faces per layers level
	int _numfaces;
	GsPnt center;
	float r;			// radius
	GLuint _texid1, _texid2;
	std::vector < float > dx, dy, dz;
	std::string imagePath, shadowPath;
	GsMat initialPos, myTrans, shadow, glbmov;
private:
	void buildTop();
	void buildBottom();
	void calculateParameters();
	float findU(GsPnt P);
	float findV(GsPnt P);
public:

public:
	SoTexturedSphere();
	void init(float x, float y, float z, std::string& imagePath);
	void build(float r, int _lfaces, int _layers);
	void drawShadow(const GsMat& tr, const GsMat& pr, const GsLight& l);
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l);

	void resetMyTrans();
	void setInitialPos(const GsMat& posMat);
	void setMyTrans(const GsMat& transMat);
	void setShadow(const GsMat& shadowMat);
	void setGlbMov(const GsMat& glbmov);

	void applyInitialPos(const GsMat& posMat);
	void applyMyTrans(const GsMat& transMat);
};

#endif SO_TEXTUREDSPHERE_H
