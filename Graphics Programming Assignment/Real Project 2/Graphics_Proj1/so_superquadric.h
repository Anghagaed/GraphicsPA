
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

// Scene objects should be implemented in their own classes; and
// here is an example of how to organize a scene object in a class.
// Scene object axis:
class SoSuperquadric : public GlObjects
 { private :
    GlShader _vshphong, _fshphong;
    GlProgram _progphong;
    GsArray<GsVec>   P; // coordinates
    GsArray<GsColor> C; // diffuse colors per face
    GsArray<GsVec>   N; // normals
    GsMaterial _mtl;
    int _numpoints;     // just saves the number of points
    bool _flatn;
	void drawCapsule(int faces); 
	void drawTube(double x_l, double x_r, double z_r, double z_l, double height, double sine_r, double cosine_r, double sine_l, double cosine_l);
	void drawCircle(double x_l, double x_r, double z_r, double z_l, double height, double sine, double cosine);//, int frame_num, double circle_rate, double radius, double curr_angle_r, double curr_angle_l);
   public :
    GsArray<GsVec> NL; // normal lines computed in last build
   public :
    SoSuperquadric ();
    void init ();
    void flat ( bool b ) { _flatn=b; }
	void build(int nfaces);
    void draw ( const GsMat& tr, const GsMat& pr, const GsLight& l );
 };

#endif // SO_MODEL_H
