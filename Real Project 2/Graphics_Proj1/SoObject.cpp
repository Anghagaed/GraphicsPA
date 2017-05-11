
# include "SoObject.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

int fac(int x)
{
	int factorial = 1;
	for (int i = 2; i < x; ++i)
		factorial *= i;
	return factorial;
}

FrameAnimation::FrameAnimation() {
	Identity();
}

void FrameAnimation::Identity() {
	for (int i = 0; i < 4; ++i) {
		arm[i].identity();
		armJoint[i].identity();
		leg[i].identity();
		legJoint[i].identity();
	}
	body.identity();
	head.identity();
}

void FrameAnimation::CopyFrom(FrameAnimation& toCopy) {
	for (int i = 0; i < 4; ++i) {
		this->arm[i] = toCopy.arm[i];
		this->leg[i] = toCopy.leg[i];
		this->armJoint[i] = toCopy.armJoint[i];
		this->legJoint[i] = toCopy.legJoint[i];
	}

	this->body = toCopy.body;
	this->head = toCopy.head;
}

GsVec eval_bezier(float t, const GsArray<GsVec>& ctrlpnts)
{
	int n = ctrlpnts.size() - 1;					// summation's n
	GsVec result(0.0f, 0.0f, 0.0f);					// resulting point
	float b_func = 0;								// to determine Bernstein polynomial part
	float bin_coeff;								// binomial coefficient

	for (int c = 0; c <= n; ++c)
	{
		bin_coeff = (float)fac(n) / (fac(n - c)*fac(c));
		b_func = (bin_coeff*pow(t, c))*(pow((1 - t), n - c));
		result += ctrlpnts[c] * b_func;
	}
	return result;
}

SoObject::SoObject()
{
	// Whole object translation stuff
	moveYby = 0.0f;
	moveZby = 0.0f;
	moveXby = 0.0f;
	moveOffset = 0.025f;
	//forwardVec = GsVec(0,0,-1)*moveOffset;

	// Drawing cylinder stuff
	_nfaces = 16;
	_radius = 0.25f/4;

	// Frame Counter for animation
	frameCounter1 = 0;
	frameCounter3 = 0;

	// Jump animation stuff
	animateUp = false;
	animateDown = false;

	// Default object rotation state
	rotateby = 0;

	spinJointArmBool[0] = false;
	spinJointArmBool[1] = false;
	spinJointArmBool[2] = false;
	spinJointArmBool[3] = false;

	frame_num = 0;
	frame = 0;
	animate = false;

	// Rotation
	angleLeftArm1 = 0;
	angleLeftArm2 = 0;
	angleLeftJoint1 = 0;
	angleLeftJoint2 = 0;

	// Create Bezier
	numsegs = 60;
	ctrlpnts.push() = GsVec(0.0f, 0.0f, 0.0f);
	ctrlpnts.push() = GsVec(-0.5f, 0.0f, -0.5f);
	ctrlpnts.push() = GsVec(0.0f, 0.0f, -1.0f);
	ctrlpnts.push() = GsVec(1.0f, 0.0f, -1.0f);
	ctrlpnts.push() = GsVec(0.0f, 0.0f, 0.0f);
	for (int i = 0; i <= numsegs; ++i)
		P.push() = eval_bezier(i * 1.0f / numsegs, ctrlpnts);
}

