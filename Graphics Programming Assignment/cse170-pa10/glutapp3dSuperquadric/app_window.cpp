
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
   _fs = 0.75f;
   _flatn = true;
   _displayTexture = false;
   _displayWireFrame = false;
   _nfaces = 3;
   initPrograms ();
 }

void AppWindow::printInfo()
 {
   std::cout<<"CSE-170 - Yun's PA 10\n";
   std::cout<<"Camera keys: Arrows + PgUp and PgDn\n";
   std::cout<<"Print this information: i\n";
   std::cout<<"Modify sphere's faces: keys q,a\n";
   std::cout<<"Axis on/off: space bar\n";
   std::cout<<"Flat/Smooth normals: n\n";
   std::cout << "Increase fs: x\n";
   std::cout << "Decrease fs: c\n";
   std::cout << "Display/Not Display texture: z\n";
   std::cout << "Display/Not Display wire frame: v\n";
   std::cout << "End the program: ESC\n";
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

   // set light:
   _light.set ( GsVec(0,0,10), GsColor(90,90,90,255), GsColor::white, GsColor::white );

   // Load demo model:
   _superq.build ( _nfaces );

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
   float inc=0.025f;
   switch ( key )
    { case 27 : exit(1); // Esc was pressed
      case 'i' : printInfo(); return;

      case ' ': _viewaxis = !_viewaxis; break;
	  case 'v': _displayWireFrame = !_displayWireFrame; if(!_displayTexture) redraw(); std::cout << (_displayWireFrame ? "Wire Frame On...\n" : "Wire Frame Off...\n"); break;
      case 'n' : _flatn=!_flatn;
                 std::cout<<(_flatn?"Flat...\n":"Smooth...\n");
                 _superq.flat(_flatn); 
                 break;
	  case 'z': _displayTexture = !_displayTexture; _superq.switchDisplay(_displayTexture); redraw(); std::cout << (_displayTexture ? "Texture On...\n" : "Texture Off...\n"); break;
	  case 'q':  _nfaces += 1; redraw(); break;
	  case 'a':  _nfaces -= 1; redraw(); break;
	  case 'w':  _light.pos += GsVec(0.05, 0.0, 0.0); redraw(); break;
	  case 's':  _light.pos += GsVec(-0.05, 0.0, 0.0); redraw(); break;
	  case 'x':  _fs += 0.05; redraw(); break;
	  case 'c':  _fs -= 0.05; redraw(); break;
      default : return;
	}
    redraw(); 
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
   _superq.build(_nfaces);
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
   _superq.draw ( stransf, sproj, _light, _fs, _displayWireFrame);
   // Swap buffers and draw:
   glFlush();         // flush the pipeline (usually not necessary)
   glutSwapBuffers(); // we were drawing to the back buffer, now bring it to the front
}


