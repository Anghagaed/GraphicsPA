# include "so_capsule.h"

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
	this->len = len;
	this->rt = rt;
	this->rb = rb;
	this->_numfaces = nfaces;

	P.clear(); C.clear(); 
	
	this->drawTop();
	//this->drawMiddle();
	//this->drawBottom();
	//double Q = 0; double dQ = 2 * M_PI / _numfaces;
	//P.push_back(GsVec(x, y, z + (len / 2)));	C.push_back(GsColor::white);
	//P.push_back(GsVec(x + rt * cos(Q), y + rt * sin(Q), z + (len / 2)));	C.push_back(GsColor::white);
	//P.push_back(GsVec(x + rt * cos(Q + dQ), y + rt * sin(Q + dQ), z + (len / 2)));	C.push_back(GsColor::white);

	//P.push_back(GsVec(0.0, 0.5, 0.0)); C.push_back(GsColor::white);
	//P.push_back(GsVec(0.0, 0.0, 0.0)); C.push_back(GsColor::white);
	//P.push_back(GsVec(0.5, 0.5, 0.0)); C.push_back(GsColor::white);

	// send data to OpenGL buffers:
	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, P.size() * 3 * sizeof(float), &P[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ARRAY_BUFFER, C.size() * 4 * sizeof(gsbyte), &C[0], GL_STATIC_DRAW);

	// save size so that we can free our buffers and later just draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.resize(0); C.resize(0);
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

void SoCapsule::drawTop() {
	int layers = ceil(4 * rt / _numfaces);
	double dQ = 2 * M_PI / _numfaces;

	for (double Q = 0; Q < 2 * M_PI; Q += dQ) {
		P.push_back(GsVec(x, y, z + (len / 2)));
		P.push_back(GsVec(x + rt * cos(Q), y + rt * sin(Q), z + (len / 2)));
		P.push_back(GsVec(x + rt * cos(Q + dQ), y + rt * sin(Q + dQ), z + (len / 2)));
		C.push_back(GsColor::white);
		C.push_back(GsColor::white);
		C.push_back(GsColor::white);
	}

}

void SoCapsule::drawMiddle() {

}

void SoCapsule::drawBottom() {

}