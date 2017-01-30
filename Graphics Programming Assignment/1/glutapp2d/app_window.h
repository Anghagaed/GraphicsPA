
// Ensure the header file is included only once in multi-file projects
#ifndef APP_WINDOW_H
#define APP_WINDOW_H
#define _USE_MATH_DEFINES

# include <vector>
# include <gsim/gs_color.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"
# include "glut_window.h"
# include <math.h>

// The functionality of your application should be implemented inside AppWindow
class AppWindow : public GlutWindow
 { private : // My OGL Data
    GlShader _vertexsh, _fragsh;
    GlProgram _prog;
    GlObjects _tris, _pts, _lines;
	std::vector<GsVec>   _tricoords;
    std::vector<GsColor> _tricolors;
    std::vector<GsVec>   _ptcoords;
    std::vector<GsColor> _ptcolors;
    std::vector<GsVec>   _linecoords;
    std::vector<GsColor> _linecolors;

   private : // App Data
    enum MenuEv { evOption0, evOption1 };
	double lx, ly, ldx, ldy;
	double ddx;
	boolean bx, by;
    GsColor _markc;
    GsVec2 _mark;
    int _w, _h;

   public :
    AppWindow ( const char* label, int x, int y, int w, int h );
    void initPrograms ();
	void buildObjects ();
    GsVec2 windowToScene ( const GsVec2& v );

   private : // functions derived from the base class
    virtual void glutMenu ( int m );
    virtual void glutKeyboard ( unsigned char key, int x, int y );
    virtual void glutSpecial ( int key, int x, int y );
    virtual void glutMouse ( int b, int s, int x, int y );
    virtual void glutMotion ( int x, int y );
    virtual void glutDisplay ();
    virtual void glutReshape ( int w, int h );
	virtual void glutIdle();
	
	// Functions for drawing character
	void drawPaper(double x1, double x2, double y1, double y2, double dx, double dy, GsColor col);
	void drawLetterH(double x1, double x2, double y1, double y2, double y3, double dx, double dy, GsColor col);
	void drawLetterA(double x1, double x2, double x3, double y1, double y2, double y3, double dx, double dy, GsColor col);
	void drawLetterN(double x1, double x2, double y1, double y2, double dx, double dy, GsColor col);
	void drawLetterG(double x1, double y1, double r, double dQ, double dR, double dx, double dy, GsColor col);
	void drawLines(double x1, double y1, double x2, double y2, GsColor col);
	void drawPoints(double x, double y, GsColor col);
 };

#endif // APP_WINDOW_H
