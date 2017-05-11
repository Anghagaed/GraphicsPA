#include "SoLeg.h"

using namespace std;

SoLeg::SoLeg()
{
	_numpoints = 0;
	r = 0;
	x = 0;
	y = 0;
	z = 0;
	l = 0;
	_nfaces = 0;
}

void SoLeg::init(float x, float y, float z, string imagePath)
{
	this->x = x; this->y = y; this->z = z; this->imagePath = imagePath;

	_vshphong.load_and_compile(GL_VERTEX_SHADER, "../texgouraud.vert");
	_fshphong.load_and_compile(GL_FRAGMENT_SHADER, "../texgouraud.frag");
	_progphong.init_and_link(_vshphong, _fshphong);

	// Define buffers needed:
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(3);       // will use 3 buffers

	_progphong.uniform_locations(12); // will send 10 variables
	_progphong.uniform_location(0, "vTransf");
	_progphong.uniform_location(1, "vProj");
	_progphong.uniform_location(2, "lPos");
	_progphong.uniform_location(3, "la");
	_progphong.uniform_location(4, "ld");
	_progphong.uniform_location(5, "ls");
	_progphong.uniform_location(6, "ka");
	_progphong.uniform_location(7, "kd");
	_progphong.uniform_location(8, "ks");
	_progphong.uniform_location(9, "sh");
	_progphong.uniform_location(10, "Tex1");
	_progphong.uniform_location(11, "fs");

	// Copy paste here
	GsImage I;
	gsuint id;
	if (!I.load(imagePath.c_str())) { std::cout << "COULD NOT LOAD IMAGE!\n"; exit(1); }

	glGenTextures(1, &_texid);
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

void SoLeg::calculateParameters() {
	int layers = _nfaces / 4;
	double dQ = TWOPI / _nfaces;
	for (double i = (TWOPI + dQ); i >= 0; i -= dQ) {
		dx.push_back(cos(i));
		dz.push_back(sin(i));
	}
}

void SoLeg::buildBody() {
	for (int i = 0; i < dx.size() - 1; ++i) {
		GsPnt a, b, c, d;
		a = GsPnt(x + r * dx[i], y + l / 2, z + r * dz[i]);
		b = GsPnt(x + r * dx[i + 1], y + l / 2, z + r * dz[i + 1]);
		c = GsPnt(x + r * dx[i + 1], y - (l / 2), z + r * dz[i + 1]);
		d = GsPnt(x + r * dx[i], y - (l / 2), z + r * dz[i]);

		P.push() = a;
		P.push() = b;
		P.push() = c;

		P.push() = a;
		P.push() = d;
		P.push() = c;

		T.push_back(GsVec2(((float)i) / _nfaces, 0));						// for point a
		T.push_back(GsVec2(((float)i + 1.0f) / _nfaces, 0));				// for point b
		T.push_back(GsVec2(((float)i + 1.0f) / _nfaces, 1));				// for point c

		T.push_back(GsVec2(((float)i) / _nfaces, 0));						// for point a
		T.push_back(GsVec2(((float)i) / _nfaces, 1));						// for point d
		T.push_back(GsVec2(((float)i + 1.0f) / _nfaces, 1));				// for point c

		GsVec n1, n2;
		n1 = GsVec(a.x, 0, a.z); n1.normalize(); N.push() = n1;
		n1 = GsVec(b.x, 0, b.z); n1.normalize(); N.push() = n1;
		n1 = GsVec(c.x, 0, c.z); n1.normalize(); N.push() = n1;

		n2 = GsVec(a.x, 0, a.z); n2.normalize(); N.push() = n2;
		n2 = GsVec(d.x, 0, d.z); n2.normalize(); N.push() = n2;
		n2 = GsVec(c.x, 0, c.z); n2.normalize(); N.push() = n2;
	}
}

void SoLeg::buildHat() {
	for (int i = 0; i < dx.size() - 1; ++i) {
		GsPnt a, b, c;
		GsVec n;
		a = GsPnt(x, y + l / 2, z);
		b = GsPnt(x + r * dx[i], y + l / 2, z + r * dz[i]);
		c = GsPnt(x + r * dx[i + 1], y + l / 2, z + r * dz[i + 1]);

		// a, b, c
		P.push() = a;  P.push() = b; P.push() = c;

		n = GsVec(0.0f, 1.0f, 0.0f);
		N.push() = n; N.push() = n; N.push() = n;

		a = GsPnt(x, y - (l / 2), z);
		b = GsPnt(x + r * dx[i], y - (l / 2), z + r * dz[i]);
		c = GsPnt(x + r * dx[i + 1], y - (l / 2), z + r * dz[i + 1]);

		// a, b, c
		P.push() = a;  P.push() = b; P.push() = c;

		n = GsVec(0.0f, -1.0f, 0.0f);
		N.push() = n; N.push() = n; N.push() = n;

	}
}

void SoLeg::build(float r, float l, int _nfaces)
{
	T.clear();
	this->r = r; this->l = l; this->_nfaces = _nfaces;

	this->calculateParameters();
	this->buildBody();

	_numpoints = P.size();


	glBindVertexArray(va[0]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float)*P.size(), P.pt(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float)*N.size(), N.pt(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buf[2]);
	glBufferData(GL_ARRAY_BUFFER, T.size() * 2 * sizeof(float), &T[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0); // break the existing vertex array object binding.


						  // free non-needed memory:
	P.capacity(0); T.clear(); N.capacity(0);
	dx.clear(); dz.clear();
	//_mtl.specular.set(255, 255, 255);
	_mtl.diffuse.set(255, 0, 0);
	_mtl.shininess = 8; // increase specular effect
}

GsVec SoLeg::center() {
	return GsVec(x, y, z);
}

GsVec SoLeg::firstJointPts() {
	return GsVec(0.0f, l / 2, 0.0f);
}

GsVec SoLeg::secondJointPts() {
	return GsVec(0.0f, -l / 2, 0.0f);
}

void SoLeg::draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs)
{
	float f[4];
	float sh = (float)_mtl.shininess;
	if (sh<0.001f) sh = 64;

	glUseProgram(_progphong.id);

	glBindVertexArray(va[0]);
	glBindTexture(GL_TEXTURE_2D, _texid);

	// Updates uniforms
	glUniformMatrix4fv(_progphong.uniloc[0], 1, GL_FALSE, tr.e);
	glUniformMatrix4fv(_progphong.uniloc[1], 1, GL_FALSE, pr.e);
	glUniform3fv(_progphong.uniloc[2], 1, l.pos.e);
	glUniform4fv(_progphong.uniloc[3], 1, l.amb.get(f));
	glUniform4fv(_progphong.uniloc[4], 1, l.dif.get(f));
	glUniform4fv(_progphong.uniloc[5], 1, l.spe.get(f));
	glUniform4fv(_progphong.uniloc[6], 1, _mtl.ambient.get(f));
	glUniform4fv(_progphong.uniloc[7], 1, _mtl.diffuse.get(f));
	glUniform4fv(_progphong.uniloc[8], 1, _mtl.specular.get(f));
	glUniform1fv(_progphong.uniloc[9], 1, &sh);
	glUniform1ui(_progphong.uniloc[10], _texid);
	glUniform1fv(_progphong.uniloc[11], 1, &fs);

	glBindVertexArray(va[0]);
	glDrawArrays(GL_TRIANGLES, 0, _numpoints);
	glBindVertexArray(0); // break the existing vertex array object binding.

}

