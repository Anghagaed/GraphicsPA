
// Ensure the header file is included only once in multi-file projects
#ifndef SO_TEXTURED_FLOOR
#define SO_TEXTURED_FLOOR

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
class SoTexturedFloor : public GlObjects
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
	void drawPrism(double radius);
	void drawTube(double radius, double height, double sine_r, double cosine_r, double sine_l, double cosine_l);
	void drawCaps(double radius, double height, double sine_r, double cosine_r, double sine_l, double cosine_l);
public:
	GsArray<GsVec> NL; // normal lines computed in last build
public:
	SoTexturedFloor();
	void init();
	void flat(bool b) { _flatn = b; }
	void build(double radius);
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs);
};

#endif // SO_TEXTURED_FLOOR
