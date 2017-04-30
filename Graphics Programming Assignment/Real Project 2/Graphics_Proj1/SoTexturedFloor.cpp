
# include "SoTexturedFloor.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

SoTexturedFloor::SoTexturedFloor()
{
	_numpoints = 0;
	_flatn = true;
}

void SoTexturedFloor::init()
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
	if (!I.load("../pic2.png"))
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


void SoTexturedFloor::drawFloor()
{
	// Variables

	GsVec a, b, c, d;
	a = GsVec(10, 0, 10);
	b = GsVec(10, 0, -10);
	c = GsVec(-10, 0, -10);
	d = GsVec(-10, 0, 10);

	P.push() = c;
	P.push() = b;
	P.push() = a;
	P.push() = a;
	P.push() = d;
	P.push() = c;

	GsVec n1, n2, n3, n4;
	n1 = GsVec((double)a.x, 0.0, (double)a.z);
	n2 = GsVec((double)b.x, 0.0, (double)b.z);
	n3 = GsVec((double)c.x, 0.0, (double)c.z);
	n4 = GsVec((double)d.x, 0.0, (double)d.z);
	n1.normalize();
	N.push() = (n3);
	N.push() = (n2);
	N.push() = (n1);
	N.push() = (n1);
	N.push() = (n4);
	N.push() = (n3);

	a = GsVec2(0.0f, 0.0f);
	b = GsVec2(1.0f, 0.0f);
	c = GsVec2(1.0f, 1.0f);
	d = GsVec2(0.0f, 1.0f);
	T.push() = c;
	T.push() = d;
	T.push() = a;
	T.push() = a;
	T.push() = b;
	T.push() = c;
	
}

void SoTexturedFloor::build()
{
	P.size(0); T.size(0); N.size(0); NL.size(0); // set size to zero, just in case

	drawFloor(); // draws Floor

	_mtl.diffuse.set(255, 255, 255);
	_mtl.shininess = 2; // increase specular effect

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
void SoTexturedFloor::draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs)
{
	float modify[16] = {
		1, 0, 0, 0,
		0, 1, 0, -0.001,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	GsMat mod(modify);
	GsMat result = tr*mod;
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
	
	glUniformMatrix4fv(_prog.uniloc[0], 1, GL_FALSE, result.e);
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