void SoObject::initAnimation1() {
	// Frame 1 Rotate Arm and Leg
	// Leg
	{
		float dist;
		GsMat translate, rot, final;
		GsVec vec;
		dist = 0.30f;
		translate.translation(0.0f, -dist, 0.0f);
		rot.rotx(-(PI / 3));
		final = translate.inverse()*rot*translate;
		ani1Frame1.leg[3] = final;
		ani1Frame1.leg[2] = final;
		ani1Frame1.legJoint[2] = final;
		ani1Frame1.legJoint[3] = final;
	}
	// Arm Left
	{
		float aRadius = _radius / 2.5;
		float aHeight = 0.05;
		GsMat translate, rot, final;
		GsVec vec;
		rot.rotx(-PI / 2);
		vec = iniPosArmLJ1*(_armJoint.center() + GsVec(-aRadius, aHeight / 2, 0));
		cout << vec << endl;
		translate.translation(-vec.x, -vec.y, -vec.z);
		final = translate.inverse()*rot*translate;
		ani1Frame1.arm[0] = final;
		ani1Frame1.arm[1] = final;
		ani1Frame1.armJoint[0] = final;
		ani1Frame1.armJoint[1] = final;
	}
	// Arm Right
	{
		float aRadius = _radius / 2.5;
		float aHeight = 0.05;
		GsMat translate, rot, final;
		GsVec vec;
		rot.rotx(-PI / 2);
		vec = iniPosArmRJ1*(_armJoint.center() + GsVec(aRadius, aHeight / 2, 0));
		cout << vec << endl;
		translate.translation(-vec.x, -vec.y, -vec.z);
		final = translate.inverse()*rot*translate;
		ani1Frame1.arm[2] = final;
		ani1Frame1.arm[3] = final;
		ani1Frame1.armJoint[2] = final;
		ani1Frame1.armJoint[3] = final;
	}

	//CurrentFrame.CopyFrom(ani1Frame1);
	// 
	// Frame 2 Affects both arm and one leg right 
	// Leg
	{
		float alegRadius = _radius / 6; float alegHeight = _radius * 2;
		float alegJointRadius = 0.025f / 2; float alegJointHeight = _radius / 3;

		GsMat translate, rot, final;
		GsVec vec;
		vec = ani1Frame1.leg[3]*iniPosLegR2*(_leg.center() + GsVec(0, alegHeight / 2, 0));
		translate.translation(-vec.x, -vec.y, -vec.z);
		rot.roty(PI / 4);
		final = translate;
		//ani1Frame2.leg[3] = final;
		CurrentFrame.leg[3] = final;
	}
	// Frame 3 Twirl
}

void SoObject::initAnimation3() {
	// Frame 1 Rotate Arm
	// Arm Left
	{
		float aRadius = _radius / 2.5;
		float aHeight = 0.05;
		GsMat translate, rot, final;
		GsVec vec;
		rot.rotx(-PI / 2);
		vec = iniPosArmLJ1*(_armJoint.center() + GsVec(-aRadius, aHeight / 2, 0));
		cout << vec << endl;
		translate.translation(-vec.x, -vec.y, -vec.z);
		final = translate.inverse()*rot*translate;
		ani3Frame1.arm[0] = final;
		ani3Frame1.arm[1] = final;
		ani3Frame1.armJoint[0] = final;
		ani3Frame1.armJoint[1] = final;
	}
	// Arm Right
	{
		float aRadius = _radius / 2.5;
		float aHeight = 0.05;
		GsMat translate, rot, final;
		GsVec vec;
		rot.rotx(-PI / 2);
		vec = iniPosArmRJ1*(_armJoint.center() + GsVec(aRadius, aHeight / 2, 0));
		cout << vec << endl;
		translate.translation(-vec.x, -vec.y, -vec.z);
		final = translate.inverse()*rot*translate;
		ani3Frame1.arm[2] = final;
		ani3Frame1.arm[3] = final;
		ani3Frame1.armJoint[2] = final;
		ani3Frame1.armJoint[3] = final;
	}
	// 
	// Frame 2 Affects both arm and one leg right
	// Frame 3 Twirl
}

void SoObject::init()
{
	// head
	_head.init(0.0f, 0.0f, 0.0f, "../doggo.png");

	// body
	_body.init(0.0f,0.0f,0.0f, "../metaltexture.jpg");

	// legs
	_leg.init(0.0f, 0.0f, 0.0f, "../metaltexture.jpg");
	_jointLeg.init(0.0f, 0.0f, 0.0f, "../pinktexture.jpg");

	// arms
	_armJoint.init(0.0f, 0.0f, 0.0f, "../pinktexture.jpg");
	_arm.init(0.0f, 0.0f, 0.0f, "../metaltexture.jpg");

}

