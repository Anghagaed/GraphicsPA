
// Ensure the header file is included only once in multi-file projects
#ifndef SO_HUMAN_H
#define SO_HUMAN_H

// Include needed header files
#include <vector>
#include <string>
#include "so_humanpart.h"

using namespace std;

// Class designed to represent different human parts using cylinder and sphere defined in so_texturedsphere and so_texturedcylinder

class SoHuman {
private:
	string imagePath, jointImage;
	SoHumanPart head, torso, leftArm, leftLeg, rightArm, rightLeg;
private:
	// create functions that creates the base unit to form the human parts
	// no abuse, only call once
	void createHead(const GsMat& shadowMat);
	void createTorso(const GsMat& shadowMat);
	void createLeftArm(const GsMat& shadowMat);
	void createLeftLeg(const GsMat& shadowMat);
	void createRightArm(const GsMat& shadowMat);
	void createRightLeg(const GsMat& shadowMat);

public:
	SoHuman();
	~SoHuman();
	
	// init and build all SoHumanPart objects
	void init(const GsMat& shadowMat);
	// draw all SoHumanPart objects and its shadow
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l);

	// Reset all parts
	void reset();

	// Head interface
	void headReset() { head.reset(); }

	// Torso interface
	void torsoReset() { torso.reset(); }

	// leftArm interface
	void leftArmReset() { leftArm.reset(); }

	// rightArm interface
	void rightArmReset() { rightArm.reset(); }

	// leftLeg interface
	void leftLegReset() { leftLeg.reset(); }

	// rightLeg interface
	void rightLegReset() { rightLeg.reset(); }

	void updateGlbMov(float dx, float dz);

	void rotateArm(float dy);
};


#endif SO_HUMANPART_H
