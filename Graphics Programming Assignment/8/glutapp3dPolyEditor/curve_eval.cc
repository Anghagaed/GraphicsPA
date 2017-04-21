#include "curve_eval.h"

SoCurve::SoCurve() {
	numPoints = 0;
	numSegs = 0;
}
void SoCurve::init() {
	vsh.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_flat.glsl");
	fsh.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_flat.glsl");
	prog.init_and_link(vsh, fsh);
	set_program(prog);
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(2);       // will use 2 buffers: one for coordinates and one for colors
	uniform_locations(2); // will send 2 variables: the 2 matrices below
	uniform_location(0, "vTransf");
	uniform_location(1, "vProj");

}
void SoCurve::build(int numSegs, const GsArray<GsVec>& ctrlpnts, curveType type) {
	//P.capacity(0); C.capacity(0);
	this->numSegs = numSegs; this->type = type;
	P.size(0); C.size(0);
	float dI = 1.0f / numSegs;
	if (type == NOCURVE) {
		return;
	}
	else if (type == LAGRANGE) {
		float n = float(ctrlpnts.size() - 1);
		for (int i = 0; i <= numSegs * (ctrlpnts.size() - 1); ++i) {
			P.push() = this->eval_lagrange(i * dI, ctrlpnts);
		}
		C.size(P.size());
		C.setall(GsColor::red);
	}
	else if (type == BEZIER) {
		for (int i = 0; i <= numSegs; ++i) {
			P.push() = eval_bezier(i * dI, ctrlpnts);
		}
		C.size(P.size());
		C.setall(GsColor::green);
	}
	else if (type == BSPLINE) {
		int k = 3;
		float dT = ctrlpnts.size() / (float(numSegs));
		for (float i = 2; i <= ctrlpnts.size(); i+=dT) {
			P.push() = eval_bspline(i, k, ctrlpnts);
		}
		C.size(P.size());
		C.setall(GsColor::yellow);
	} 
	else if (type == CRSPLINE) {
		GsArray<GsVec> newCtrl, temp;
		for (int i = 1; i < ctrlpnts.size() - 1; ++i) {
			temp = crspline(i, ctrlpnts);
			// copy pasta into newCtrl
			for (int j = 0; j < temp.size(); ++j) {
				newCtrl.push() = temp[j];
			}
			temp.size(0);
		}

		// Use BSPLINE on newCtrl
		int k = 4;
		float dT = newCtrl.size() / (float)numSegs;

		for (float t = 3.0f; t < newCtrl.size(); t += dT) {
			P.push() = eval_bspline(t, k, newCtrl);
		}
		newCtrl.size(0);

		C.size(P.size());
		C.setall(GsColor::magenta);
	}
	else if (BOSPLINE) {
		GsArray<GsVec> newCtrl, temp;
		for (int i = 1; i < ctrlpnts.size() - 1; ++i) {
			temp = bospline(i, ctrlpnts);
			// copy pasta into newCtrl
			for (int j = 0; j < temp.size(); ++j) {
				newCtrl.push() = temp[j];
			}
			temp.size(0);
		}

		// Use BSPLINE on newCtrl
		int k = 4;
		float dT = newCtrl.size() / (float)numSegs;

		for (float t = 3.0f; t < newCtrl.size(); t += dT) {
			P.push() = eval_bspline(t, k, newCtrl);
		}
		newCtrl.size(0);

		C.size(P.size());
		C.setall(GsColor::cyan);
	}
	// copying
	copy.capacity(0);
	copy = P;

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
	numPoints = P.size();

	// free non-needed memory:
	P.capacity(0); C.capacity(0);
}
void SoCurve::draw(const GsMat& tr, const GsMat& pr) {
	if (type != NOCURVE) {
		glUseProgram(prog.id);
		glBindVertexArray(va[0]);
		glUniformMatrix4fv(uniloc[0], 1, GL_FALSE, tr.e);
		glUniformMatrix4fv(uniloc[1], 1, GL_FALSE, pr.e);

		glDrawArrays(GL_POINTS, 0, numPoints);
		glDrawArrays(GL_LINE_STRIP, 0, numPoints);
		glBindVertexArray(0);
	}
}
// Calculate n factorial
long int factorial(int n) {
	return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

// Calculate Binomial Coefficient (int n, int i) : ways of picking i unordered outcomes from n
int binoCoefficient(int n, int i) {
	return (factorial(n)) / factorial(i) / factorial(n - i);
}

GsVec SoCurve::eval_bezier(float t, const GsArray<GsVec>& ctrlpnts) {
	GsVec result;
	float intermediate;
	result = GsVec(0, 0, 0);
	int n = ctrlpnts.size() - 1;
	for (int i = 0; i < ctrlpnts.size(); ++i) {
		intermediate = binoCoefficient(n, i) * pow(t, i) * pow(1.0f - t, n - i);
		result += ctrlpnts[i] * intermediate;
	}
	return result;
}
GsVec SoCurve::eval_lagrange(float t, const GsArray<GsVec>& ctrlpnts) {
	GsVec result;
	float intermediate;
	result = GsVec(0, 0, 0);
	for (int i = 0; i < ctrlpnts.size(); ++i) {
		intermediate = 1;
		// Calculate intermediate product
		for (int j = 0; j < ctrlpnts.size(); ++j) {
			if (j == i)
				continue;
			intermediate *= (t - j) / (i - j);
		}
		result += ctrlpnts[i] * intermediate;
	}
	return result;
}

float SoCurve::NRecurs(int i, int k, float t) {
	float iFloat = float(i);
	// base case
	if (k == 1)
		return (iFloat <= t && t < (iFloat + 1)) ? 1.0f : 0;
	// recursive case
	else 
		return ( ((t - iFloat) / (k - 1)) * NRecurs(i, k - 1, t) + ((iFloat + k - t) / (k - 1)) * NRecurs(i + 1, k - 1, t) );
}

GsVec SoCurve::eval_bspline(float t, int k, const GsArray<GsVec>& pnts) {
	GsVec result;
	result = GsVec(0.0f, 0.0f, 0.0f);
	float intermediate;
	for (int i = 0; i < pnts.size(); ++i) {
		intermediate = NRecurs(i, k, t);
		result += pnts[i] * intermediate;
	}
	return result;
}

GsArray<GsVec> SoCurve::crspline(int t, const GsArray<GsVec>& pnts) {
	GsArray<GsVec> result;
	GsVec I = (pnts[t + 1] - pnts[t - 1]) / 2.0f;
	result.push() = pnts[t] - I / 3.0f;
	result.push() = pnts[t];
	result.push() = pnts[t] + I / 3.0f;
	return result;
}
GsArray<GsVec> SoCurve::bospline(int t, const GsArray<GsVec>& pnts) {
	GsArray<GsVec> result;
	GsVec v, vL, vR;
	float dL, dR;
	dL = dist(pnts[t], pnts[t - 1]);							
	dR = dist(pnts[t + 1], pnts[t]);
	vL = (pnts[t] - pnts[t - 1]) / dL;
	vR = (pnts[t + 1] - pnts[t]) / dR;
	v = (dR * vL + dL * vR) / (dR + dL);
	result.push() = pnts[t] - (dL * v) / 3.0f;
	result.push() = pnts[t];
	result.push() = pnts[t] + (dR*v) / 3.0f;
	return result;
}

GsArray<GsVec>& SoCurve::getCopy() {
	return copy;
}