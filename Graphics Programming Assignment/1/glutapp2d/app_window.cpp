
# include <iostream>
# include "app_window.h"

AppWindow::AppWindow ( const char* label, int x, int y, int w, int h )
          :GlutWindow ( label, x, y, w, h )
 {
   initPrograms ();
   addMenuEntry ( "Option 0", evOption0 );
   addMenuEntry ( "Option 1", evOption1 );
   _markc = GsColor::yellow;
   _w=w; _h=h;
   lx = 0;
   ly = -0.625;
   ldx = 0.25;
   ldy = 0.125;
   ddx = 0.001;
   bx = true;
   by = true;
 }

void AppWindow::initPrograms ()
 {
   // We are following the OpenGL API version 4, 
   // see docs at: http://www.opengl.org/sdk/docs/man/

   // Load your shaders and link your programs here:
   _vertexsh.load_and_compile ( GL_VERTEX_SHADER, "2dcolored.vert" );
   _fragsh.load_and_compile ( GL_FRAGMENT_SHADER, "flat.frag" );
   _prog.init_and_link ( _vertexsh, _fragsh );

   // Define buffers needed for each of your OpenGL objects:
   // (here they all use the same definitions because we are using the same shaders)
   // Program for rendering triangles:
   { GlObjects& o = _tris;
     o.set_program ( _prog );
     o.gen_vertex_arrays ( 1 );
     o.gen_buffers ( 2 );
     o.uniform_locations ( 2 );
     o.uniform_location ( 0, "vTransf" );
     o.uniform_location ( 1, "vLocf" );
   }

   // Define buffers needed for each of your OpenGL objects:
   { GlObjects& o = _pts;
     o.set_program ( _prog );
     o.gen_vertex_arrays ( 1 );
     o.gen_buffers ( 2 );
     o.uniform_locations ( 2 );
     o.uniform_location ( 0, "vTransf" );
     o.uniform_location ( 1, "vLocf" );
   }

   // Define buffers needed for each of your OpenGL objects:
   { GlObjects& o = _lines;
     o.set_program ( _prog );
     o.gen_vertex_arrays ( 1 );
     o.gen_buffers ( 2 );
     o.uniform_locations ( 2 );
     o.uniform_location ( 0, "vTransf" );
     o.uniform_location ( 1, "vLocf" );
   }
 }


// mouse events are in window coordinates, but your scene is in [0,1]x[0,1],
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
   switch ( key )
    { case ' ': // space bar
	   std::cout << "Space pressed.\n";
       _mark.set ( 1.5, 1.5 );
       redraw();
	   break;

	case GLUT_KEY_LEFT :
		ddx -= 0.001;
		break;
	case GLUT_KEY_RIGHT :
		ddx += 0.001;
		break;
	case GLUT_KEY_UP :
		ddx = 0.001;
		break;
	case GLUT_KEY_DOWN :
		ddx = 0.0;
		break;
	  case 27: // Esc was pressed
	   exit(1);
	}
 }

void AppWindow::glutSpecial ( int key, int x, int y )
 {
   bool rd=true;
   const float incx=0.02f;
   const float incy=0.02f;
   switch ( key )
    { case GLUT_KEY_LEFT:  
		_mark.x-=incx; 
		ddx -= 0.001;
		break;
      case GLUT_KEY_RIGHT: 
		  _mark.x+=incx;
		  ddx += 0.001;
		  break;
      case GLUT_KEY_UP:    
		  _mark.y+=incy; 
		  ddx = 0.001;
		  break;
      case GLUT_KEY_DOWN:  
		  _mark.y-=incy; 
		  ddx = 0.0;
		  break;
      default: rd=false; // no redraw
	}

   if (rd) 
    { _lines.changed = true; // mark that new lines have to be generated
      redraw(); // ask the window to be rendered when possible
    }
 }

void AppWindow::glutMouse ( int b, int s, int x, int y )
 {
   _markc = GsColor::yellow;
   _mark = windowToScene ( GsVec2(x,y) );
   _lines.changed = true; // mark that new lines have to be generated
   // note: a better design would be to only change the geometry when really needed so
   // that we minimize overhead with buffer definition and transfer, for example here
   // we could just send a transformation to our shader and keep the geometry the same.
   redraw();
 }

