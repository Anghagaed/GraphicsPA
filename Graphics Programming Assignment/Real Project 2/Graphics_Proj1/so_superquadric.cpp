
# include "so_superquadric.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

SoSuperquadric::SoSuperquadric()
 {
   _numpoints = 0;
   _flatn = true;
 }

void SoSuperquadric::init ()
 {
	_vshphong.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_gouraud.glsl");
	_fshphong.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_gouraud.glsl");
	_progphong.init_and_link(_vshphong, _fshphong);

	// Define buffers needed:
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(3);       // will use 3 buffers

	_progphong.uniform_locations(9); // will send 9 variables
	_progphong.uniform_location(0, "vTransf");
	_progphong.uniform_location(1, "vProj");
	_progphong.uniform_location(2, "lPos");
	_progphong.uniform_location(3, "la");
	_progphong.uniform_location(4, "ld");
	_progphong.uniform_location(5, "ls");
	_progphong.uniform_location(6, "ka");
	_progphong.uniform_location(7, "ks");
	_progphong.uniform_location(8, "sh");
 }
// Draws Tube
void SoSuperquadric::drawTube(double x_l, double x_r, double z_r, double z_l, double height, double sine_r, double cosine_r, double sine_l, double cosine_l)
{
	GsPnt a, b, c, d; // Points
	a = GsPnt(x_r, height, z_r);
	b = GsPnt(x_l, height, z_l);
	c = GsPnt(x_r, -height, z_r);
	d = GsPnt(x_l, -height, z_l);

	// top triangle
	P.push() = (a); 
	P.push() = (b);
	P.push() = (c);
	
	// bot triangle
	P.push() = (b);
	P.push() = (c); 
	P.push() = (d); 

	GsVec n1, n2, n3, n4;
	const double ns = 0.05f; // how long they appear in the screen
	if (_flatn)
	{
		n1 = normal(a, b, c);
		n2 = normal(a, d, c);
		N.push() = n1; N.push() = n1; N.push() = n1;
		N.push() = n2; N.push() = n2; N.push() = n2;
		n1 *= ns; n2 *= ns;
		GsPnt f1c = (a + c + b) / 3.0f; NL.push() = f1c; NL.push() = f1c + n1;
		GsPnt f2c = (a + d + c) / 3.0f; NL.push() = f2c; NL.push() = f2c + n2;
	}
	else
	{
		// a, b = top
		// c, d = bot
		n1 = GsVec((double)a.x, 0.0, (double)a.z);
		n2 = GsVec((double)b.x, 0.0, (double)b.z);
		n3 = GsVec((double)c.x, 0.0, (double)c.z);
		n4 = GsVec((double)d.x, 0.0, (double)d.z);
		//n1 *= ns; n2 *= ns;
		NL.push() = a; NL.push() = a + n1*ns;
		NL.push() = c; NL.push() = c + n2*ns;
		n1.normalize();
		// top
		N.push() = (n1);
		N.push() = (n2);
		N.push() = (n3);
		// bot
		N.push() = (n2);
		N.push() = (n3);
		N.push() = (n4);
	}
}

// Draws Circle
void SoSuperquadric::drawCircle(double x_l, double x_r, double z_r, double z_l, double height, double sine, double cosine) //, int frame_num, double circle_rate, double radius, double curr_angle_r, double curr_angle_l)
{
	// top
	GsPnt a, b, c, d; // Points
	a = GsPnt(x_r, height, z_r);
	b = GsPnt(x_l, height, z_l);
	c = GsPnt(0.0, height, 0.0);
	P.push() = (a);
	P.push() = (b);
	P.push() = (c);
	GsVec n1, n2, n3, n4;
	const float ns = 0.05f; // how long they appear in the screen
	if (_flatn)
	{
		n1 = normal(a, c, b);
		N.push() = n1; N.push() = n1; N.push() = n1;
		n1 *= ns; 
		GsPnt f1c = (a + c + b) / 3.0f; NL.push() = f1c; NL.push() = f1c + n1;
	}
	else
	{
		n1 = GsVec(0.0, 1.0, 0.0);
		//n2 = GsVec(0.0, 1.0, 0.0);
		//N.push() = n1; N.push() = n1; N.push() = n1;
		//n1 *= ns; //n2 *= ns;
		NL.push() = a; NL.push() = a + n1*ns;
		NL.push() = b; NL.push() = b + n1*ns;
		NL.push() = c; NL.push() = c + n1*ns;
		//n1.normalize();
		N.push() = n1; N.push() = n1; N.push() = n1;
	}
	// bot
	a = GsPnt(x_r, -height, z_r);
	b = GsPnt(x_l, -height, z_l);
	c = GsPnt(0.0, -height, 0.0);

	P.push() = (a);
	P.push() = (b);
	P.push() = (c);

	if (_flatn)
	{
		n1 = normal(a, b, c);
		N.push() = n1; N.push() = n1; N.push() = n1;
		n1 *= ns; n2 *= ns;
		GsPnt f1c = (a + c + b) / 3.0f; NL.push() = f1c; NL.push() = f1c + n1;
	}
	else
	{
		n1 = GsVec(0.0, -1.0, 0.0);
		NL.push() = a; NL.push() = a + n1*ns;
		NL.push() = b; NL.push() = b + n1*ns;
		NL.push() = c; NL.push() = c + n1*ns;
		//n1.normalize();
		N.push() = n1; N.push() = n1; N.push() = n1;
	}
}

