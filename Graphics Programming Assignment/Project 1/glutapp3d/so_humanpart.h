
// Ensure the header file is included only once in multi-file projects
#ifndef SO_HUMANPART_H
#define SO_HUMANPART_H

// Include needed header files
#include <vector>
#include <string>
#include "so_texturedsphere.h"
#include "so_texturedcylinder.h"

using namespace std;

// Class designed to represent different human parts using cylinder and sphere defined in so_texturedsphere and so_texturedcylinder

class SoHumanPart {
private:
	string imagePath;
	GsMat shadow;
	vector<SoTexturedSphere*> spherList;
	vector<SoTexturedCylinder*> cylinList;

	// Sphere Parameters
	int sLfaces, sLayers;
	// Cylinder Parameters
	int rNFaces;
public:
	SoHumanPart();
	~SoHumanPart();
	SoHumanPart(const GsMat& shadowMat, const string& imagePath);
	SoHumanPart(const string& imagePath);
	void init(const GsMat& shadowMat, const string& imagePath);
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l);
	void createSphere(float r, const GsMat& initialPos, const GsMat& myTrans);
	void createCylinder(float r, float l, const GsMat& initialPos, const GsMat& myTrans);
};


#endif SO_HUMANPART_H
