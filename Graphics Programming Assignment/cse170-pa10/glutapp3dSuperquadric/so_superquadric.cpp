
# include "so_superquadric.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

SoSuperquadric::SoSuperquadric()
 {
   _numpoints = 0;
   _flatn = true;
   _displayTexture = false;
 }

void SoSuperquadric::init ()
 {
	_vsh.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_gouraud.glsl");
	_fsh.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_gouraud.glsl");
	_prog.init_and_link(_vsh, _fsh);

	// Define buffers needed:
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(3);       // will use 3 buffers

	_prog.uniform_locations(9); // will send 9 variables
	_prog.uniform_location(0, "vTransf");
	_prog.uniform_location(1, "vProj");
	_prog.uniform_location(2, "lPos");
	_prog.uniform_location(3, "la");
	_prog.uniform_location(4, "ld");
	_prog.uniform_location(5, "ls");
	_prog.uniform_location(6, "ka");
	_prog.uniform_location(7, "ks");
	_prog.uniform_location(8, "sh");
 }

void SoSuperquadric::switchDisplay(bool which)
{
	_displayTexture = which;
	if (which) // Texture
	{
		_vsh.load_and_compile(GL_VERTEX_SHADER, "../texgouraud.vert");
		_fsh.load_and_compile(GL_FRAGMENT_SHADER, "../texgouraud.frag");
		_prog.init_and_link(_vsh, _fsh);

		// Define buffers needed:
		gen_vertex_arrays(1); // will use 1 vertex array
		gen_buffers(3);       // will use 3 buffers

		_prog.uniform_locations(12); // will send 9 variables

		_prog.uniform_location(0, "vTransf");
		_prog.uniform_location(1, "vProj");
		_prog.uniform_location(2, "lPos");
		_prog.uniform_location(3, "la");
		_prog.uniform_location(4, "ld");
		_prog.uniform_location(5, "ls");
		_prog.uniform_location(6, "ka");
		_prog.uniform_location(7, "kd");
		_prog.uniform_location(8, "ks");
		_prog.uniform_location(9, "sh");
		_prog.uniform_location(10, "Tex1");
		_prog.uniform_location(11, "fs");
		GsImage I;
		gsuint id;
		if (!I.load("../fe.png"))
		{
			std::cout << "COULD NOT LOAD IMAGE!\n"; exit(1);
		}
		glGenTextures(1, &_texid); // generated ids start at 1
		glBindTexture(GL_TEXTURE_2D, _texid);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, I.w(), I.h(), 0, GL_RGBA, GL_UNSIGNED_BYTE, I.data());

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		I.init(0, 0);
	}
	else // Nontexture
		init();
}

void SoSuperquadric::texturize(GsVec x)
{
	double u = 0.5 + (double)atan2(x.z,x.x)/(2*M_PI);
	double v = 0.5 - (double)asin(x.y)/M_PI;
	T.push() = GsVec2(u, v);
}

// Draws Sphere
void SoSuperquadric::drawSphere(int nfaces)
{
	// Variables
	double radius = 1;								// default radius (for top and bot circles)
	double old_radius = radius;							// old radius
	double new_radius;									// new radius
	double height = 1;								// 1/2 height of the cylinder
	double old_height = 0.0;							// old height
	double new_height;									// new height
	int frame_num = ((nfaces - 1) / 2);					// # of frames for top and bot semispheres
	double x_r, x_l, z_r, z_l;							// coordinates helper
	GsVec a, b, c, d;									// points to plug in
	GsVec n1, n2, n3, n4;								// normal vectors

	// (Degrees) * (Radian Conversion) = Radian
	double angle_rate = 360.0 / nfaces * M_PI / 180;	// rate the triangle rotates
	double angle_l = -360.0 / nfaces * M_PI / 180 / 2;	// default left side
	double angle_r = 360.0 / nfaces * M_PI / 180 / 2;	// default right side
	double vertical_angle_rate = (M_PI / 2) / frame_num;

	for (int i = 0; i < nfaces; ++i)
	{
		for (int j = 0; j < frame_num; ++j)
		{
			// calculate a,b
			// side of the rectangles in the tube setion
			x_r = old_radius * (double)sin(angle_r + (angle_rate * i));
			x_l = old_radius * (double)sin(angle_l + (angle_rate * i));

			// apothem of the rectangles in the tube section
			z_r = old_radius * (double)cos(angle_r + (angle_rate * i));
			z_l = old_radius * (double)cos(angle_l + (angle_rate * i));

			a = GsVec(x_l, old_height, z_l);
			b = GsVec(x_r, old_height, z_r);

			// calculate c,d
			new_radius = radius * (double)cos(vertical_angle_rate * (j + 1));
			new_height = height * (double)sin(vertical_angle_rate * (j + 1));

			// side of the rectangles in the tube setion
			x_r = new_radius * (double)sin(angle_r + (angle_rate * i));
			x_l = new_radius * (double)sin(angle_l + (angle_rate * i));

			// apothem of the rectangles in the tube section
			z_r = new_radius * (double)cos(angle_r + (angle_rate * i));
			z_l = new_radius * (double)cos(angle_l + (angle_rate * i));

			c = GsVec(x_l, new_height, z_l);
			d = GsVec(x_r, new_height, z_r);

			// top semisphere
			// top triangle: a,b,c
			// top side
			P.push() = a;
			P.push() = b;
			P.push() = c;
			texturize(a);
			texturize(b);
			texturize(c);

			// bot triangle: d,c,b
			P.push() = d;
			P.push() = c;
			P.push() = b;
			texturize(d);
			texturize(c);
			texturize(b);

			// let's try flat shading for now
			if (_flatn)					// flat shading
			{
				n1 = normal(a, b, c);
				n2 = normal(d, c, b);
				N.push() = n1;
				N.push() = n1;
				N.push() = n1;
				N.push() = n2;
				N.push() = n2;
				N.push() = n2;
			}
			else						// smooth shading
			{
				n1 = a;
				n2 = b;
				n3 = c;
				n4 = d;
				N.push() = n1;
				N.push() = n2;
				N.push() = n3;
				N.push() = n4;
				N.push() = n3;
				N.push() = n2;
			}
			// bot semisphere
			a.y *= -1;
			b.y *= -1;
			c.y *= -1;
			d.y *= -1;
			
			// top side
			P.push() = a;
			P.push() = b;
			P.push() = c;
			texturize(a);
			texturize(b);
			texturize(c);
			
			// bot triangle: d,c,b
			P.push() = d;
			P.push() = c;
			P.push() = b;
			texturize(d);
			texturize(c);
			texturize(b);

			if (_flatn)					// flat shading
			{
				n1 = normal(c, b, a);
				n2 = normal(b, c, d);
				N.push() = n1;
				N.push() = n1;
				N.push() = n1;
				N.push() = n2;
				N.push() = n2;
				N.push() = n2;
			}
			else						// smooth shading
			{
				n1 = a;
				n2 = b;
				n3 = c;
				n4 = d;
				N.push() = n1;
				N.push() = n2;
				N.push() = n3;
				N.push() = n4;
				N.push() = n3;
				N.push() = n2;
			}
			// update old_height and old_radius
			old_radius = new_radius;
			old_height = new_height;
		}
		// reset old_height and old_radius
		old_radius = radius;
		old_height = 0.0;
	}
}

