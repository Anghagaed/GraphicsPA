
# include <iostream>
# include <gsim/gs.h>
# include "app_window.h"
#define _USE_MATH_DEFINES
# include <math.h>
using namespace std;

AppWindow::AppWindow ( const char* label, int x, int y, int w, int h )
          :GlutWindow ( label, x, y, w, h )
 {
   addMenuEntry ( "Option 0", evOption0 );
   addMenuEntry ( "Option 1", evOption1 );

   _viewaxis = true;
   _fovy = GS_TORAD(60.0f);
   _rotx = gspi/8.0f;
   _roty = gspi/4.0f;
   _w = w;
   _h = h;

   stop = false;
   _lasttime = gs_time();
   _animateinc = 0.1f;
   _jump = false;
   _move = true;
   _front = false;
   _left = false;
   _rotate = false;
   _animate = false;
   keyframe = 0;
   initPrograms ();
   _fs = 0.10f;
   //stop = true;
}

//void AppWindow::printInfo()
// {
// }

static void printInfo(GsModel& m)
{
	std::cout << "V:  " << m.V.size() << "\n";
	std::cout << "F:  " << m.F.size() << "\n";
	std::cout << "N:  " << m.N.size() << "\n";
	std::cout << "M:  " << m.M.size() << "\n";
	std::cout << "Fn: " << m.Fn.size() << "\n";
	std::cout << "Fm: " << m.Fm.size() << "\n";
}
void AppWindow::initPrograms ()
 {
   // We are not directly initializing glsl programs here, instead, each scene object
   // initializes its own program(s). This makes sharing more difficult but is probably easier
   // to manage.

   // Init my scene objects:
   _axis.init ();
   _floor.init();
   _object.init();
   _jukebox = SoModel(-0.9, 0.009, -0.9);
   _airplane = SoModel(0.0, 1.0, 0.0);
   _car = SoModel(0.9, 0.09, 0.9);
   _airplane.init();
   _car.init();
   _jukebox.init();
   // set light:
   _lightCoord = GsVec(0.0f, 1.5f, -0.5f);
   _light.set ( GsVec(0,0,5), GsColor(90,90,90,255), GsColor::white, GsColor::white );

   // Load demo model:
   _floor.build();
   _object.build();
   loadModel(3, _airplane);
   loadModel(1, _car);
   loadModel(4, _jukebox);
   _airplane.phong(true);	// always phong shading
   _car.phong(true);		// always phong shading
 }

void AppWindow::loadModel(int model, SoModel& loadMe)
{
	float f;
	GsString file;
	switch (model)
	{
	case 1: f = 0.005f; file = "../models/porsche.obj"; break;
	case 2: f = 0.20f; file = "../models/al.obj"; break;
	case 3: f = 0.05f; file = "../models/f-16.obj"; break;
	case 4: f = 0.05f; file = "../models/Jukebox.obj"; break;
	default: return;
	}
	std::cout << "Loading " << file << "...\n";
	if (!_gsm.load(file)) std::cout << "Error!\n";
	printInfo(_gsm);
	_gsm.scale(f); // to fit our camera space
	loadMe.build(_gsm);
	redraw();
}

// mouse events are in window coordinates, but your 2D scene is in [0,1]x[0,1],
// so make here the conversion when needed
GsVec2 AppWindow::windowToScene ( const GsVec2& v )
 {
   // GsVec2 is a lighteweight class suitable to return by value:
   return GsVec2 ( (2.0f*(v.x/float(_w))) - 1.0f,
                    1.0f - (2.0f*(v.y/float(_h))) );
 }

// Called every time there is a window event
void AppWindow::glutKeyboard ( unsigned char key, int x, int y )
 {
 //  float inc=0.025f;
 //  switch ( key )
 //   { case 27 : exit(1); // Esc was pressed
 //     //case 'i' : printInfo(); return;

 //     case ' ': _viewaxis = !_viewaxis; break;
	//  case 'z': _jump = true; break;
	//  case 'x':  _fs += 0.05; redraw(); break;
	//  case 'c':  _fs -= 0.05; redraw(); break;
	//  case 'h': _animate = true; keyframe = 1; break;
	//  case 'j': _move = true; break;
	//  case 'l': stop = !stop; break;
	//  case '8': _move = true; _front = true; break;
	//  case '2': _move = true; _front = false; break;
	//  case '4': _rotate = true; _left = true; break;
	//  case '6': _rotate = true; _left = false; break;
	//  default : return;
	//}
 //   redraw(); 
	switch (key)
	{
	case ' ': _viewaxis = !_viewaxis; redraw(); break;
	case 27: exit(1); // Esc was pressed
	case 's': std::cout << "Smoothing normals...\n";
		_gsm.smooth(GS_TORAD(35));
		printInfo(_gsm);
		_airplane.build(_gsm);
		_car.build(_gsm);
		_jukebox.build(_gsm);
		redraw();
		break;
	case 'f': std::cout << "Flat normals...\n";
		_gsm.flat();
		printInfo(_gsm);
		_airplane.build(_gsm);
		_car.build(_gsm);
		_jukebox.build(_gsm);
		redraw();
		break;
	case 'g': std::cout << "Animate...\n"; stop = false; break;
	}
 }

