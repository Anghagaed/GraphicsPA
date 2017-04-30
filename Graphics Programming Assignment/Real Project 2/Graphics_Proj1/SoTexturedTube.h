
// Ensure the header file is included only once in multi-file projects
#ifndef SO_TEXTURED_TUBE
#define SO_TEXTURED_TUBE

// Include needed header files
# include <gsim/gs_mat.h>
# include <gsim/gs_light.h>
# include <gsim/gs_color.h>
# include <gsim/gs_material.h>
# include <gsim/gs_array.h>
# include "ogl_tools.h"
# include "gs_shared.h"
# include "gs_image.h"

// Scene objects should be implemented in their own classes; and
// here is an example of how to organize a scene object in a class.
// Scene object axis:
class SoTexturedTube : public GlObjects
{
private:
	GlShader _vsh, _fsh;
	GlProgram _prog;
	GsArray<GsVec>   P; // coordinates
	GsArray<GsVec2>  T; // texture coords
	GsArray<GsVec>   N; // normals
	GsMaterial _mtl;
	GLuint _texid;
	int _numpoints;     // just saves the number of points
	bool _flatn;
	void drawCapsule(int nfaces, double radiusTop, double radiusBot);
	void drawTube(double radiusTop, double radiusBot, double height, double sine_r, double cosine_r, double sine_l, double cosine_l);
	void SoTexturedTube::drawCircle(double radiusTop, double radiusBot, double height, double sine_r, double cosine_r, double sine_l, double cosine_l);
public:
	GsArray<GsVec> NL; // normal lines computed in last build
public:
	SoTexturedTube();
	void init();
	void flat(bool b) { _flatn = b; }
	void build(int nfaces, double radiusTop, double radiusBot);
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs);
	void updateTexture(bool which);
};

#endif // SO_TEXTURED_TUBE
