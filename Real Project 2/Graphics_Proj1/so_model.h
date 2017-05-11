
// Ensure the header file is included only once in multi-file projects
#ifndef SO_MODEL_H
#define SO_MODEL_H

// Include needed header files
# include <gsim/gs_mat.h>
# include <gsim/gs_light.h>
# include <gsim/gs_color.h>
# include <gsim/gs_array.h>
# include <gsim/gs_model.h>
# include "ogl_tools.h"

// Scene objects should be implemented in their own classes; and
// here is an example of how to organize a scene object in a class.
// Scene object axis:
class SoModel : public GlObjects
 { private :
    GlShader _vshgou, _fshgou, _vshphong, _fshphong;
    GlProgram _proggouraud, _progphong;

    GsArray<GsVec>   P; // coordinates
    GsArray<GsColor> C; // diffuse colors per face
    GsArray<GsVec>   N; // normals
    GsMaterial _mtl;    // main material
    int _numpoints;     // just saves the number of points
    bool _phong;

	// move animation
	GsVec forwardVec;
	float moveOffset;
	float moveYby;							// rigid movement (jump part)
	float moveZby;							// rigid movement (move part)
	float moveXby;							// rigid movement (move part)
	float rotateby;							// model rotation
	int frame_num;

	// Curve
	int numsegs;
	GsArray<GsVec> ctrlpnts;				// control points
	GsArray<GsVec> coord;						// coordinates


   public :
	SoModel ();
    void phong ( bool b ) { _phong=b; }
    bool phong () const { return _phong; }
    void init ();
    void build ( GsModel& m );
    void draw ( const GsMat& tr, const GsMat& pr, const GsLight& l );
	bool animate;
	void move();				// check and animate (will be used in idle() function in app_window)
};

#endif // SO_MODEL_H