void SoObject::build()
{
	// head
	_head.build(_radius, 0.2, _nfaces);

	// body
	_body.build(_radius, 0.4, _nfaces);

	// legs
	legRadius = _radius / 6; legHeight = _radius * 2;
	legJointRadius = 0.025f / 2; legJointHeight = _radius / 3;
	_jointLeg.build(legJointRadius, legJointHeight, _nfaces);
	_leg.build(legRadius, legHeight, _nfaces);
	// arms
	armRadius = _radius / 2.5;
	armHeight = 0.05;
	_armJoint.build(armRadius, armHeight, _nfaces);
	_arm.build(armRadius, armHeight, _nfaces);

	// initialPos Matrix Calculation
	GsVec temp;
	GsMat armRot;
	GsMat temp1, temp2, temp3;
	armRot.rotz(M_PI / 2);
	temp2.rotz(-M_PI / 2);																	// Z rotation for the left arm

	// Arm
	iniPosArmLJ1.translation(_radius + armRadius, 0.6, 0);

	temp3.translation(_radius + armRadius*3, 0.6, 0);	// 1 diameter + 1 radius away from left joint 1
	temp = _arm.firstJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x - armRadius, temp.y, temp.z);

	iniPosArmL1 = temp1*temp2*temp1.inverse()*temp3*armRot;
	
	temp3.translation(_radius + armRadius*5, 0.6, 0.0);	// 2 diameter + 1 radius away
	temp = _armJoint.secondJointPts();
	temp = temp3 * temp;
	temp1.translation(temp.x - armRadius*3, temp.y, temp.z);

	iniPosArmLJ2 = temp1*temp2*temp1.inverse()*temp3;

	temp3.translation(_radius + armRadius*7, 0.6, 0);
	temp = _arm.firstJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x - armRadius*5, temp.y, temp.z);
	iniPosArmL2 = temp1*temp2*temp1.inverse()*temp3*armRot;
	

	// right arm
	temp2.rotz(M_PI / 2);
	iniPosArmRJ1.translation(-(_radius + armRadius), 0.6, 0);

	temp3.translation(-(_radius + armRadius*3), 0.6, 0);
	temp = _arm.secondJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x + armRadius, temp.y, temp.z);
	iniPosArmR1 = temp1*temp2*temp1.inverse()*temp3*armRot;
		
	temp3.translation(-(_radius + armRadius*5), 0.6, 0.0);
	temp = _armJoint.firstJointPts();
	temp = temp3 * temp;
	temp1.translation(temp.x + armRadius*3, temp.y, temp.z);
	iniPosArmRJ2 = temp1*temp2*temp1.inverse()*temp3;
	
	temp3.translation(-(_radius + armRadius*7), 0.6, 0);
	temp = _arm.secondJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x + armRadius*5, temp.y, temp.z);
	iniPosArmR2 = temp1*temp2*temp1.inverse()*temp3*armRot;
	// Leg
	/*
	iniPosLegL1 = ; iniPosLegL2 = ;
	iniPosLegR1 = ; iniPosLegR2 = ;
	iniPosLegLJ1 = ; iniPosLegLJ2 = ;
	iniPosLegRJ1 = ; iniPosLegRJ2 = ;
	*/
	// Left Leg
	temp1.rotx(PI / 2);
	temp2.translation(_radius / 4, 0.2875f, 0.0f);
	iniPosLegLJ1 = temp2 * temp1;

	temp1.translation(_radius / 4, 0.2125f, 0.0f);
	iniPosLegL1 = temp1;

	temp1.rotx(PI / 2);
	temp2.translation(_radius / 4, 0.1375f, 0.0f);
	iniPosLegLJ2 = temp2 * temp1;

	temp1.translation(_radius / 4, 0.0625f, 0.0f);
	iniPosLegL2 = temp1;

	// Right Leg
	temp1.rotx(PI / 2);
	temp2.translation(-_radius / 4, 0.2875f, 0.0f);
	iniPosLegRJ1 = temp2 * temp1;

	temp1.translation(-_radius / 4, 0.2125f, 0.0f);
	iniPosLegR1 = temp1;

	temp1.rotx(PI / 2);
	temp2.translation(-_radius / 4, 0.1375, 0.0f);
	iniPosLegRJ2 = temp2 * temp1;

	// Drawing Right Leg 2
	temp1.translation(-_radius / 4, 0.0625f, 0.0f);
	iniPosLegR2 = temp1;

	initAnimation1();
	initAnimation3();
}


