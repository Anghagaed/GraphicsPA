#include "so_watch.h"

SoWatch::SoWatch() {
}

void SoWatch::init(const GlProgram& prog)
{
	// Define buffers needed:
	set_program(prog);
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(2);       // will use 2 buffers: one for coordinates and one for colors
	uniform_locations(2); // will send 2 variables: the 2 matrices below
	uniform_location(0, "vTransf");
	uniform_location(1, "vProj");

	x = 0.0;
	y = 0.0;
	z = 0.25;
	r = 0.0;
	mlen = 0.0;
	slen = 0.0;
	mq = 0.0;
	sq = 0.0;
	_numpoints = 0;
}

void SoWatch::buildCircle() {
	float dQ = 2 * PI / 100;

	for (float q = 0.; q <= 2 * PI; q += dQ) {
		P.push_back(GsVec(x + r * cos(q), y + r * sin(q), z)); C.push_back(GsColor::white);
		P.push_back(GsVec(x + r * cos(q + dQ), y + r * sin(q + dQ), z)); C.push_back(GsColor::white);
	}


}

void SoWatch::buildTicky() {
	int i = 0;
	float q = PI / 2 ;
	float dQ = 2 * PI / 60;

	while (q < 5 * PI / 2) {
		if (i % 5 == 0) {
			P.push_back(GsVec(x + r * cos(q), y + r * sin(q), z)); C.push_back(GsColor::magenta);
			P.push_back(GsVec(x + r * cos(q) * 0.85f, y + r * sin(q) * 0.85f, z)); C.push_back(GsColor::magenta);
		}
		else {
			P.push_back(GsVec(x + r * cos(q), y + r * sin(q), z)); C.push_back(GsColor::white);
			P.push_back(GsVec(x + r * cos(q) * 0.925f, y + r * sin(q) * 0.925f, z)); C.push_back(GsColor::white);
		}
		q += dQ;
		++i;
	}
}

void SoWatch::buildHand() {
	q = PI / 2;
	P.push_back(GsVec(x + r * cos(q) * 0.55f, y + r * sin(q) * 0.55f, z)); C.push_back(GsColor::cyan);
	q = q + PI;
	P.push_back(GsVec(x + r * cos(q) * 0.05f, y + r * sin(q) * 0.05f, z)); C.push_back(GsColor::cyan);

	q = PI / 2;
	P.push_back(GsVec(x + r * cos(q) * 0.75f, y + r * sin(q) * 0.75f, z)); C.push_back(GsColor::magenta);
	q = q + PI;
	P.push_back(GsVec(x + r * cos(q) * 0.15f, y + r * sin(q) * 0.15f, z)); C.push_back(GsColor::magenta);
}

void SoWatch::build(float r, float mlen, float slen) {
	this->r = r;	 this->mlen = mlen;		this->slen = slen;

	P.clear(); C.clear();

	// Build the watch
	this->buildCircle();
	this->buildTicky();
	this->buildHand();

	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, P.size() * 3 * sizeof(float), &P[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ARRAY_BUFFER, C.size() * 4 * sizeof(gsbyte), &C[0], GL_STATIC_DRAW);

	// save size so that we can free our buffers and later just draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.resize(0); C.resize(0);
}

void SoWatch::draw(GsMat& tr, GsMat& pr)
{
	// Draw Lines:
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

	glDrawArrays(GL_LINES, 0, _numpoints);
	/*
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_LINES, 0, _numpoints);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	*/
}
