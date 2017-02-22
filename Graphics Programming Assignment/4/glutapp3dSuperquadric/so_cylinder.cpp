
# include "so_cylinder.h"

SoCylinder::SoCylinder()
{
	_numpoints = 0;
	_flatn = true;
	r = 0; 
	x = 0; 
	y = 0; 
	z = 0;
	l = 0;
	_nfaces = 0;
}

void SoCylinder::init()
{
	_vshphong.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_phong.glsl");
	_fshphong.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_mcol_phong.glsl");
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

void SoCylinder::calculateParameters() {
	int layers = _nfaces / 4;
	double dQ = 2 * PI / _nfaces;
	for (double i = 0; i <= (2 * PI + dQ); i += dQ) {
		dx.push_back(cos(i));
		dz.push_back(sin(i));
	}
}

void SoCylinder::buildBody() {
	for (int i = 0; i < dx.size() - 1; ++i) {
		GsPnt a, b, c, d;
		a = GsPnt(x + r * dx[i], y + l / 2, z + r * dz[i]);
		b = GsPnt(x + r * dx[i + 1], y + l / 2, z + r * dz[i + 1]);
		c = GsPnt(x + r * dx[i + 1], -(y + l / 2), z + r * dz[i + 1]);
		d = GsPnt(x + r * dx[i], -(y + l / 2), z + r * dz[i]);
		P.push() = a;
		P.push() = b;
		P.push() = c;

		P.push() = a;
		P.push() = d;
		P.push() = c;

		N.push() = a;
		N.push() = b;
		N.push() = c;

		normalize(a, b, c);
		normalize(a, d, c);
	}
}

void SoCylinder::buildTop() {
	for (int i = 0; i < dx.size() - 1; ++i) {
		GsPnt a = GsPnt(x, y + l / 2, z);
		GsPnt b = GsPnt(x + r * dx[i], y + l / 2, z + r * dz[i]);
		GsPnt c = GsPnt(x + r * dx[i + 1], y + l / 2, z + r * dz[i + 1]);
		
		P.push() = a;
		P.push() = b;
		P.push() = c;

		normalize(a, b, c);
	}
}

void SoCylinder::buildBottom() {
	for (int i = 0; i < dx.size() - 1; ++i) {
		GsPnt a, b, c;
		a = GsPnt(x, -(y + l / 2), z);
		b = GsPnt(x + r * dx[i], -(y + l / 2), z + r * dz[i]);
		c = GsPnt(x + r * dx[i + 1], -(y + l / 2), z + r * dz[i + 1]);
		
		P.push() = a;
		P.push() = b;
		P.push() = c;

		normalize(a, b, c);
	}
}

void SoCylinder::normalize(GsPnt a, GsPnt b, GsPnt c) {
	GsVec n;
	const float ns = 0.05f;
	if (_flatn) {
		n = normal(a, b, c);
		N.push() = n; N.push() = n; N.push() = n;
		n *= ns;
		GsPnt fc = (a + c + b) / 3.0f; NL.push() = fc; NL.push() = fc + n;
	}
	else {

	}

}

void SoCylinder::build(float r, float l, int _nfaces)
{
	this->r = r; this->l = l; this->_nfaces = _nfaces;

	this->calculateParameters();
	this->buildBody();
	this->buildTop();
	this->buildBottom();

	C.size(P.size()); C.setall(GsColor::blue);

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
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(gsbyte)*C.size(), C.pt(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);

	glBindVertexArray(0); // break the existing vertex array object binding.

						  // save size so that we can free our buffers and later draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.capacity(0); C.capacity(0); N.capacity(0);
	dx.clear(); dz.clear();
	_mtl.specular.set(255, 255, 255);
	_mtl.shininess = 8; // increase specular effect
}

void SoCylinder::draw(const GsMat& tr, const GsMat& pr, const GsLight& l)
{
	float f[4];
	float sh = (float)_mtl.shininess;
	if (sh<0.001f) sh = 64;

	glUseProgram(_progphong.id);
	glUniformMatrix4fv(_progphong.uniloc[0], 1, GL_FALSE, tr.e);
	glUniformMatrix4fv(_progphong.uniloc[1], 1, GL_FALSE, pr.e);
	glUniform3fv(_progphong.uniloc[2], 1, l.pos.e);
	glUniform4fv(_progphong.uniloc[3], 1, l.amb.get(f));
	glUniform4fv(_progphong.uniloc[4], 1, l.dif.get(f));
	glUniform4fv(_progphong.uniloc[5], 1, l.spe.get(f));
	glUniform4fv(_progphong.uniloc[6], 1, _mtl.ambient.get(f));
	glUniform4fv(_progphong.uniloc[7], 1, _mtl.specular.get(f));
	glUniform1fv(_progphong.uniloc[8], 1, &sh);

	glBindVertexArray(va[0]);
	glDrawArrays(GL_TRIANGLES, 0, _numpoints);
	glBindVertexArray(0); // break the existing vertex array object binding.
	
}

