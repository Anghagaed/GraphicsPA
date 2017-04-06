#include "so_texturedsphere.h"

using namespace std;

SoTexturedSphere::SoTexturedSphere()
{
	_numpoints = 0;
	r = 0;
	center = GsPnt(0, 0, 0);
	_layers = 0;
	_lfaces = 0;
	_numfaces = 0;

	initialPos.identity();

	myTrans.identity();

	shadow.identity();

	shadowPath = "../shadow.png";
}

void SoTexturedSphere::setInitialPos(const GsMat& posMat) {
	initialPos = posMat;
}
void SoTexturedSphere::setMyTrans(const GsMat& transMat) {
	myTrans = transMat;
}
void SoTexturedSphere::setShadow(const GsMat& shadowMat) {
	shadow = shadowMat;
}
void SoTexturedSphere::resetMyTrans() {
	myTrans.identity();
}

void SoTexturedSphere::init(float x, float y, float z, string& imagePath)
{
	center = GsPnt(x, y, z); this->imagePath = imagePath;

	_vshphong.load_and_compile(GL_VERTEX_SHADER, "../texgouraud.vert");
	_fshphong.load_and_compile(GL_FRAGMENT_SHADER, "../texgouraud.frag");
	_progphong.init_and_link(_vshphong, _fshphong);

	// Define buffers needed:
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(3);       // will use 3 buffers

	_progphong.uniform_locations(11); // will send 10 variables
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

	// Copy paste here
	GsImage I1, I2;
	gsuint id1, id2;
	if (!I1.load(imagePath.c_str())) { std::cout << "COULD NOT LOAD IMAGE!\n"; exit(1); }

	glGenTextures(1, &_texid1);
	glBindTexture(GL_TEXTURE_2D, _texid1);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, I1.w(), I1.h(), 0, GL_RGBA, GL_UNSIGNED_BYTE, I1.data());

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	I1.init(0, 0); // free image from CPU

	if (!I2.load(shadowPath.c_str())) { std::cout << "COULD NOT LOAD IMAGE!\n"; exit(1); }

	glGenTextures(1, &_texid2);
	glBindTexture(GL_TEXTURE_2D, _texid2);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, I2.w(), I2.h(), 0, GL_RGBA, GL_UNSIGNED_BYTE, I2.data());

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	I2.init(0, 0); // free image from CPU
}

void SoTexturedSphere::calculateParameters() {
	_numfaces = _lfaces * _layers;
	float dQ = TWOPI / _lfaces;
	for (float i = 0; i <= (TWOPI + dQ); i += dQ) {
		dx.push_back(cos(i));
		dz.push_back(sin(i));
	}

	dQ = (PIOVER2) / _layers;
	for (float i = 0; i <= (PIOVER2); i += dQ) {
		dy.push_back(i);
	}
}

float SoTexturedSphere::findU(GsPnt P) {
	return (0.5 + (atan2(P.z - center.z, P.x - center.x)) / TWOPI);
}

float SoTexturedSphere::findV(GsPnt P) {
	return (0.5 - (asin(P.y - center.y)) / PI);
}

