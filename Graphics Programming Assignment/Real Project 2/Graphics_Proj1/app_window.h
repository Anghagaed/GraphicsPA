
// Ensure the header file is included only once in multi-file projects
#ifndef APP_WINDOW_H
#define APP_WINDOW_H

# include <gsim/gs_color.h>
# include <gsim/gs_array.h>
# include <gsim/gs_light.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"
# include "glut_window.h"
# include "so_axis.h"
# include "so_lines.h"
# include "SoTexturedFloor.h"
# include "SoObject.h"

// The functionality of your application should be implemented inside AppWindow
class AppWindow : public GlutWindow
 { private :

    // My scene objects:
    SoAxis _axis;
	SoObject _object;
	SoTexturedFloor _floor;
    // Scene data:
    bool  _viewaxis;
    GsLight _light;
	GsVec _lightCoord;
    
    // App data:
    enum MenuEv { evOption0, evOption1 };
    float _rotx, _roty, _fovy;
    int _w, _h;
    double _lasttime; // last time update during animation
    float _animateinc;
	bool _jump;
	bool _move;
	bool _rotate;
	bool _front;
	bool _left;
	float _fs;
   public :
    AppWindow ( const char* label, int x, int y, int w, int h );
    void initPrograms ();
    GsVec2 windowToScene ( const GsVec2& v );
    void printInfo ();

   private : // functions derived from the base class
    virtual void glutMenu ( int m );
    virtual void glutKeyboard ( unsigned char key, int x, int y );
    virtual void glutSpecial ( int key, int x, int y );
    virtual void glutMouse ( int b, int s, int x, int y );
    virtual void glutMotion ( int x, int y );
    virtual void glutIdle ();
    virtual void glutDisplay ();
    virtual void glutReshape ( int w, int h );
 };

#endif // APP_WINDOW_H