void AppWindow::glutSpecial ( int key, int x, int y )
 {
   bool rd=true;
   const float incr=GS_TORAD(2.5f);
   const float incf=0.05f;
   switch ( key )
    { case GLUT_KEY_LEFT:      _roty-=incr; break;
      case GLUT_KEY_RIGHT:     _roty+=incr; break;
      case GLUT_KEY_UP:        _rotx+=incr; break;
      case GLUT_KEY_DOWN:      _rotx-=incr; break;
      case GLUT_KEY_PAGE_UP:   _fovy-=incf; break;
      case GLUT_KEY_PAGE_DOWN: _fovy+=incf; break;
      default: return; // return without rendering
	}
   if (rd) redraw(); // ask the window to be rendered when possible
 }

void AppWindow::glutMouse ( int b, int s, int x, int y )
 {
   // The mouse is not used in this example.
   // Recall that a mouse click in the screen corresponds
   // to a whole line traversing the 3D scene.
 }

void AppWindow::glutMotion ( int x, int y )
 {
 }

void AppWindow::glutMenu ( int m )
 {
   std::cout<<"Menu Event: "<<m<<std::endl;
 }

void AppWindow::glutReshape ( int w, int h )
 {
   // Define that OpenGL should use the whole window for rendering
   glViewport( 0, 0, w, h );
   _w=w; _h=h;
 }

void AppWindow::glutIdle ()
 {
	// millisecond * 60 / 1000 = 1/60 second
	if (glutGet(GLUT_ELAPSED_TIME) * 60 % (1000) == 0 && !stop)
	{
		//_airplane.animate = true;
		//_airplane.move();
		//_car.animate = true;
		//_car.move();
		//if (_animate)
		//	_object.keyFrame1(_animate);
		//else
		//	keyframe = 0;
		//_object.jump(_jump);
		//_object.move(_move);
		//_object.turn(_rotate, _left);
		if (!_object.animationThree())
			stop = true;
		redraw();
	}
 }
GsMat shadowMatrix(float light[3])
{
	float cof = 0.77;
	float temp[16] = {
		1, -light[0] / light[1], 0, light[0] * (cof) / light[1],
		0, 0, 0, -(cof),
		0, -light[2] / light[1], 1, light[2] * (cof) / light[1],
		0, 0, 0, 1
	};

	GsMat mat(temp);
	return mat;
}
// here we will redraw the scene according to the current state of the application.
void AppWindow::glutDisplay ()
 {
   // Clear the rendering window
   glClearColor(0.1f,0.1f,0.2f,1); // change the background a little bit
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // Build a cross with some lines (if not built yet):
   if ( _axis.changed ) // needs update
    { _axis.build(1.0f); // axis has radius 1.0
    }
   // Define our scene transformation:
   GsMat rx, ry, stransf;
   rx.rotx ( _rotx );
   ry.roty ( _roty );
   stransf = rx*ry; // set the scene transformation matrix

   // Define our projection transformation:
   // (see demo program in gltutors-projection.7z, we are replicating the same behavior here)
   GsMat camview, persp, sproj;
   float x, y, z;
   GsVec eye(0,0,2), center(0,0,0), up(0,1,0);
   camview.lookat ( eye, center, up ); // set our 4x4 "camera" matrix

   float aspect=1.0f, znear=0.1f, zfar=50.0f;
   persp.perspective ( _fovy, aspect, znear, zfar ); // set our 4x4 perspective matrix

   // Our matrices are in "line-major" format, so vertices should be multiplied on the 
   // right side of a matrix multiplication, therefore in the expression below camview will
   // affect the vertex before persp, because v' = (persp*camview)*v = (persp)*(camview*v).
   sproj = persp * camview; // set final scene projection

   //  Note however that when the shader receives a matrix it will store it in column-major 
   //  format, what will cause our values to be transposed, and we will then have in our 
   //  shaders vectors on the left side of a multiplication to a matrix.
   
   // Draw:
   if ( _viewaxis ) _axis.draw ( stransf, sproj );

   // Draw the object
   _object.draw(stransf, sproj, _light, _fs, _lightCoord);

   // Everything else
  // _floor.draw(stransf, sproj, _light, _fs);
  // _airplane.draw(stransf, sproj, _light);
  // _car.draw(stransf, sproj, _light);
   //_jukebox.draw(stransf, sproj, _light);

   // Swap buffers and draw:
   glFlush();         // flush the pipeline (usually not necessary)
   glutSwapBuffers(); // we were drawing to the back buffer, now bring it to the front
}