void SoTexturedSphere::buildTop() {
	float x, y, z;
	x = center.x; y = center.y; z = center.z;
	for (int j = 0; j < dy.size() - 1; ++j) {
		for (int i = 0; i < dx.size() - 1; ++i) {
			float x1, x2, x3, x4, y1, y2, z1, z2, z3, z4;
			x1 = x + r * sin(dy[j]) * dx[i];		 x2 = x + r * sin(dy[j]) * dx[i + 1];
			x3 = x + r * sin(dy[j + 1]) * dx[i + 1];	 x4 = x + r * sin(dy[j + 1]) * dx[i];

			y1 = y + r * cos(dy[j]);				 y2 = y + r * cos(dy[j + 1]);

			z1 = z + r * sin(dy[j]) * dz[i];		 z2 = z + r * sin(dy[j]) * dz[i + 1];
			z3 = z + r * sin(dy[j + 1]) * dz[i + 1]; z4 = z + r * sin(dy[j + 1]) * dz[i];
			GsPnt a, b, c, d;
			a = GsPnt(x1, y1, z1);
			b = GsPnt(x2, y1, z2);
			c = GsPnt(x3, y2, z3);
			d = GsPnt(x4, y2, z4);

			// a, b, c
			P.push() = a; P.push() = b; P.push() = c;
			// a, d, c
			P.push() = a; P.push() = d; P.push() = c;

			// Texture
			// a, b, c
			T.push_back(GsVec2(findU(a), findV(a)));
			T.push_back(GsVec2(findU(b), findV(b)));
			T.push_back(GsVec2(findU(c), findV(c)));
			// a, d, c
			T.push_back(GsVec2(findU(a), findV(a)));
			T.push_back(GsVec2(findU(d), findV(d)));
			T.push_back(GsVec2(findU(c), findV(c)));
			// Normals

			GsVec n1, n2;
			// a, b, c
			n1 = GsVec(a.x - x, a.y - y, a.z - z); n1.normalize(); N.push() = n1;
			n1 = GsVec(b.x - x, b.y - y, b.z - z); n1.normalize(); N.push() = n1;
			n1 = GsVec(c.x - x, c.y - y, c.z - z); n1.normalize(); N.push() = n1;

			// a, d, c
			n2 = GsVec(a.x - x, a.y - y, a.z - z); n2.normalize(); N.push() = n2;
			n2 = GsVec(d.x - x, d.y - y, d.z - z); n2.normalize(); N.push() = n2;
			n2 = GsVec(c.x - x, c.y - y, c.z - z); n2.normalize(); N.push() = n2;
		}
	}
}

void SoTexturedSphere::buildBottom() {
	float x, y, z;
	x = center.x; y = center.y; z = center.z;
	for (int j = 0; j < dy.size() - 1; ++j) {
		for (int i = 0; i < dx.size() - 1; ++i) {
			float x1, x2, x3, x4, y1, y2, z1, z2, z3, z4;
			x1 = x + r * sin(dy[j]) * dx[i];			 x2 = x + r * sin(dy[j]) * dx[i + 1];
			x3 = x + r * sin(dy[j + 1]) * dx[i + 1];	 x4 = x + r * sin(dy[j + 1]) * dx[i];

			y1 = y + r * cos(dy[j]);					 y2 = y + r * cos(dy[j + 1]);
			y1 *= -1;									 y2 *= -1;

			z1 = z + r * sin(dy[j]) * dz[i];			 z2 = z + r * sin(dy[j]) * dz[i + 1];
			z3 = z + r * sin(dy[j + 1]) * dz[i + 1];	 z4 = z + r * sin(dy[j + 1]) * dz[i];
			GsPnt a, b, c, d;
			a = GsPnt(x1, y1, z1);
			b = GsPnt(x2, y1, z2);
			c = GsPnt(x3, y2, z3);
			d = GsPnt(x4, y2, z4);

			// a, b, c
			P.push() = a; P.push() = b; P.push() = c;
			// a, d, c
			P.push() = a; P.push() = d; P.push() = c;

			// Texture
			// a, b, c
			T.push_back(GsVec2(findU(a), findV(a)));
			T.push_back(GsVec2(findU(b), findV(b)));
			T.push_back(GsVec2(findU(c), findV(c)));
			// a, d, c
			T.push_back(GsVec2(findU(a), findV(a)));
			T.push_back(GsVec2(findU(d), findV(d)));
			T.push_back(GsVec2(findU(c), findV(c)));
			// Normals

			GsVec n1, n2;
			// a, b, c
			n1 = GsVec(a.x - x, a.y - y, a.z - z); n1.normalize(); N.push() = n1;
			n1 = GsVec(b.x - x, b.y - y, b.z - z); n1.normalize(); N.push() = n1;
			n1 = GsVec(c.x - x, c.y - y, c.z - z); n1.normalize(); N.push() = n1;

			// a, d, c
			n2 = GsVec(a.x - x, a.y - y, a.z - z); n2.normalize(); N.push() = n2;
			n2 = GsVec(d.x - x, d.y - y, d.z - z); n2.normalize(); N.push() = n2;
			n2 = GsVec(c.x - x, c.y - y, c.z - z); n2.normalize(); N.push() = n2;
		}
	}
}