void AppWindow::glutMotion ( int x, int y )
 {
   _markc = GsColor::red;
   _mark = windowToScene ( GsVec2(x,y) );
   _lines.changed = true; // mark that new lines have to be generated
   redraw();
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

// here we will redraw the scene according to the current state of the application.
void AppWindow::buildObjects()
{
	// Define a cross with some lines:
	if (_lines.changed) // needs update
	{
		_linecoords.clear(); _linecolors.clear();
		// Encode our lines in buffers according to _mark position, _markc color and size s:
		/*
		const float s = 0.05f;
		_linecoords.push_back(GsVec(_mark.x - s, _mark.y, 0)); _linecolors.push_back(_markc);
		_linecoords.push_back(GsVec(_mark.x + s, _mark.y, 0)); _linecolors.push_back(_markc);
		_linecoords.push_back(GsVec(_mark.x, _mark.y - s, 0)); _linecolors.push_back(_markc);
		_linecoords.push_back(GsVec(_mark.x, _mark.y + s, 0)); _linecolors.push_back(_markc);
		// send data to OpenGL buffers:
		glBindBuffer(GL_ARRAY_BUFFER, _lines.buf[0]);
		glBufferData(GL_ARRAY_BUFFER, _linecoords.size() * 3 * sizeof(float), &_linecoords[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, _lines.buf[1]);
		glBufferData(GL_ARRAY_BUFFER, _linecolors.size() * 4 * sizeof(gsbyte), &_linecolors[0], GL_STATIC_DRAW);
		// mark that data does not need more changes:
		*/
		_lines.changed = false;
	}

	// Define some white points:
	if (_pts.changed) // needs update
	{
		/*
		_ptcoords.clear(); _ptcolors.clear();
		// Encode some white points in buffers:
		_ptcoords.push_back(GsVec(0.5, 0.5, 0.0)); _ptcolors.push_back(GsColor::white);
		_ptcoords.push_back(GsVec(-0.5, -0.5, 0.0)); _ptcolors.push_back(GsColor::white);
		_ptcoords.push_back(GsVec(0.5, -0.5, 0.0)); _ptcolors.push_back(GsColor::white);
		//_ptcoords.push_back(GsVec(-0.5, 0.5, 0.0)); _ptcolors.push_back(GsColor::white);
		// send data to OpenGL buffers:
		glBindBuffer(GL_ARRAY_BUFFER, _pts.buf[0]);
		glBufferData(GL_ARRAY_BUFFER, _ptcoords.size() * 3 * sizeof(float), &_ptcoords[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, _pts.buf[1]);
		glBufferData(GL_ARRAY_BUFFER, _ptcolors.size() * 4 * sizeof(gsbyte), &_ptcolors[0], GL_STATIC_DRAW);
		// mark that data does not need more changes:
		*/
		_pts.changed = false;
		
	}

	// Define some triangles:
	if (true) // needs update
	{
		// x1 = -0.30, x2 = -0.15, x3 = 0.0, y1 = 0.65, y2 = 0.35, y3 = 0.05
		// dx = 0.05, dy = 0.05
		_tricoords.clear(); _tricolors.clear();
		_linecoords.clear(); _linecolors.clear();
		_ptcoords.clear(); _ptcolors.clear();

		// Creating a sheet of paperwhite with  color
		this->drawPaper(0.75, -0.75, 0.70, 0.0, 0.005, 0.005, GsColor::white);
		// Drawing letter H
		this->drawLetterH(-0.675, -0.375, 0.65, 0.35, 0.05, 0.005, 0.005, GsColor::red);
		// Drawing letter A
		this->drawLetterA(-0.30, -0.15, 0.0, 0.65, 0.35, 0.05, 0.005, 0.01, GsColor::red);
		// Drawing letter N
		this->drawLetterN(0.35, 0.05, 0.65, 0.05, 0.005, 0.005, GsColor::red);
		// Drawing letter G
		this->drawLetterG(0.55, 0.35, 0.125, M_PI/1024, 0.005, 0.005, 0.005, GsColor::red);

		// Creating a triangle using lines
		drawLines(lx, ly + ldy, lx + ldx, ly - ldy, GsColor::blue);
		drawLines(lx, ly + ldy, lx - ldx, ly - ldy, GsColor::red);
		drawLines(lx + ldx, ly - ldy, lx - ldx, ly - ldy, GsColor::green);
		drawPoints(lx, ly, GsColor::yellow);
		// Create a point at the center of the triangle
		// send data to OpenGL buffers:
		glBindBuffer(GL_ARRAY_BUFFER, _tris.buf[0]);
		glBufferData(GL_ARRAY_BUFFER, _tricoords.size() * 3 * sizeof(float), &_tricoords[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, _tris.buf[1]);
		glBufferData(GL_ARRAY_BUFFER, _tricolors.size() * 4 * sizeof(gsbyte), &_tricolors[0], GL_STATIC_DRAW);
		// mark that data does not need more changes:
		_tris.changed = false;
	}
 }
void AppWindow::glutIdle() {
	if (lx > 0.75 || lx < -0.75) {
		ddx = -ddx;
	}
	lx += ddx;
	glutPostRedisplay();
	//this->buildObjects();
}
void AppWindow::drawPoints(double x, double y, GsColor col) {
	_ptcoords.push_back(GsVec(x, y, 0.0)); _ptcolors.push_back(col);
	glBindBuffer(GL_ARRAY_BUFFER, _pts.buf[0]);
	glBufferData(GL_ARRAY_BUFFER, _ptcoords.size() * 3 * sizeof(float), &_ptcoords[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, _pts.buf[1]);
	glBufferData(GL_ARRAY_BUFFER, _ptcolors.size() * 4 * sizeof(gsbyte), &_ptcolors[0], GL_STATIC_DRAW);
}
void AppWindow::drawPaper(double x1, double x2, double y1, double y2, double dx, double dy, GsColor col) {
	// Coordinates and changes in x and y
	//double x1 = 0.65, x2 = -0.65, y1 = 0.70, y2 = 0;
	//double dx = 0.005, dy = 0.005;
	// Drawing the right vertical line
	_tricoords.push_back(GsVec(x1 - dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 - dx, y2, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y2, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x1 - dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y2, 0.0)); _tricolors.push_back(col);

	// Drawing the left vertical line
	_tricoords.push_back(GsVec(x2 + dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 + dx, y2, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 - dx, y2, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x2 + dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 - dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 - dx, y2, 0.0)); _tricolors.push_back(col);
	// Drawing the bottom line
	_tricoords.push_back(GsVec(x1, y2 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1, y2 - dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2, y2 - dy, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x1, y2 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2, y2 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2, y2 - dy, 0.0)); _tricolors.push_back(col);
	// Drawing the upper line
	_tricoords.push_back(GsVec(x1, y1 - dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1, y1 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2, y1 - dy, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x1, y1 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2, y1 - dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2, y1 + dy, 0.0)); _tricolors.push_back(col);
}

void AppWindow::drawLetterH(double x1, double x2, double y1, double y2, double y3, double dx, double dy, GsColor col) {
	// Drawing left vertical line 
	_tricoords.push_back(GsVec(x1 - dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 - dx, y3, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y3, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x1 - dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y3, 0.0)); _tricolors.push_back(col);
	// Drawing right vertical line
	_tricoords.push_back(GsVec(x2 - dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 - dx, y3, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 + dx, y3, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x2 - dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 + dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 + dx, y3, 0.0)); _tricolors.push_back(col);
	// Drawing middle horizontal line
	_tricoords.push_back(GsVec(x1 + dx, y2 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y2 - dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 - dx, y2 - dy, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x1 + dx, y2 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 - dx, y2 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 - dx, y2 - dy, 0.0)); _tricolors.push_back(col);
}

void AppWindow::drawLetterA(double x1, double x2, double x3, double y1, double y2, double y3, double dx, double dy, GsColor col) {
	// x1 = -0.30, x2 = -0.15, x3 = 0.0, y1 = 0.65, y2 = 0.35, y3 = 0.05
	// dx = 0.005, dy = 0.001
	// Drawing Left Slanted Line
	_tricoords.push_back(GsVec(x1, y3, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2, y1 - 2 * dy * sqrt(3), 0.0)); _tricolors.push_back(col);
	
	_tricoords.push_back(GsVec(x1, y3, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y3, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2, y1 - 2 * dy * sqrt(3), 0.0)); _tricolors.push_back(col);
	// Drawing the Right Slanted Line
	_tricoords.push_back(GsVec(x2, y1 - 2 * dy * sqrt(3), 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x3 - dx, y3, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x3, y3, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x2, y1 - 2 * dy * sqrt(3), 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x3, y3, 0.0)); _tricolors.push_back(col);
	// Drawing the Middle Line
	_tricoords.push_back(GsVec((x1 + x2)/2, y2 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec((x1 + x2) / 2, y2 - dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec((x3 + x2) / 2, y2 - dy, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec((x1 + x2) / 2, y2 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec((x3 + x2) / 2, y2 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec((x3 + x2) / 2, y2 - dy, 0.0)); _tricolors.push_back(col);
	//_tricoords.push_back(GsVec()); _tricolors.push_back(col);
}

void AppWindow::drawLetterN(double x1, double x2, double y1, double y2, double dx, double dy, GsColor col) {
	// Drawing left vertical line 
	_tricoords.push_back(GsVec(x1 - dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 - dx, y2, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y2, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x1 - dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y2, 0.0)); _tricolors.push_back(col);
	// Drawing right vertical line
	_tricoords.push_back(GsVec(x2 - dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 - dx, y2, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 + dx, y2, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x2 - dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 + dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2 + dx, y2, 0.0)); _tricolors.push_back(col);
	// Drawing middle horizontal line
	_tricoords.push_back(GsVec(x2, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x2, y1 - 3 * dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1, y2, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x2, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1, y2 + 3 * dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1, y2, 0.0)); _tricolors.push_back(col);

}

void AppWindow::drawLetterG(double x1, double y1, double r, double dQ, double dR, double dx, double dy, GsColor col) {
	// x1
	// Drawing horizontal Line
	_tricoords.push_back(GsVec(x1 - 10 * dx, y1 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 - 10 * dx, y1 - dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + 10 * dx, y1 - dy, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x1 + 10 * dx, y1 - dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + 10 * dx, y1 + dy, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 - 10 * dx, y1 + dy, 0.0)); _tricolors.push_back(col);

	// Drawing smaller black circle
	for (double q = M_PI / 2; q <= M_PI * 3 / 2; q += dQ) {
		_tricoords.push_back(GsVec(x1, y1, 0.0)); _tricolors.push_back(GsColor::black);
		_tricoords.push_back(GsVec(x1 + (r - dR) * cos(q), y1 + 2.25 * (r - dR) * sin(q), 0.0)); _tricolors.push_back(GsColor::black);
		_tricoords.push_back(GsVec(x1 + (r - dR) * cos(q + dQ), y1 + 2 * (r - dR) * sin(q + dQ), 0.0)); _tricolors.push_back(GsColor::black);
	}
	// Covering bigger red circle 
	for (double q = M_PI/2; q < M_PI * 3 / 2; q+=dQ) {
		_tricoords.push_back(GsVec(x1, y1, 0.0)); _tricolors.push_back(col);
		_tricoords.push_back(GsVec(x1 + r * cos(q), y1 + 2.25 * r * sin(q), 0.0)); _tricolors.push_back(col);
		_tricoords.push_back(GsVec(x1 + r * cos(q + dQ), y1 + 2 * r * sin(q + dQ), 0.0)); _tricolors.push_back(col);
	}

	// Drawing vertical Line
	_tricoords.push_back(GsVec(x1 - dx, y1 - 2 * r, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y1 - 2 * r, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 + dx, y1, 0.0)); _tricolors.push_back(col);

	_tricoords.push_back(GsVec(x1 + dx, y1, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 - dx, y1 - 2 * r, 0.0)); _tricolors.push_back(col);
	_tricoords.push_back(GsVec(x1 - dx, y1, 0.0)); _tricolors.push_back(col);

}

void AppWindow::drawLines(double x1, double y1, double x2, double y2, GsColor col) {
	_linecoords.push_back(GsVec(x1, y1, 0.0)); _linecolors.push_back(col);
	_linecoords.push_back(GsVec(x2, y2, 0.0)); _linecolors.push_back(col);
	glBindBuffer(GL_ARRAY_BUFFER, _lines.buf[0]);
	glBufferData(GL_ARRAY_BUFFER, _linecoords.size() * 3 * sizeof(float), &_linecoords[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, _lines.buf[1]);
	glBufferData(GL_ARRAY_BUFFER, _linecolors.size() * 4 * sizeof(gsbyte), &_linecolors[0], GL_STATIC_DRAW);
}
void AppWindow::glutDisplay ()
 {
   // Clear the rendering window
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // Update objects if needed:
   buildObjects();

   // Define some identity transformations - our shaders require them but in this
   // example code we do not need to use them, so just let them be GsMat::id:
   GsMat vtransf(GsMat::id), vproj(GsMat::id); // transformations

   // Draw Lines:
   glUseProgram ( _lines.prog );
   glBindVertexArray ( _lines.va[0] );

   glBindBuffer ( GL_ARRAY_BUFFER, _lines.buf[0] ); // positions
   glEnableVertexAttribArray ( 0 );
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

   glBindBuffer ( GL_ARRAY_BUFFER, _lines.buf[1] ); // colors
   glEnableVertexAttribArray ( 1 );
   glVertexAttribPointer ( 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0 );

   glUniformMatrix4fv ( _lines.uniloc[0], 1, GL_FALSE, vtransf.e );
   glUniformMatrix4fv ( _lines.uniloc[1], 1, GL_FALSE, vproj.e );

   glDrawArrays ( GL_LINES, 0, _linecoords.size() );
   // (see documentation at: https://www.opengl.org/sdk/docs/man/html/glDrawArrays.xhtml)

   // Draw Points:
   glUseProgram ( _pts.prog );
   glBindVertexArray ( _pts.va[0] );

   glBindBuffer ( GL_ARRAY_BUFFER, _pts.buf[0] ); // positions
   glEnableVertexAttribArray ( 0 );
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

   glBindBuffer ( GL_ARRAY_BUFFER, _pts.buf[1] ); // colors
   glEnableVertexAttribArray ( 1 );
   glVertexAttribPointer ( 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0 );

   glUniformMatrix4fv ( _pts.uniloc[0], 1, GL_FALSE, vtransf.e );
   glUniformMatrix4fv ( _pts.uniloc[1], 1, GL_FALSE, vproj.e );

   glDrawArrays ( GL_POINTS, 0, _ptcoords.size() );

   // Draw Triangles:
   glUseProgram ( _tris.prog );
   glBindVertexArray ( _tris.va[0] );

   glBindBuffer ( GL_ARRAY_BUFFER, _tris.buf[0] ); // positions
   glEnableVertexAttribArray ( 0 );
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

   glBindBuffer ( GL_ARRAY_BUFFER, _tris.buf[1] ); // colors
   glEnableVertexAttribArray ( 1 );
   glVertexAttribPointer ( 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0 );

   glUniformMatrix4fv ( _tris.uniloc[0], 1, GL_FALSE, vtransf.e );
   glUniformMatrix4fv ( _tris.uniloc[1], 1, GL_FALSE, vproj.e );

   glDrawArrays ( GL_TRIANGLES, 0, _tricoords.size() );

   // Swap buffers and draw:
   glFlush();         // flush the pipeline (usually not necessary)
   glutSwapBuffers(); // we were drawing to the back buffer, now bring it to the front
}