// lift arms up
void SoObject::keyFrame1(bool &_animate)
{
	animate = _animate;
	GsMat temp1, temp2, temp3, armRot;
	GsVec temp;
	armRot.rotz(M_PI / 2);

	temp2.rotz(-M_PI / 2 + (frame) * M_PI / 120);

	// left arm
	newPosArmLJ1.translation(_radius + 0.025, 0.6, 0);

	temp3.translation(_radius + 0.075, 0.6, 0);
	temp = _arm.firstJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x - 0.025, temp.y, temp.z);
	newPosArmL1 = temp1*temp2*temp1.inverse()*temp3*armRot;

	temp3.translation(_radius + 0.125, 0.6, 0.0);
	temp = _armJoint.secondJointPts();
	temp = temp3 * temp;
	temp1.translation(temp.x - 0.075, temp.y, temp.z);
	newPosArmLJ2 = temp1*temp2*temp1.inverse()*temp3;

	temp3.translation(_radius + 0.175, 0.6, 0);
	temp = _arm.firstJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x - 0.125, temp.y, temp.z);
	newPosArmL2 = temp1*temp2*temp1.inverse()*temp3*armRot;

	// right arm
	// right arm
	temp2.rotz(M_PI / 2 - frame * M_PI / 120);
	newPosArmRJ1.translation(-(_radius + 0.025), 0.6, 0);

	temp3.translation(-(_radius + 0.075), 0.6, 0);
	temp = _arm.secondJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x + 0.025, temp.y, temp.z);
	newPosArmR1 = temp1*temp2*temp1.inverse()*temp3*armRot;

	temp3.translation(-(_radius + 0.125), 0.6, 0.0);
	temp = _armJoint.firstJointPts();
	temp = temp3 * temp;
	temp1.translation(temp.x + 0.075, temp.y, temp.z);
	newPosArmRJ2 = temp1*temp2*temp1.inverse()*temp3;

	temp3.translation(-(_radius + 0.175), 0.6, 0);
	temp = _arm.secondJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x + 0.125, temp.y, temp.z);
	newPosArmR2 = temp1*temp2*temp1.inverse()*temp3*armRot;
	//cout << frame << endl;
	if (frame < 60)
		++frame;
	else
	{
		frame = 0;
		_animate = false;
	}
}

// Return 0 if animation is done 1 if animation is still going on
int SoObject::animationOne() {
	if (frameCounter1 < 40) {
		// Frame 1
		frameCounter1++;
		return 1;
	}
	else if (frameCounter1 < 80) {
		// Frame 2
		frameCounter1++;
		return 1;
	}
	else if (frameCounter1 < 120) {
		// Frame 3
		frameCounter1++;
		return 1;
	}
	else if (frameCounter1 == 120) {
		frameCounter1 = 0;
		return 0;
	}

}

int SoObject::animationThree()
{
	if (frameCounter3 < 40) {
		// Frame 1
		frameCounter3++;
		return 1;
	}
	else if (frameCounter3 < 80) {
		// Frame 2
		frameCounter3++;
		return 1;
	}
	else if (frameCounter3 < 120) {
		// Frame 3
		frameCounter3++;
		return 1;
	}
	else if (frameCounter3 == 120) {
		frameCounter3 = 0;
		return 0;
	}
}

void SoObject::draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs, const GsVec lcoord)
{
	// Whole object translation
	GsMat temp1, temp2;
	temp1.translation(moveXby, moveYby, moveZby);						// rigid translation
	temp2.roty(rotateby);												// rotation matrix
	translationMatrix = temp1 * temp2;

	ftransform = tr*translationMatrix;									// apply perspective matrix? (this should be multiplied at the most left corner)
	
	// Head:
	drawHead(pr, l, fs, lcoord);

	// Body:
	drawBody(pr, l, fs, lcoord);
	
	// Legs:
	drawLegs(pr, l, fs, lcoord);

	// Arms:
	drawArms(pr, l, fs, lcoord);
}

void SoObject::drawHead(const GsMat & pr, const GsLight & l, const float & fs, const GsVec lcoord)
{
	GsMat temp1, temp2;
	temp1.translation(0.0f, 0.8f, 0.0f);								// rigid translation matrix (to put the head in the correct place)
	temp2.roty(M_PI / 2);												// rotation matrix (to align with the whole body)
	_head.draw(ftransform*CurrentFrame.head*temp1*temp2, pr, l, fs);					// draw head
}