void SoTexturedSphere::build(float r, int _lfaces, int _layers)
{
	
	T.clear();
	this->r = r; this->_lfaces = _lfaces; this->_layers = _layers;

	this->calculateParameters();
	this->buildTop();
	this->buildBottom();

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

void SoTexturedSphere::draw(const GsMat& tr, const GsMat& pr, const GsLight& l)
{
	GsMat toSend;
	toSend = tr * myTrans * initialPos;
	float f[4];
	float sh = (float)_mtl.shininess;
	if (sh<0.001f) sh = 64;

	glUseProgram(_progphong.id);

	glBindVertexArray(va[0]);
	glBindTexture(GL_TEXTURE_2D, _texid1);

	// Updates uniforms
	glUniformMatrix4fv(_progphong.uniloc[0], 1, GL_FALSE, toSend.e);
	glUniformMatrix4fv(_progphong.uniloc[1], 1, GL_FALSE, pr.e);
	glUniform3fv(_progphong.uniloc[2], 1, l.pos.e);
	glUniform4fv(_progphong.uniloc[3], 1, l.amb.get(f));
	glUniform4fv(_progphong.uniloc[4], 1, l.dif.get(f));
	glUniform4fv(_progphong.uniloc[5], 1, l.spe.get(f));
	glUniform4fv(_progphong.uniloc[6], 1, _mtl.ambient.get(f));
	glUniform4fv(_progphong.uniloc[7], 1, _mtl.diffuse.get(f));
	glUniform4fv(_progphong.uniloc[8], 1, _mtl.specular.get(f));
	glUniform1fv(_progphong.uniloc[9], 1, &sh);
	glUniform1ui(_progphong.uniloc[10], _texid1);

	glBindVertexArray(va[0]);
	glDrawArrays(GL_TRIANGLES, 0, _numpoints);
	glBindVertexArray(0); // break the existing vertex array object binding.

}

void SoTexturedSphere::drawShadow(const GsMat& tr, const GsMat& pr, const GsLight& l)
{
	//cout << "Size of P is " << P.size() << endl;
	GsMat toSend;
	toSend = tr * shadow * myTrans * initialPos;
	float f[4];
	float sh = (float)_mtl.shininess;
	if (sh<0.001f) sh = 64;

	glUseProgram(_progphong.id);

	glBindVertexArray(va[0]);
	glBindTexture(GL_TEXTURE_2D, _texid2);

	// Updates uniforms
	glUniformMatrix4fv(_progphong.uniloc[0], 1, GL_FALSE, toSend.e);
	glUniformMatrix4fv(_progphong.uniloc[1], 1, GL_FALSE, pr.e);
	glUniform3fv(_progphong.uniloc[2], 1, l.pos.e);
	glUniform4fv(_progphong.uniloc[3], 1, l.amb.get(f));
	glUniform4fv(_progphong.uniloc[4], 1, l.dif.get(f));
	glUniform4fv(_progphong.uniloc[5], 1, l.spe.get(f));
	glUniform4fv(_progphong.uniloc[6], 1, _mtl.ambient.get(f));
	glUniform4fv(_progphong.uniloc[7], 1, _mtl.diffuse.get(f));
	glUniform4fv(_progphong.uniloc[8], 1, _mtl.specular.get(f));
	glUniform1fv(_progphong.uniloc[9], 1, &sh);
	glUniform1ui(_progphong.uniloc[10], _texid2);

	glBindVertexArray(va[0]);
	glDrawArrays(GL_TRIANGLES, 0, _numpoints);
	glBindVertexArray(0); // break the existing vertex array object binding.

}

