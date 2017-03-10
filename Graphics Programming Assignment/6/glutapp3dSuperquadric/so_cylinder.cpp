
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
	_vshphong.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_gouraud.glsl");
	_fshphong.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_gouraud.glsl");
	_progphong.init_and_link(_vshphong, _fshphong);

	// Define buffers needed:
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(3);       // will use 3 buffers

	_progphong.uniform_locations(10); // will send 9 variables
	_progphong.uniform_location(0, "vTransf");
	_progphong.uniform_location(1, "vProj");
	_progphong.uniform_location(2, "lPos");
	_progphong.uniform_location(3, "la");
	_progphong.uniform_location(4, "ld");
	_progphong.uniform_location(5, "ls");
	_progphong.uniform_location(6, "ka");
	_progphong.uniform_location(7, "ks");
	_progphong.uniform_location(8, "sh");
	_progphong.uniform_location(9, "Tex1");
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

		GsVec n1, n2;
		const float ns = 0.05f;
		if (_flatn) {
			n1 = normal(c, a, b); n2 = normal(a, c, d);
			N.push() = n1; N.push() = n1; N.push() = n1;
			N.push() = n2; N.push() = n2; N.push() = n2;
			n1 *= ns; n2 *= ns;
			GsPnt f1c = (a + c + b) / 3.0f; NL.push() = f1c; NL.push() = f1c + n1;
			GsPnt f2c = (a + d + c) / 3.0f; NL.push() = f2c; NL.push() = f2c + n2;
		}
		else {
			n1 = GsVec(a.x, 0, a.z);
			n1.normalize();
			N.push() = n1; NL.push() = a;
			NL.push(a + n1*ns);
			n1 = GsVec(b.x, 0, b.z); 
			n1.normalize();
			N.push() = n1; NL.push() = b;
			NL.push(b + n1*ns);
			n1 = GsVec(c.x, 0, c.z);
			n1.normalize();
			N.push() = n1; NL.push() = c;
			NL.push(c + n1*ns);
			
			n2 = GsVec(a.x, 0, a.z);
			n2.normalize();
			N.push() = n2;
			n2 = GsVec(d.x, 0, d.z);
			n2.normalize();
			N.push() = n2; NL.push() = d;
			NL.push(d + n2*ns);
			n2 = GsVec(c.x, 0, c.z);
			N.push() = n2;
		}
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

		GsVec n;
		const float ns = 0.05f;
		if (_flatn) {
			n = normal(a, c, b);
			N.push() = n; N.push() = n; N.push() = n;
			n *= ns;
			GsPnt fc = (a + c + b) / 3.0f; NL.push() = fc; NL.push() = fc + n;
		}
		else {
			n = GsVec(0, 1.0f, 0);
			a = b;
			N.push() = n; NL.push() = a;
			NL.push(a + n*ns);
			N.push() = n;
			N.push() = n;
		}
	}
	if (!_flatn) {
		GsPnt n = GsPnt(0, l / 2, 0);
		GsPnt N = GsPnt(0, 0.15f, 0);
		NL.push() = n;
		NL.push() = N + n;
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

		GsVec n;
		const float ns = 0.05f;
		if (_flatn) {
			n = normal(c, a, b);
			N.push() = n; N.push() = n; N.push() = n;
			n *= ns;
			GsPnt fc = (a + c + b) / 3.0f; NL.push() = fc; NL.push() = fc + n;
		}
		else {
			n = GsVec(0, -1.0f, 0);
			N.push() = n; NL.push() = b;
			NL.push(b + n*ns);
			N.push() = n;
			N.push() = n;
		}
	}
	if (!_flatn) {
		GsPnt n = GsPnt(0, -l / 2, 0);
		GsPnt N = GsPnt(0, -0.15f, 0);
		NL.push() = n;
		NL.push() = N + n;
	}
}

void SoCylinder::build(float r, float l, int _nfaces)
{
	NL.capacity(0);
	this->r = r; this->l = l; this->_nfaces = _nfaces;

	this->calculateParameters();
	this->buildBody();
	//this->buildTop();
	//this->buildBottom();

	C.size(P.size());
	if (_flatn)
		C.setall(GsColor::red);
	else
		C.setall(GsColor::green);
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