void SoObject::drawArms(const GsMat & pr, const GsLight & l, const float & fs, const GsVec lcoord)
{
	GsMat temp1, temp2;
	GsMat armRot, spin;
	GsVec temp;
	armRot.rotz(M_PI / 2);

	// Drawing left arm

	//temp2.rotz(M_PI / 4);																	// Z rotation for the left arm

		// Left arm joint1
	_armJoint.draw(ftransform*CurrentFrame.armJoint[0] * iniPosArmLJ1, pr, l, fs);		// draw joint (between body and left arm)

	// Left arm1
	_arm.draw(ftransform*CurrentFrame.arm[0] * iniPosArmL1, pr, l, fs);			// draw left arm1

	// Left arm joint2
	_armJoint.draw(ftransform*CurrentFrame.armJoint[1] * iniPosArmLJ2, pr, l, fs);		// draw joint (between left arms)

	// Left arm2
	_arm.draw(ftransform*CurrentFrame.arm[1] * iniPosArmL2, pr, l, fs);			// draw left arm2

	// Right arm
	// Right arm joint1	
	_armJoint.draw(ftransform*CurrentFrame.armJoint[2] * iniPosArmRJ1, pr, l, fs);		// draw joint (between body and right arm)

	// Right arm1
	_arm.draw(ftransform*CurrentFrame.arm[2] * iniPosArmR1, pr, l, fs);			// draw right arm1

	// Right arm joint2
	_armJoint.draw(ftransform*CurrentFrame.armJoint[3] * iniPosArmRJ2, pr, l, fs);		// draw joint (between right arms)

	// Right arm2
	_arm.draw(ftransform*CurrentFrame.arm[3] * iniPosArmR2, pr, l, fs);			// draw right arm2
}

void SoObject::drawLegs(const GsMat & pr, const GsLight & l, const float & fs, const GsVec lcoord)
{
	GsMat temp1, temp2;
	// Drawing Left Leg
	// _jointLegL1
	_jointLeg.draw(ftransform*CurrentFrame.legJoint[0]*iniPosLegLJ1, pr, l, fs);

	// Drawing Left Leg 1
	_leg.draw(ftransform*CurrentFrame.leg[0]*iniPosLegL1, pr, l, fs);

	// _jointLegL2
	_jointLeg.draw(ftransform*CurrentFrame.legJoint[1]*iniPosLegLJ2, pr, l, fs);

	// Drawing Left Leg 2
	_leg.draw(ftransform *CurrentFrame.leg[1]*iniPosLegL2, pr, l, fs);

	// Drawing Right Leg
	// _jointLegR1
	_jointLeg.draw(ftransform*CurrentFrame.legJoint[2]*iniPosLegRJ1, pr, l, fs);

	// Drawing Right Leg 1
	_leg.draw(ftransform*CurrentFrame.leg[2]*iniPosLegR1, pr, l, fs);

	// _jointLegR2
	_jointLeg.draw(ftransform*CurrentFrame.legJoint[3]*iniPosLegRJ2, pr, l, fs);

	// Drawing Right Leg 2
	_leg.draw(ftransform*CurrentFrame.leg[3]*iniPosLegR2, pr, l, fs);
}

void SoObject::drawBody(const GsMat & pr, const GsLight & l, const float & fs, const GsVec lcoord)
{
	GsMat temp1, temp2;
	temp1.translation(0.0f, 0.5f, 0.0f);
	_body.draw(ftransform*CurrentFrame.body*temp1, pr, l, fs);				// draw body
}


void SoObject::jump(bool& animate)
{
	if (animate == true)
	{
		if (moveYby >= 1.0f) 
		{
			animateDown = true;
			animateUp = false;
		}
		if(animateDown == false)
			animateUp = true;
		const float rate = 0.01f;
		if (animateUp == true)
			moveYby += rate;
		else
		if(animateDown == true)
			moveYby -= rate;
		if (moveYby <= 0.0f)
		{
			animate = false;
			animateUp = false;
			animateDown = false;
			moveYby = 0.0f;
		}
	}
}

void SoObject::move(bool& animate)
{
	if (frame_num == numsegs + 1)
		frame_num = 0;
	if (animate == true)
	{
		moveXby = P[frame_num].x;
		moveZby = P[frame_num].z;
		frame_num++;
	}
}


void SoObject::turn(bool & animate, bool & left)
{
	if (animate == true)
	{
		if (left == true)
			rotateby += M_PI / 4;
		else
			rotateby -= M_PI / 4;
		animate = false;
	}
}
