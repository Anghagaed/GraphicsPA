# include "draw3D.h"

draw3D::draw3D() {
	numPoints = 0;
	color = GsColor::blue;
}

void draw3D::init() {
	vsh.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_gouraud.glsl");
	fsh.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_gouraud.glsl");
	prog.init_and_link(vsh, fsh);
	set_program(prog);
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(3);       // will use 2 buffers: one for coordinates and one for colors

	uniform_locations(9); // will send 2 variables: the 2 matrices below
	uniform_location(0, "vTransf");
	uniform_location(1, "vProj");
	uniform_location(2, "lPos");
	uniform_location(3, "la");
	uniform_location(4, "ld");
	uniform_location(5, "ls");
	uniform_location(6, "ka");
	uniform_location(7, "ks");
	uniform_location(8, "sh");

}

void draw3D::build(const GsArray<GsVec>& ctrl) {
	GsVec offset = GsVec(0.0f, 0.0f, -0.3f);
	for (int i = 0; i < ctrl.size() - 1; ++i) {
		GsVec a, b, c, d;
		a = ctrl[i];
		b = a + offset;
		c = ctrl[i + 1];
		d = c + offset;

		// a, b, d
		P.push() = a; P.push() = b; P.push() = d;
		// d, c, a
		P.push() = d; P.push() = c; P.push() = a;

		// Calculating normals
		GsVec na, nb, nc, nd;
		na.cross(b - a, c - a);
		nb.cross(a - b, d - b);
		nc.cross(a - c, d - c);
		nd.cross(b - d, c - d);

		na.normalize();
		nb.normalize();
		nc.normalize();
		nd.normalize();

		// a, b, d
		N.push() = na; N.push() = nb; N.push() = nd;
		// d, c, a
		N.push() = nd; N.push() = nc; N.push() = na;
	}
	C.size(P.size());
	C.setall(color);

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

	glBindVertexArray(0); 
	numPoints = P.size();

	// free non-needed memory:
	P.capacity(0); C.capacity(0); N.capacity(0);
}

void draw3D::draw(const GsMat& tr, const GsMat& pr) {
	float f[5][4];
	float sh = 1.0f;
	GsVec lightPos = GsVec(0, 0, 5);

	f[0][0] = 90.0f / 255.0f, f[0][1] = 90.0f / 255.0f, f[0][2] = 90.0f / 255.0f, f[0][3] = 255.0f / 255.0f;
	f[1][0] = 255.0f / 255.0f, f[1][1] = 255.0f / 255.0f, f[1][2] = 255.0f / 255.0f, f[1][3] = 255.0f / 255.0f;
	f[2][0] = 255.0f / 255.0f, f[2][1] = 255.0f / 255.0f, f[2][2] = 255.0f / 255.0f, f[2][3] = 255.0f / 255.0f;
	f[3][0] = 0.2f, f[3][1] = 0.2f, f[3][2] = 0.2f, f[3][3] = 1.0f;
	f[4][0] = 1.0f, f[4][1] = 1.0f, f[4][2] = 1.0f, f[4][3] = 1.0f;

	glUseProgram(prog.id);
	glUniformMatrix4fv(uniloc[0], 1, GL_FALSE, tr.e);
	glUniformMatrix4fv(uniloc[1], 1, GL_FALSE, pr.e);
	glUniform3fv(uniloc[2], 1, lightPos.e);	
	glUniform4fv(uniloc[3], 1, f[0]);
	glUniform4fv(uniloc[4], 1, f[1]);
	glUniform4fv(uniloc[5], 1, f[2]);
	glUniform4fv(uniloc[6], 1, f[3]);
	glUniform4fv(uniloc[7], 1, f[4]);
	glUniform1fv(uniloc[8], 1, &sh);

	glBindVertexArray(va[0]);
	glDrawArrays(GL_TRIANGLES, 0, numPoints);
	glBindVertexArray(0); // break the existing vertex array object binding.
}