void SoSuperquadric::build(int nfaces)
{
	if (nfaces < 3) // Error check
	{
		std::cout << "nfaces should not be less than 3. nfaces currently is " << nfaces << std::endl;
		return;
	}
	P.size(0); C.size(0); N.size(0);  // set size to zero, just in case

	drawSphere(nfaces); // draws "Capsule"

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

	if (!_displayTexture)	// no texture
	{
		// C
		glBindBuffer(GL_ARRAY_BUFFER, buf[2]);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(gsbyte)*C.size(), C.pt(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
	}
	else					// texture
	{
		// T
		glBindBuffer(GL_ARRAY_BUFFER, buf[2]);
		glBufferData(GL_ARRAY_BUFFER, T.size() * 2 * sizeof(float), T.pt(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	glBindVertexArray(0); // break the existing vertex array object binding.

	// save size so that we can free our buffers and later just draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.capacity(0); C.capacity(0); N.capacity(0);
}
void SoSuperquadric::draw ( const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs, const bool displayWireFrame)
 {
	if (fs < 0)
	{
		std::cout << "NEGATIVE FS! INCREASE FS!\n";
		return;
	}
	if (!_displayTexture)		// no texture
	{
		float f[4];
		float sh = (float)_mtl.shininess;
		if (sh<0.001f) sh = 64;


		glUseProgram(_prog.id);
		glUniformMatrix4fv(_prog.uniloc[0], 1, GL_FALSE, tr.e);
		glUniformMatrix4fv(_prog.uniloc[1], 1, GL_FALSE, pr.e);
		glUniform3fv(_prog.uniloc[2], 1, l.pos.e);
		glUniform4fv(_prog.uniloc[3], 1, l.amb.get(f));
		glUniform4fv(_prog.uniloc[4], 1, l.dif.get(f));
		glUniform4fv(_prog.uniloc[5], 1, l.spe.get(f));
		glUniform4fv(_prog.uniloc[6], 1, _mtl.ambient.get(f));
		glUniform4fv(_prog.uniloc[7], 1, _mtl.specular.get(f));
		glUniform1fv(_prog.uniloc[8], 1, &sh);

		glBindVertexArray(va[0]);
		if (displayWireFrame)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, _numpoints);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, _numpoints);
		}
		glBindVertexArray(0); // break the existing vertex array object binding.
	}
	else
	{
		float f[4];
		float sh = (float)_mtl.shininess;
		if (sh<0.001f) sh = 64;

		glUseProgram(_prog.id);
		glBindVertexArray(va[0]);
		glBindTexture(GL_TEXTURE_2D, _texid);

		glUniformMatrix4fv(_prog.uniloc[0], 1, GL_FALSE, tr.e);
		glUniformMatrix4fv(_prog.uniloc[1], 1, GL_FALSE, pr.e);
		glUniform3fv(_prog.uniloc[2], 1, l.pos.e);
		glUniform4fv(_prog.uniloc[3], 1, l.amb.get(f));
		glUniform4fv(_prog.uniloc[4], 1, l.dif.get(f));
		glUniform4fv(_prog.uniloc[5], 1, l.spe.get(f));
		glUniform4fv(_prog.uniloc[6], 1, _mtl.ambient.get(f));
		glUniform4fv(_prog.uniloc[7], 1, _mtl.diffuse.get(f));
		glUniform4fv(_prog.uniloc[8], 1, _mtl.specular.get(f));
		glUniform1fv(_prog.uniloc[9], 1, &sh);
		glUniform1ui(_prog.uniloc[10], _texid);
		glUniform1fv(_prog.uniloc[11], 1, &fs);

		glBindVertexArray(va[0]);
		glDrawArrays(GL_TRIANGLES, 0, _numpoints);
		glBindVertexArray(0); // break the existing vertex array object binding.
	}
 }

