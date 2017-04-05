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
	//cout << "spherList.size " << spherList.size() << endl;
	//cout << "cylinList.size " << cylinList.size() << endl;
	for (int i = 0; i < spherList.size(); ++i) {
		//cout << "in draw sphere " << i << endl;
		spherList[i]->draw(tr, pr, l);
		spherList[i]->drawShadow(tr, pr, l);
	}

	for (int i = 0; i < cylinList.size(); ++i) {
		//cout << "in draw cylinder" << endl;
		cylinList[i]->draw(tr, pr, l);
		cylinList[i]->drawShadow(tr, pr, l);
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

	/*
	spherList.push_back(toPush);
	spherList[spherList.size() - 1].init(0, 0, 0, imagePath);
	spherList[spherList.size() - 1].build(r, sLfaces, sLayers);
	spherList[spherList.size() - 1].setInitialPos(initialPos);
	spherList[spherList.size() - 1].setMyTrans(myTrans);
	spherList[spherList.size() - 1].setShadow(shadow);
	*/
}

void SoHumanPart::createCylinder(float r, float l, const GsMat& initialPos, const GsMat& myTrans) {
	SoTexturedCylinder* toPush = new SoTexturedCylinder();

	toPush->init(0, 0, 0, imagePath);
	toPush->build(r, l, rNFaces);
	toPush->setInitialPos(initialPos);
	toPush->setMyTrans(myTrans);
	toPush->setShadow(shadow);

	cylinList.push_back(toPush);
	/*
	cylinList.push_back(toPush);
	cylinList[cylinList.size() - 1].init(0, 0, 0, imagePath);
	cylinList[cylinList.size() - 1].build(r, l, rNFaces);
	cylinList[cylinList.size() - 1].setInitialPos(initialPos);
	cylinList[cylinList.size() - 1].setMyTrans(myTrans);
	cylinList[cylinList.size() - 1].setShadow(shadow);
	*/
}