# include "so_capsule.h"
# include <iostream>
SoCapsule::SoCapsule() {
}

void SoCapsule::init(const GlProgram& prog) {
	set_program(prog);
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(2);       // will use 2 buffers: one for coordinates and one for colors
	uniform_locations(2); // will send 2 variables: the 2 matrices below
	uniform_location(0, "vTransf");
	uniform_location(1, "vProj");

	x = 0.0;
	y = 0.0;
	z = 0.0;
	len = 0.0;
	rt = 0.0;
	rb = 0.0;
	_numfaces = 0;
	_numpoints = 0;
	P.clear(); C.clear();
}

void SoCapsule::build(float len, float rt, float rb, int nfaces) {
	this->len = len;	 this->rt = rt;
	this->rb = rb;		this->_numfaces = nfaces;

	P.clear(); C.clear(); dy.clear(); dx.clear(); dz.clear();
	
	this->calculateParameters();
	this->drawTop();
	this->drawMiddle();
	this->drawBottom();

	// send data to OpenGL buffers:
	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, P.size() * 3 * sizeof(float), &P[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ARRAY_BUFFER, C.size() * 4 * sizeof(gsbyte), &C[0], GL_STATIC_DRAW);
	
	// save size so that we can free our buffers and later just draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.resize(0); C.resize(0); dx.resize(0); dy.resize(0); dz.resize(0);
}

void SoCapsule::draw(GsMat& tr, GsMat& pr) {
	glUseProgram(prog);
	glBindVertexArray(va[0]);

	glBindBuffer(GL_ARRAY_BUFFER, buf[0]); // positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buf[1]); // colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);

	glUniformMatrix4fv(uniloc[0], 1, GL_FALSE, tr.e);
	glUniformMatrix4fv(uniloc[1], 1, GL_FALSE, pr.e);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, _numpoints);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void SoCapsule::calculateParameters() {
	int layers = _numfaces / 4;
	double dQ = 2 * M_PI / _numfaces;
	for (double i = 0; i <= (2 * M_PI + dQ); i += dQ) {
		dx.push_back(cos(i));
		dz.push_back(sin(i));
	}
	dQ = (M_PI / 2) / layers;
	for (double i = 0; i <= M_PI / 2; i += dQ) {
		dy.push_back(i);
	}
}

void SoCapsule::drawTop() {
	int j = 0;
	//for (int j = 0; j < dy.size() - 1; ++j) {
		for (int i = 0; i < dx.size() - 1; ++i) {
			P.push_back(GsVec(x + rt * cos(dy[j]) * dx[i], y + len / 2 + rt * sin(dy[j]), z + rt * cos(dy[j]) * dz[i])); C.push_back(GsColor::white);
			P.push_back(GsVec(x + rt * cos(dy[j]) * dx[i + 1], y + len / 2 + rt * sin(dy[j]), z + rt * cos(dy[j]) * dz[i + 1])); C.push_back(GsColor::white);
			P.push_back(GsVec(x + rt * cos(dy[j+1]) * dx[i + 1], y + len / 2 + rt * sin(dy[j + 1]), z + rt * cos(dy[j + 1]) * dz[i + 1])); C.push_back(GsColor::white);

			P.push_back(GsVec(x + rt * cos(dy[j]) * dx[i] , y + len / 2 + rt * sin(dy[j]), z + rt * cos(dy[j]) * dz[i])); C.push_back(GsColor::white);
			P.push_back(GsVec(x + rt * cos(dy[j+1]) * dx[i] , y + len / 2 + rt * sin(dy[j+1]), z + rt * cos(dy[j+1]) * dz[i])); C.push_back(GsColor::white);
			P.push_back(GsVec(x + rt * cos(dy[j+1]) * dx[i + 1] , y + len / 2 + rt * sin(dy[j+1]), z + rt * cos(dy[j+1]) * dz[i + 1])); C.push_back(GsColor::white);
		}
	//}
}

void SoCapsule::drawMiddle() {
	for (int i = 0; i < dx.size() - 1; ++i) {
		P.push_back(GsVec(x + rt * dx[i], y + len / 2, z + rt * dz[i])); C.push_back(GsColor::white);
		P.push_back(GsVec(x + rt * dx[i+1], y + len / 2, z + rt * dz[i+1])); C.push_back(GsColor::white);
		P.push_back(GsVec(x + rb * dx[i + 1], -( y + len / 2 ), z + rb * dz[i + 1])); C.push_back(GsColor::white);

		P.push_back(GsVec(x + rt * dx[i], y + len / 2, z + rt * dz[i])); C.push_back(GsColor::white);
		P.push_back(GsVec(x + rb * dx[i], -(y + len / 2), z + rb * dz[i])); C.push_back(GsColor::white);
		P.push_back(GsVec(x + rb * dx[i + 1], -(y + len / 2), z + rb * dz[i + 1])); C.push_back(GsColor::white);
	}
}

void SoCapsule::drawBottom() {
	int j = 0;
	//for (int j = 0; j < dy.size() - 1; ++j) {
		for (int i = 0; i < dx.size() - 1; ++i) {
			P.push_back(GsVec(x + rb * cos(dy[j]) * dx[i], -( y + len / 2 + rb * sin(dy[j])), z + rb * cos(dy[j]) * dz[i])); C.push_back(GsColor::white);
			P.push_back(GsVec(x + rb * cos(dy[j]) * dx[i + 1], -(y + len / 2 + rb * sin(dy[j])), z + rb * cos(dy[j]) * dz[i + 1])); C.push_back(GsColor::white);
			P.push_back(GsVec(x + rb * cos(dy[j + 1]) * dx[i + 1], -(y + len / 2 + rb * sin(dy[j + 1])), z + rb * cos(dy[j + 1]) * dz[i + 1])); C.push_back(GsColor::white);

			P.push_back(GsVec(x + rb * cos(dy[j]) * dx[i], -(y + len / 2 + rb * sin(dy[j])), z + rb * cos(dy[j]) * dz[i])); C.push_back(GsColor::white);
			P.push_back(GsVec(x + rb * cos(dy[j + 1]) * dx[i], -(y + len / 2 + rb * sin(dy[j + 1])), z + rb * cos(dy[j + 1]) * dz[i])); C.push_back(GsColor::white);
			P.push_back(GsVec(x + rb * cos(dy[j + 1]) * dx[i + 1], -(y + len / 2 + rb * sin(dy[j + 1])), z + rb * cos(dy[j + 1]) * dz[i + 1])); C.push_back(GsColor::white);
		}
	//}
}