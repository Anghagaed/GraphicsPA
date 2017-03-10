# include "so_capsule.h"
# include <iostream>
SoCapsule::SoCapsule() {
	x = 0; y = 0; z = 0;
	_numfaces = 0;
	_numpoints = 0;
}

void SoCapsule::init() {
	// Build program:
	_vshgou.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_gouraud.glsl");
	_fshgou.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_gouraud.glsl");
	_proggouraud.init_and_link(_vshgou, _fshgou);

	_vshphong.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_phong.glsl");
	_fshphong.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_mcol_phong.glsl");
	_progphong.init_and_link(_vshphong, _fshphong);

	// Define buffers needed:
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(2);       // will use 2 buffers: one for coordinates and one for colors
	_prog.uniform_locations(2); // will send 2 variables: the 2 matrices below
	_prog.uniform_location(0, "vTransf");
	_prog.uniform_location(1, "vProj");
}

void SoCapsule::build(float len, float rt, float rb, int nfaces) {
	this->len = len;	 this->rt = rt;
	this->rb = rb;		this->_numfaces = nfaces;

	P.size(0); C.size(0);

	this->calculateParameters();
	this->buildTop();
	this->buildMiddle();
	this->buildBottom();

	// send data to OpenGL buffers:
	glBindVertexArray(va[0]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float)*P.size(), P.pt(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(gsbyte)*C.size(), C.pt(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);

	glBindVertexArray(0); // break the existing vertex array object binding.

	// save size so that we can free our buffers and later draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.capacity(0); C.capacity(0); dy.clear(); dx.clear(); dz.clear();
}

void SoCapsule::draw(GsMat& tr, GsMat& pr) {
	// Prepare program:
	glUseProgram(_prog.id);
	glUniformMatrix4fv(_prog.uniloc[0], 1, GL_FALSE, tr.e);
	glUniformMatrix4fv(_prog.uniloc[1], 1, GL_FALSE, pr.e);

	// Draw:
	glBindVertexArray(va[0]);
	glDrawArrays(GL_TRIANGLES, 0, _numpoints);
	glBindVertexArray(0); // break the existing vertex array object binding.
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

void SoCapsule::buildTop() {
	int j = 0;
	//for (int j = 0; j < dy.size() - 1; ++j) {
		for (int i = 0; i < dx.size() - 1; ++i) {
			P.push().set(x + rt * cos(dy[j]) * dx[i], y + len / 2 + rt * sin(dy[j]), z + rt * cos(dy[j]) * dz[i]); C.push() = (GsColor::white);
			P.push().set(x + rt * cos(dy[j]) * dx[i + 1], y + len / 2 + rt * sin(dy[j]), z + rt * cos(dy[j]) * dz[i + 1]); C.push() = (GsColor::white);
			P.push().set(x + rt * cos(dy[j + 1]) * dx[i + 1], y + len / 2 + rt * sin(dy[j + 1]), z + rt * cos(dy[j + 1]) * dz[i + 1]); C.push() = (GsColor::white);

			P.push().set(x + rt * cos(dy[j]) * dx[i], y + len / 2 + rt * sin(dy[j]), z + rt * cos(dy[j]) * dz[i]); C.push() = (GsColor::white);
			P.push().set(x + rt * cos(dy[j + 1]) * dx[i], y + len / 2 + rt * sin(dy[j + 1]), z + rt * cos(dy[j + 1]) * dz[i]); C.push() = (GsColor::white);
			P.push().set(x + rt * cos(dy[j + 1]) * dx[i + 1], y + len / 2 + rt * sin(dy[j + 1]), z + rt * cos(dy[j + 1]) * dz[i + 1]); C.push() = (GsColor::white);
		}
	//}
}

void SoCapsule::buildMiddle() {
	for (int i = 0; i < dx.size() - 1; ++i) {
		P.push().set(x + rt * dx[i], y + len / 2, z + rt * dz[i]); C.push() = (GsColor::white);
		P.push().set(x + rt * dx[i + 1], y + len / 2, z + rt * dz[i + 1]); C.push() = (GsColor::white);
		P.push().set(x + rb * dx[i + 1], -(y + len / 2), z + rb * dz[i + 1]); C.push() = (GsColor::white);

		P.push().set(x + rt * dx[i], y + len / 2, z + rt * dz[i]); C.push() = (GsColor::white);
		P.push().set(x + rb * dx[i], -(y + len / 2), z + rb * dz[i]); C.push() = (GsColor::white);
		P.push().set(x + rb * dx[i + 1], -(y + len / 2), z + rb * dz[i + 1]); C.push() = (GsColor::white);
	}
}

void SoCapsule::buildBottom() {
	int j = 0;
	//for (int j = 0; j < dy.size() - 1; ++j) {
		for (int i = 0; i < dx.size() - 1; ++i) {
			P.push().set(x + rb * cos(dy[j]) * dx[i], -(y + len / 2 + rb * sin(dy[j])), z + rb * cos(dy[j]) * dz[i]); C.push() = (GsColor::white);
			P.push().set(x + rb * cos(dy[j]) * dx[i + 1], -(y + len / 2 + rb * sin(dy[j])), z + rb * cos(dy[j]) * dz[i + 1]); C.push() = (GsColor::white);
			P.push().set(x + rb * cos(dy[j + 1]) * dx[i + 1], -(y + len / 2 + rb * sin(dy[j + 1])), z + rb * cos(dy[j + 1]) * dz[i + 1]); C.push() = (GsColor::white);

			P.push().set(x + rb * cos(dy[j]) * dx[i], -(y + len / 2 + rb * sin(dy[j])), z + rb * cos(dy[j]) * dz[i]); C.push() = (GsColor::white);
			P.push().set(x + rb * cos(dy[j + 1]) * dx[i], -(y + len / 2 + rb * sin(dy[j + 1])), z + rb * cos(dy[j + 1]) * dz[i]); C.push() = (GsColor::white);
			P.push().set(x + rb * cos(dy[j + 1]) * dx[i + 1], -(y + len / 2 + rb * sin(dy[j + 1])), z + rb * cos(dy[j + 1]) * dz[i + 1]); C.push() = (GsColor::white);
		}
	//}
	
}