# include "so_human.h"

SoHuman::SoHuman() : imagePath("") {}

SoHuman::~SoHuman() {}

void SoHuman::init(const GsMat& shadowMat) {
	createHead(shadowMat);
	createTorso(shadowMat);
	createLeftArm(shadowMat);
	createLeftLeg(shadowMat);
	createRightArm(shadowMat);
	createRightLeg(shadowMat);
}

void SoHuman::draw(const GsMat& tr, const GsMat& pr, const GsLight& l) {
	head.draw(tr, pr, l);
	torso.draw(tr, pr, l);
	leftArm.draw(tr, pr, l);
	leftLeg.draw(tr, pr, l);
	rightArm.draw(tr, pr, l);
	rightLeg.draw(tr, pr, l);
}

void SoHuman::reset() {
	headReset();
	torsoReset();
	leftArmReset();
	rightArmReset();
	leftLegReset();
	rightLegReset();
}

void SoHuman::createHead(const GsMat& shadowMat) {
	imagePath = "../skin1.png";
	jointImage = "../skin3.png";
	head.init(shadowMat, imagePath);
	GsMat Pos, Trans;
	// create joint with Torso
	Pos.translation(0.0f, 0.60f, 0.0f);
	Trans.identity();
	head.createCylinder(0.15f, 0.20f, Pos, Trans, jointImage);
	// create head
	Pos.translation(0.0f, 0.80f, 0.0f);
	Trans.identity();
	head.createSphere(0.20, Pos, Trans);
}

void SoHuman::createTorso(const GsMat& shadowMat) {
	imagePath = "../skin2.png";
	torso.init(shadowMat, imagePath);
	GsMat Pos, Trans;
	Trans.identity();
	Pos.translation(0.0f, 0.2f, 0.0f);
	torso.createCylinder(0.35f, 0.6f, Pos, Trans);
}

void SoHuman::createLeftArm(const GsMat& shadowMat) {
	imagePath = "../skin1.png";
	leftArm.init(shadowMat, imagePath);
	GsMat Pos, Trans, temp;
	// Joint with Torso
	Trans.identity();
	Pos.translation(-0.40f, 0.20f, 0.0f);
	leftArm.createSphere(0.15f, Pos, Trans, jointImage);
	// Arm
	Trans.identity();
	Pos.translation(-0.65f, 0.20f, 0.0f);
	temp.rotz(PIOVER2);
	leftArm.createCylinder(0.15f, 0.4f, Pos * temp, Trans);
}

void SoHuman::createLeftLeg(const GsMat& shadowMat) {
	imagePath = "../skin1.png";
	leftLeg.init(shadowMat, imagePath);
	GsMat Pos, Trans, temp;
	// Joint with Torso
	Trans.identity();
	Pos.translation(-0.15f, -0.175f, 0.0f);
	leftLeg.createSphere(0.125f, Pos, Trans, jointImage);
	// Leg
	Trans.identity();
	Pos.translation(-0.15f, -0.50f, 0.0f);
	leftLeg.createCylinder(0.125f, 0.6f, Pos, Trans);
}

void SoHuman::createRightArm(const GsMat& shadowMat) {
	imagePath = "../skin1.png";
	rightArm.init(shadowMat, imagePath);
	GsMat Pos, Trans, temp;
	// Joint with Torso
	Trans.identity();
	Pos.translation(0.40f, 0.20f, 0.0f);
	rightArm.createSphere(0.15f, Pos, Trans, jointImage);
	// Arm
	Trans.identity();
	Pos.translation(0.65f, 0.20f, 0.0f);
	temp.rotz(PIOVER2);
	rightArm.createCylinder(0.15f, 0.4f, Pos * temp, Trans);
}


void SoHuman::createRightLeg(const GsMat& shadowMat) {
	imagePath = "../skin1.png";
	rightLeg.init(shadowMat, imagePath);
	GsMat Pos, Trans, temp;
	// Joint with Torso
	// Joint with Torso
	Trans.identity();
	Pos.translation(0.15f, -0.175f, 0.0f);
	leftLeg.createSphere(0.125f, Pos, Trans, jointImage);
	// Leg
	Trans.identity();
	Pos.translation(0.15f, -0.50f, 0.0f);
	leftLeg.createCylinder(0.125f, 0.6f, Pos, Trans);
}

void SoHuman::updateGlbMov(float dx, float dz) {
	GsMat glbMov;
	glbMov.translation(dx, 0.0f, dz);
	head.updateGlobalMovement(glbMov);
	torso.updateGlobalMovement(glbMov);
	leftArm.updateGlobalMovement(glbMov);
	rightArm.updateGlobalMovement(glbMov);
	leftLeg.updateGlobalMovement(glbMov);
	rightLeg.updateGlobalMovement(glbMov);
}

void SoHuman::rotateArm(float dy) {
	GsMat changeOfBasis, rot;
	// Pos.translation(-0.40f, 0.20f, 0.0f);
	changeOfBasis.translation(-0.40f, 0.20f, 0.0f);
	rot.rotz(dy);
	leftArm.updateMyTrans(changeOfBasis * rot * changeOfBasis.inverse());

	// Pos.translation(0.40f, 0.20f, 0.0f);
	changeOfBasis.translation(0.40f, 0.20f, 0.0f);
	rot.rotz(-dy);
	rightArm.updateMyTrans(changeOfBasis * rot * changeOfBasis.inverse());
}