void SoSuperquadric::drawCapsule(int nfaces)
{
	// Variables
	double radius = 0.5; // default radius (for top and bot circles)
	double height = 0.5; // 1/2 height of the cylinder
	int frame_num = ((nfaces - 1) / 2); // # of frames for top and bot "circles"
	double circle_rate = (M_PI / 2.0) / frame_num; // rate of angle that changes the radius of "circle"

	// (Degrees) * (Radian Conversion) = Radian
	double angle_rate = 360.0 / nfaces * M_PI / 180; // rate the triangle rotates
	double angle_l = -360.0 / nfaces * M_PI / 180 / 2; // default left side
	double angle_r = 360.0 / nfaces * M_PI / 180 / 2; // default right side

	for (int i = 0; i < nfaces; i++)
	{
		// side of the rectangles in the tube setion
		double x_r = radius * (double)sin(angle_r + (angle_rate * i));
		double x_l = radius * (double)sin(angle_l + (angle_rate * i));

		// apothem of the rectangles in the tube section
		double z_r = radius * (double)cos(angle_r + (angle_rate * i));
		double z_l = radius * (double)cos(angle_l + (angle_rate * i));

		drawTube(x_l, x_r, z_r, z_l, height, sin(angle_r + (angle_rate * i)), cos(angle_r + (angle_rate * i)), sin(angle_l + (angle_rate * i)), cos(angle_l + (angle_rate * i))); // draws tube
		drawCircle(x_l, x_r, z_r, z_l, height, sin(angle_r + (angle_rate * i)), cos(angle_r + (angle_rate * i))); // draws "circles"
	}
}

void SoSuperquadric::build(int nfaces)
{
	if (nfaces < 3) // Error check
	{
		std::cout << "nfaces should not be less than 3. nfaces currently is " << nfaces << std::endl;
		return;
	}
	P.size(0); C.size(0); N.size(0); NL.size(0); // set size to zero, just in case

	drawCapsule(nfaces); // draws "Capsule"

	C.size(P.size()); C.setall(GsColor::green);
	_mtl.specular.set(255, 255, 255);
	_mtl.shininess = 8; // increase specular effect

	// send data to OpenGL buffers:
	glBindVertexArray(va[0]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// P
	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float)*P.size(), P.pt(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// N
	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float)*N.size(), N.pt(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// C
	glBindBuffer(GL_ARRAY_BUFFER, buf[2]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(gsbyte)*C.size(), C.pt(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);

	glBindVertexArray(0); // break the existing vertex array object binding.

	// save size so that we can free our buffers and later just draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.capacity(0); C.capacity(0); N.capacity(0);
}
void SoSuperquadric::draw ( const GsMat& tr, const GsMat& pr, const GsLight& l )
 {
   float f[4]; 
   float sh = (float)_mtl.shininess;
   if ( sh<0.001f ) sh=64;

   
      glUseProgram ( _progphong.id );
      glUniformMatrix4fv ( _progphong.uniloc[0], 1, GL_FALSE, tr.e );
      glUniformMatrix4fv ( _progphong.uniloc[1], 1, GL_FALSE, pr.e );
      glUniform3fv ( _progphong.uniloc[2], 1, l.pos.e );
      glUniform4fv ( _progphong.uniloc[3], 1, l.amb.get(f) );
      glUniform4fv ( _progphong.uniloc[4], 1, l.dif.get(f) );
      glUniform4fv ( _progphong.uniloc[5], 1, l.spe.get(f) );
      glUniform4fv ( _progphong.uniloc[6], 1, _mtl.ambient.get(f) );
      glUniform4fv ( _progphong.uniloc[7], 1, _mtl.specular.get(f) );
      glUniform1fv ( _progphong.uniloc[8], 1, &sh );
    
   glBindVertexArray ( va[0] );
   glDrawArrays ( GL_TRIANGLES, 0, _numpoints );
   glBindVertexArray(0); // break the existing vertex array object binding.
 }

