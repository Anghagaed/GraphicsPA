
# include <iostream>
# include <gsim/gs.h>
# include "app_window.h"

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
   _numfaces = 3;

   A=B=C=0.5f;
   R=2.0f; S=2.0f; T=0.9f;

   _lasttime = gs_time();
   _animateinc = 0.1f;
   _animate = true;
   _normals = false;
   _flatn = true;
   _phong = false;
   _normals = true;

   initPrograms ();
 }

void AppWindow::printInfo()
 {
   std::cout<<"CSE-170 - Superquadric Example Project\n";
   std::cout<<"Camera keys: Arrows + PgUp and PgDn\n";
   std::cout<<"Print this information: i\n";
   std::cout<<"Modify superquadric: keys q,a,w,s,...\n";
   std::cout<<"Axis on/off: space bar\n";
   std::cout<<"Show/hide normals: z\n";
   std::cout<<"Animation on/off: x\n";
   std::cout<<"Flat/Smooth normals: n\n";
   std::cout<<"Phong/Gouraud shading: p\n";
   std::cout<<"\n";
 }

void AppWindow::initPrograms ()
 {
   // We are not directly initializing glsl programs here, instead, each scene object
   // initializes its own program(s). This makes sharing more difficult but is probably easier
   // to manage.

   // Init my scene objects:
   _axis.init ();
   _superq.init ();
   _lines.init ();
   _cylinder.init();

   // set light:
   _light.set ( GsVec(0,0,10), GsColor(90,90,90,255), GsColor::white, GsColor::white );

   // Load demo model:
   //_superq.build ( R,S,T,A,B,C );


   // Build normals object:
   _lines.build ( _cylinder.NL, GsColor::yellow );
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
	/*
   float inc=0.025f;
   switch ( key )
    { case 27 : exit(1); // Esc was pressed
      case 'i' : printInfo(); return;

      case ' ': _viewaxis = !_viewaxis; break;
      case 'z' : _normals=!_normals; break;
      case 'x' : _animate=!_animate; break;

      case 'n' : _flatn=!_flatn;
                 std::cout<<(_flatn?"Flat...\n":"Smooth...\n");
                 _superq.flat(_flatn); 
                 break;

      case 'p' : _phong=!_phong;
                  std::cout<<"Switching to "<<(_phong?"Phong shader...\n":"Gouraud shader...\n");
                 _superq.phong(_phong);
                 break;

      case 'q' : _superq.build ( R+=inc,S,T,A,B,C ); std::cout<<"r="<<R<<gsnl; break;
      case 'a' : _superq.build ( R-=inc,S,T,A,B,C ); std::cout<<"r="<<R<<gsnl; break;
      case 'w' : _superq.build ( R,S+=inc,T,A,B,C ); std::cout<<"s="<<S<<gsnl; break;
      case 's' : _superq.build ( R,S-=inc,T,A,B,C ); std::cout<<"s="<<S<<gsnl; break;
      case 'e' : _superq.build ( R,S,T+=inc,A,B,C ); std::cout<<"t="<<T<<gsnl; break;
      case 'd' : _superq.build ( R,S,T-=inc,A,B,C ); std::cout<<"t="<<T<<gsnl; break;

      case 'r' : _superq.build ( R,S,T,A+=inc,B,C ); std::cout<<"A="<<A<<gsnl; break;
      case 'f' : _superq.build ( R,S,T,A-=inc,B,C ); std::cout<<"A="<<A<<gsnl; break;
      case 't' : _superq.build ( R,S,T,A,B+=inc,C ); std::cout<<"B="<<B<<gsnl; break;
      case 'g' : _superq.build ( R,S,T,A,B-=inc,C ); std::cout<<"B="<<B<<gsnl; break;
      case 'y' : _superq.build ( R,S,T,A,B,C+=inc ); std::cout<<"C="<<C<<gsnl; break;
      case 'h' : _superq.build ( R,S,T,A,B,C-=inc ); std::cout<<"C="<<C<<gsnl; break;


      default : return;
	}
   if ( _normals ) _lines.build ( _superq.NL, GsColor::blue );
    redraw(); 
	*/
	switch (key) {
	case 'q': _numfaces += 1; redraw(); break;
	case 'a': _numfaces -= 1; redraw(); break;
	case 'z': _cylinder.flat(true); redraw(); break;
	case 'x': _cylinder.flat(false); redraw(); break;
	case 'c': _normals = true; redraw(); break;
	case 'v': _normals = false; redraw(); break;
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
	/*
   if ( !_animate ) return;

   double curtime = gs_time();
   if (curtime - _lasttime > 0.05f)
   { _roty+=0.005f;
     if ( R<0.4 || R>8 ) _animateinc*=-1;
     R+=_animateinc;
     S+=_animateinc;
     _superq.build ( R,S,T,A,B,C ); 
     if ( _normals ) _lines.build ( _superq.NL, GsColor::blue );
     redraw();
     _lasttime=curtime;
   }
   */
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
   _cylinder.build(0.25, 0.50, _numfaces);
   _lines.build(_cylinder.NL, GsColor::yellow);

   // Define our scene transformation:
   GsMat rx, ry, stransf;
   rx.rotx ( _rotx );
   ry.roty ( _roty );
   stransf = rx*ry; // set the scene transformation matrix

   // Define our projection transformation:
   // (see demo program in gltutors-projection.7z, we are replicating the same behavior here)
   GsMat camview, persp, sproj;

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
   //_superq.draw ( stransf, sproj, _light );

   _lines.build(_cylinder.NL, GsColor::yellow);
   if ( _normals ) _lines.draw ( stransf, sproj );
   _cylinder.draw(stransf, sproj, _light);
   // Swap buffers and draw:
   glFlush();         // flush the pipeline (usually not necessary)
   glutSwapBuffers(); // we were drawing to the back buffer, now bring it to the front
}


