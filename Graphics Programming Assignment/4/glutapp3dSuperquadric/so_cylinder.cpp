
# include "so_cylinder.h"

SoCylinder::SoCylinder()
{
	_numpoints = 0;
	_phong = false;
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
	// Load programs:
	_vshgou.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_gouraud.glsl");
	_fshgou.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_gouraud.glsl");
	_proggouraud.init_and_link(_vshgou, _fshgou);

	_vshphong.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_phong.glsl");
	_fshphong.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_mcol_phong.glsl");
	_progphong.init_and_link(_vshphong, _fshphong);

	// Define buffers needed:
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(3);       // will use 3 buffers

	_proggouraud.uniform_locations(9); // will send 9 variables
	_proggouraud.uniform_location(0, "vTransf");
	_proggouraud.uniform_location(1, "vProj");
	_proggouraud.uniform_location(2, "lPos");
	_proggouraud.uniform_location(3, "la");
	_proggouraud.uniform_location(4, "ld");
	_proggouraud.uniform_location(5, "ls");
	_proggouraud.uniform_location(6, "ka");
	_proggouraud.uniform_location(7, "ks");
	_proggouraud.uniform_location(8, "sh");

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
		P.push().set(x + r * dx[i], y + l / 2, z + r * dz[i]);
		P.push().set(x + r * dx[i + 1], y + l / 2, z + r * dz[i + 1]);
		P.push().set(x + r * dx[i + 1], -(y + l / 2), z + r * dz[i + 1]); 

		P.push().set(x + r * dx[i], y + l / 2, z + r * dz[i]);
		P.push().set(x + r * dx[i], -(y + l / 2), z + r * dz[i]);
		P.push().set(x + r * dx[i + 1], -(y + l / 2), z + r * dz[i + 1]); 

		N.push().set(x + r * dx[i], y + l / 2, z + r * dz[i]);
		N.push().set(x + r * dx[i + 1], y + l / 2, z + r * dz[i + 1]);
		N.push().set(x + r * dx[i + 1], -(y + l / 2), z + r * dz[i + 1]);

		N.push().set(x + r * dx[i], y + l / 2, z + r * dz[i]);
		N.push().set(x + r * dx[i], -(y + l / 2), z + r * dz[i]);
		N.push().set(x + r * dx[i + 1], -(y + l / 2), z + r * dz[i + 1]);
	}
}

void SoCylinder::buildTop() {
	for (int i = 0; i < dx.size() - 1; ++i) {
		P.push().set(x, y + l / 2, z);
		P.push().set(x + r * dx[i], y + l / 2, z + r * dz[i]);
		P.push().set(x + r * dx[i + 1], y + l / 2, z + r * dz[i + 1]);

		N.push().set(x, y + l / 2, z);
		N.push().set(x + r * dx[i], y + l / 2, z + r * dz[i]);
		N.push().set(x + r * dx[i + 1], y + l / 2, z + r * dz[i + 1]);
	}
}

void SoCylinder::buildBottom() {
	for (int i = 0; i < dx.size() - 1; ++i) {
		P.push().set(x, -(y + l / 2), z);
		P.push().set(x + r * dx[i], -(y + l / 2), z + r * dz[i]);
		P.push().set(x + r * dx[i + 1], -(y + l / 2), z + r * dz[i + 1]);

		N.push().set(x, -(y + l / 2), z);
		N.push().set(x + r * dx[i], -(y + l / 2), z + r * dz[i]);
		N.push().set(x + r * dx[i + 1], -(y + l / 2), z + r * dz[i + 1]);
	}
}

void SoCylinder::build(float r, float l, int _nfaces)
{
	this->r = r; this->l = l; this->_nfaces = _nfaces;

	this->calculateParameters();
	this->buildBody();
	this->buildTop();
	this->buildBottom();
	//N.capacity(0);
	//N.push().set(0.75f, 0.75f, 0.75f);
	//N.push().set(0.75f, 0.76f, 0.75f);
	//N.push().set(0.76f, 0.75f, 0.76f);

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

	_mtl.specular.set(255, 255, 255);
	_mtl.shininess = 8; // increase specular effect
}

void SoCylinder::draw(const GsMat& tr, const GsMat& pr, const GsLight& l)
{
	float f[4];
	float sh = (float)_mtl.shininess;
	if (sh<0.001f) sh = 64;

	if (_phong)
	{
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
	}
	else
	{
		glUseProgram(_proggouraud.id);
		glUniformMatrix4fv(_proggouraud.uniloc[0], 1, GL_FALSE, tr.e);
		glUniformMatrix4fv(_proggouraud.uniloc[1], 1, GL_FALSE, pr.e);
		glUniform3fv(_proggouraud.uniloc[2], 1, l.pos.e);
		glUniform4fv(_proggouraud.uniloc[3], 1, l.amb.get(f));
		glUniform4fv(_proggouraud.uniloc[4], 1, l.dif.get(f));
		glUniform4fv(_proggouraud.uniloc[5], 1, l.spe.get(f));
		glUniform4fv(_proggouraud.uniloc[6], 1, _mtl.ambient.get(f));
		glUniform4fv(_proggouraud.uniloc[7], 1, _mtl.specular.get(f));
		glUniform1fv(_proggouraud.uniloc[8], 1, &sh);
	}

	glBindVertexArray(va[0]);
	glDrawArrays(GL_TRIANGLES, 0, _numpoints);
	glBindVertexArray(0); // break the existing vertex array object binding.
	
}
/*
vec4 shade(vec4 p) {

}
*/
