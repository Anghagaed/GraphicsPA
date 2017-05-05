
// Ensure the header file is included only once in multi-file projects
#ifndef SO_MODEL_H
#define SO_MODEL_H

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
class SoSuperquadric : public GlObjects
 { private :
    GlShader _vsh, _fsh;
    GlProgram _prog;
    GsArray<GsVec>   P; // coordinates
    GsArray<GsColor> C; // diffuse colors per face
    GsArray<GsVec>   N; // normals
	GsArray<GsVec2>  T; // texture coords
	GLuint _texid;
    GsMaterial _mtl;
    int _numpoints;     // just saves the number of points
    bool _flatn;
	bool _displayTexture;
	void drawSphere(int faces); 
	void texturize(GsVec x);
   public :
    SoSuperquadric ();
    void init ();
    void flat ( bool b ) { _flatn=b; }
	void build(int nfaces);
    void draw ( const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs, const bool displayWireFrame);
	void switchDisplay(bool which);
 };

#endif // SO_MODEL_H
