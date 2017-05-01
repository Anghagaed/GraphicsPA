
# include "SoLeg.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

SoLeg::SoLeg()
{
	_numpoints = 0;
	_flatn = false;
}

void SoLeg::init()
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
	if (!I.load("../metaltexture.jpg"))
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

	I.init(0, 0); // free image from CPU
}

void SoLeg::updateTexture(bool which)
{/*
 if (!which) // Texture # 1
 {

 GsImage I;
 gsuint id;
 if (!I.load("../meme.png"))
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
 else // Texture # 2
 {

 GsImage I;
 gsuint id;
 if (!I.load("../meme.png"))
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
 */
}

// Draws Tube
void SoLeg::drawTube(double radiusTop, double radiusBot, double height, double sine_r, double cosine_r, double sine_l, double cosine_l)
{
	// Get a,b,c,d
	// Get top coordinates
	double radius = radiusTop;
	// side of the rectangles in the tube setion
	double x_r = radius * sine_r;
	double x_l = radius * sine_l;
	// apothem of the rectangles in the tube section
	double z_r = radius * cosine_r;
	double z_l = radius * cosine_l;
	GsPnt a, b, c, d; // Points
	a = GsPnt(x_r, height, z_r);
	b = GsPnt(x_l, height, z_l);

	// Get bot coordinates
	radius = radiusBot;
	x_r = radius * sine_r;
	x_l = radius * sine_l;
	z_r = radius * cosine_r;
	z_l = radius * cosine_l;

	c = GsPnt(x_r, -height, z_r);
	d = GsPnt(x_l, -height, z_l);

	// top triangle
	P.push() = (a);
	P.push() = (b);
	P.push() = (c);

	// bot triangle
	P.push() = (d);
	P.push() = (c);
	P.push() = (b);

	GsVec n1, n2, n3, n4;
	const double ns = 0.25f; // how long they appear in the screen

	if (_flatn)
	{
		n1 = normal(a, b, c);
		n2 = normal(a, d, c);
		N.push() = n1; N.push() = n1; N.push() = n1;
		N.push() = n2; N.push() = n2; N.push() = n2;
		n1 *= ns; n2 *= ns;
	}
	else
	{
		// a, b = top
		// c, d = bot
		n1 = GsVec((double)a.x, 0.0, (double)a.z);
		n2 = GsVec((double)b.x, 0.0, (double)b.z);
		n3 = GsVec((double)c.x, 0.0, (double)c.z);
		n4 = GsVec((double)d.x, 0.0, (double)d.z);
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
void SoLeg::drawCircle(double radiusTop, double radiusBot, double height, double sine_r, double cosine_r, double sine_l, double cosine_l) //, int frame_num, double circle_rate, double radius, double curr_angle_r, double curr_angle_l)
{
	// Get top coordinates
	double radius = radiusTop;
	// side of the rectangles in the tube setion
	double x_r = radius * sine_r;
	double x_l = radius * sine_l;
	// apothem of the rectangles in the tube section
	double z_r = radius * cosine_r;
	double z_l = radius * cosine_l;
	GsPnt a, b, c; // Points
	a = GsPnt(x_r, height, z_r);
	b = GsPnt(x_l, height, z_l);


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
	}
	else
	{
		n1 = GsVec(0.0, 1.0, 0.0);
		N.push() = n1; N.push() = n1; N.push() = n1;
	}
	// bot
	// Get top coordinates
	radius = radiusBot;
	// side of the rectangles in the tube setion
	x_r = radius * sine_r;
	x_l = radius * sine_l;
	// apothem of the rectangles in the tube section
	z_r = radius * cosine_r;
	z_l = radius * cosine_l;

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
	}
	else
	{
		n1 = GsVec(0.0, -1.0, 0.0);
		N.push() = n1; N.push() = n1; N.push() = n1;
	}
}

void SoLeg::drawCapsule(int nfaces, double radiusTop, double radiusBot, double height)
{
	// Variables
	if (radiusTop < 0 || radiusBot < 0)
	{
		std::cout << "TOP OR BOT RADIUS IS NEGATIVE!\n";
		return;
	}
	//double height = 1.0; // 1/2 height of the cylinder
	int frame_num = ((nfaces - 1) / 2); // # of frames for top and bot "circles"
	double circle_rate = (M_PI / 2.0) / frame_num; // rate of angle that changes the radius of "circle"

												   // (Degrees) * (Radian Conversion) = Radian
	double angle_rate = 360.0 / nfaces * M_PI / 180; // rate the triangle rotates
	double angle_l = -360.0 / nfaces * M_PI / 180 / 2; // default left side
	double angle_r = 360.0 / nfaces * M_PI / 180 / 2; // default right side

	float textRate = 1.0f / nfaces;

	for (int i = 0; i < nfaces; i++)
	{
		// side of the rectangles in the tube setion
		double sine_r = (double)sin(angle_r + (angle_rate * i));
		double sine_l = (double)sin(angle_l + (angle_rate * i));

		// apothem of the rectangles in the tube section
		double cosine_r = (double)cos(angle_r + (angle_rate * i));
		double cosine_l = (double)cos(angle_l + (angle_rate * i));

		drawTube(radiusTop, radiusBot, height, sine_r, cosine_r, sine_l, cosine_l); // draws tube

		GsVec a, b, c, d;
		a = GsVec2(textRate*(i + 1), 1.0f);
		b = GsVec2(textRate*i, 1.0f);
		c = GsVec2(textRate*(i + 1), 0.0f);
		d = GsVec2(textRate*i, 0.0f);
		T.push() = c;
		T.push() = d;
		T.push() = a;

		T.push() = b;
		T.push() = a;
		T.push() = d;
		//drawCircle(radiusTop, radiusBot, height, sine_r, cosine_r, sine_l, cosine_l); // draws "circles"
	}
}

void SoLeg::build(int nfaces, double radiusTop, double radiusBot, double height)
{
	if (nfaces < 3) // Error check
	{
		std::cout << "nfaces should not be less than 3. nfaces currently is " << nfaces << std::endl;
		return;
	}
	P.size(0); T.size(0); N.size(0); NL.size(0); // set size to zero, just in case

	drawCapsule(nfaces, radiusTop, radiusBot, height); // draws "Capsule"

	_mtl.specular.set(255, 255, 255);
	_mtl.shininess = 30; // increase specular effect

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

	// T
	glBindBuffer(GL_ARRAY_BUFFER, buf[2]);
	glBufferData(GL_ARRAY_BUFFER, T.size() * 2 * sizeof(float), T.pt(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);


	glBindVertexArray(0); // break the existing vertex array object binding.

						  // save size so that we can free our buffers and later just draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.capacity(0); N.capacity(0); T.capacity(0);
}
void SoLeg::draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs)
{
	if (fs < 0)
	{
		std::cout << "NEGATIVE FS! INCREASE FS!\n";
		return;
	}
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

