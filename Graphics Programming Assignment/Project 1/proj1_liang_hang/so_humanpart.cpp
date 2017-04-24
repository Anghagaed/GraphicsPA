# include "so_humanpart.h"

SoHumanPart::SoHumanPart() {
	imagePath = "";
	shadow.identity();
	sLayers = 6; sLfaces = 20; rNFaces = 20;
}

SoHumanPart::~SoHumanPart() {
	for (int i = 0; i < spherList.size(); ++i) {
		delete spherList[i];
	}

	for (int i = 0; i < cylinList.size(); ++i) {
		delete cylinList[i];
	}
}

SoHumanPart::SoHumanPart(const GsMat& shadowMat, const string& imagePath) {
	shadow = shadowMat;
	this->imagePath = imagePath;
	sLayers = 6; sLfaces = 20; rNFaces = 20;
}

SoHumanPart::SoHumanPart(const string& imagePath) {
	this->imagePath = imagePath;
	sLayers = 6; sLfaces = 20; rNFaces = 20;
	shadow.identity();
}

void SoHumanPart::init(const GsMat& shadowMat, const string& imagePath) {
	shadow = shadowMat;
	this->imagePath = imagePath;
	sLayers = 6; sLfaces = 20; rNFaces = 20;
}

void SoHumanPart::draw(const GsMat& tr, const GsMat& pr, const GsLight& l) {
	for (int i = 0; i < spherList.size(); ++i) {
		spherList[i]->draw(tr, pr, l);
		spherList[i]->drawShadow(tr, pr, l);
	}

	for (int i = 0; i < cylinList.size(); ++i) {
		cylinList[i]->draw(tr, pr, l);
		cylinList[i]->drawShadow(tr, pr, l);
	}
}

void SoHumanPart::reset() {
	GsMat iden;
	iden.identity();
	for (int i = 0; i < spherList.size(); ++i) {
		spherList[i]->setMyTrans(iden);
	}

	for (int i = 0; i < cylinList.size(); ++i) {
		cylinList[i]->setMyTrans(iden);
	}
}

void SoHumanPart::createSphere(float r, const GsMat& initialPos, const GsMat& myTrans) {
	SoTexturedSphere* toPush =  new SoTexturedSphere();
	toPush->init(0, 0, 0, imagePath);
	toPush->build(r, sLfaces, sLayers);
	toPush->setInitialPos(initialPos);
	toPush->setMyTrans(myTrans);
	toPush->setShadow(shadow);

	spherList.push_back(toPush);
}

void SoHumanPart::createCylinder(float r, float l, const GsMat& initialPos, const GsMat& myTrans) {
	SoTexturedCylinder* toPush = new SoTexturedCylinder();

	toPush->init(0, 0, 0, imagePath);
	toPush->build(r, l, rNFaces);
	toPush->setInitialPos(initialPos);
	toPush->setMyTrans(myTrans);
	toPush->setShadow(shadow);

	cylinList.push_back(toPush);
}

void SoHumanPart::createSphere(float r, const GsMat& initialPos, const GsMat& myTrans, string imagePath) {
	SoTexturedSphere* toPush = new SoTexturedSphere();
	toPush->init(0, 0, 0, imagePath);
	toPush->build(r, sLfaces, sLayers);
	toPush->setInitialPos(initialPos);
	toPush->setMyTrans(myTrans);
	toPush->setShadow(shadow);

	spherList.push_back(toPush);
}

void SoHumanPart::createCylinder(float r, float l, const GsMat& initialPos, const GsMat& myTrans, string imagePath) {
	SoTexturedCylinder* toPush = new SoTexturedCylinder();

	toPush->init(0, 0, 0, imagePath);
	toPush->build(r, l, rNFaces);
	toPush->setInitialPos(initialPos);
	toPush->setMyTrans(myTrans);
	toPush->setShadow(shadow);

	cylinList.push_back(toPush);
}

void SoHumanPart::updateGlobalMovement(const GsMat& glbMov) {
	for (int i = 0; i < spherList.size(); ++i) {
		spherList[i]->setGlbMov(glbMov);
	}

	for (int i = 0; i < cylinList.size(); ++i) {
		cylinList[i]->setGlbMov(glbMov);
	}
}

void SoHumanPart::updateMyTrans(const GsMat& rot) {
	/*
	for (int i = 0; i < spherList.size(); ++i) {
		spherList[i]->applyMyTrans(rot);
	}
	*/
	for (int i = 0; i < cylinList.size(); ++i) {
		cylinList[i]->setJntMov(rot);
	}
}