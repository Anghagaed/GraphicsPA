# include "so_human.h"

SoHuman::SoHuman() : imagePath("") {}

SoHuman::~SoHuman() {}

void SoHuman::init(const GsMat& shadowMat, const string& imagePath) {
	this->imagePath = imagePath;
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
	head.init(shadowMat, imagePath);
	GsMat Pos, Trans;
	Pos.setl1(1.0f, 0.0f, 0.0f, 0.0f);
	Pos.setl2(0.0f, 1.0f, 0.0f, 0.5f);
	Pos.setl3(0.0f, 0.0f, 1.0f, 0.0f);
	Pos.setl4(0.0f, 0.0f, 0.0f, 1.0f);
	Trans.identity();
	head.createSphere(0.2, Pos, Trans);
}

void SoHuman::createTorso(const GsMat& shadowMat) {

}

void SoHuman::createLeftArm(const GsMat& shadowMat) {

}

void SoHuman::createLeftLeg(const GsMat& shadowMat) {

}

void SoHuman::createRightArm(const GsMat& shadowMat) {

}

void SoHuman::createRightLeg(const GsMat& shadowMat) {

}