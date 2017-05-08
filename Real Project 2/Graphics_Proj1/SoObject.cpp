
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

	// Rotation
	angleLeftArm1 = 0;
	angleLeftArm2 = 0;
	angleLeftJoint1 = 0;
	angleLeftJoint2 = 0;

	// LeftArm build
	LArm1vec = GsVec(_radius + 0.075, 6.0, 0.0);

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
	_jointLeg.build(0.025f/2, _radius / 3, _nfaces);
	_leg.build(_radius / 6, 0.125f, _nfaces);
	
	// arms
	_armJoint.build(0.025, 0.05, _nfaces);
	_arm.build(0.025, 0.05, _nfaces);

	// initialPos Matrix Calculation
	GsVec temp;
	GsMat armRot;
	GsMat temp1, temp2, temp3;
	armRot.rotz(M_PI / 2);
	temp2.rotz(M_PI / 4);																	// Z rotation for the left arm

	iniPosArmLJ1.translation(_radius + 0.025, 0.6, 0);

	temp3.translation(_radius + 0.075, 0.6, 0);
	temp = _arm.firstJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x - 0.025, temp.y, temp.z);

	iniPosArmL1 = temp1*temp2*temp1.inverse()*temp3*armRot;
	
	temp3.translation(_radius + 0.125, 0.6, 0.0);
	temp = _armJoint.secondJointPts();
	temp = temp3 * temp;
	temp1.translation(temp.x - 0.075, temp.y, temp.z);

	iniPosArmLJ2 = temp1*temp2*temp1.inverse()*temp3;

	temp3.translation(_radius + 0.175, 0.6, 0);
	temp = _arm.firstJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x - 0.125, temp.y, temp.z);

	iniPosArmL2 = temp1*temp2*temp1.inverse()*temp3*armRot;
	
	iniPosArmRJ1.translation(-(_radius + 0.025), 0.6, 0);

	temp3.translation(-(_radius + 0.075), 0.6, 0);
	temp = _arm.secondJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x + 0.025, temp.y, temp.z);
	iniPosArmR1 = temp1*temp2*temp1.inverse()*temp3*armRot;
		
	temp3.translation(-(_radius + 0.125), 0.6, 0.0);
	temp = _armJoint.firstJointPts();
	temp = temp3 * temp;
	temp1.translation(temp.x + 0.075, temp.y, temp.z);
	iniPosArmRJ2 = temp1*temp2*temp1.inverse()*temp3;
	
	temp3.translation(-(_radius + 0.175), 0.6, 0);
	temp = _arm.secondJointPts();
	temp = temp3 * armRot * temp;
	temp1.translation(temp.x + 0.125, temp.y, temp.z);
	iniPosArmR2 = temp1*temp2*temp1.inverse()*temp3*armRot;
	/*
	iniPosLegL1, iniPosLegL2;
	iniPosLegR1, iniPosLegR2;
	iniPosLegLJ1, iniPosLegLJ2;
	iniPosLegRJ1, iniPosLegRJ2;*/
}

void SoObject::keyFrame1(bool& animate)
{
	/*spinJointArmBool[0] = false;
	spinJointArmBool[1] = true;
	spinJointArmBool[2] = false;*/
//	spinJointArmBool[3] = true;
}

void SoObject::vturnLArm1(const float& amount, const GsMat& transMatrix)
{
	GsMat temp1;
	angleLeftArm1 += amount;
	temp1.rotz(angleLeftArm1);
	vLArm1 = transMatrix.inverse() * temp1 * transMatrix;
}

void SoObject::hturnLArm1(const float& amount, const GsMat& transMatrix)
{
	GsMat temp1;
	angleLeftArm1 += amount;
	temp1.roty(angleLeftArm1);
	vLArm1 = transMatrix.inverse() * temp1 * transMatrix;
}

void SoObject::vturnLArm2(const float& amount, const GsMat& transMatrix)
{
	GsMat temp1;
	angleLeftArm2 += amount;
	temp1.rotz(angleLeftArm2);
	vLArm2 = transMatrix.inverse() * temp1 * transMatrix;
}

void SoObject::hturnLArm2(const float& amount, const GsMat& transMatrix)
{
	GsMat temp1;
	angleLeftArm2 += amount;
	temp1.roty(angleLeftArm2);
	vLArm2 = transMatrix.inverse() * temp1 * transMatrix;
}

void SoObject::rotateWholeArmDown(GsMat& rotateMe)
{
	GsMat temp1;
	wholeArmRot += M_PI;
	temp1.rotz(wholeArmRot);
	rotateMe = rotateMe * temp1 * rotateMe.inverse();
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
	_head.draw(ftransform * temp1 * temp2, pr, l, fs);					// draw head
}

void SoObject::drawArms(const GsMat & pr, const GsLight & l, const float & fs, const GsVec lcoord)
{
	GsMat temp1, temp2;
	GsMat armRot, spin;
	GsVec temp;
	armRot.rotz(M_PI / 2);

	// Drawing left arm

	temp2.rotz(M_PI / 4);																	// Z rotation for the left arm

	// Left arm joint1
	_armJoint.draw(ftransform*iniPosArmLJ1, pr, l, fs);		// draw joint (between body and left arm)
												
	// Left arm1
	_arm.draw(ftransform*iniPosArmL1, pr, l, fs);			// draw left arm1
	
	// Left arm joint2
	_armJoint.draw(ftransform*iniPosArmLJ2, pr, l, fs);		// draw joint (between left arms)
											
	// Left arm2
	_arm.draw(ftransform*iniPosArmL2, pr, l, fs);			// draw left arm2
	

	// Right arm
	// Right arm joint1

	temp2.roty(PI / 4);																	// Y rotation for the right arm

	_armJoint.draw(ftransform*iniPosArmRJ1, pr, l, fs);		// draw joint (between body and right arm)

	// Right arm1
	_arm.draw(ftransform*iniPosArmR1, pr, l, fs);			// draw right arm1

	// Right arm joint2
	_armJoint.draw(ftransform*iniPosArmRJ2, pr, l, fs);		// draw joint (between right arms)

	// Right arm2
	_arm.draw(ftransform*iniPosArmR2, pr, l, fs);			// draw left arm2
}

void SoObject::drawLegs(const GsMat & pr, const GsLight & l, const float & fs, const GsVec lcoord)
{
	GsMat temp1, temp2;
	// Drawing Left Leg
	// _jointLegL1
	temp1.rotx(PI / 2);
	temp2.translation(_radius / 4, 0.2875f, 0.0f);
	_jointLeg.draw(ftransform * temp2 * temp1, pr, l, fs);

	// Drawing Left Leg 1
	temp1.translation(_radius / 4, 0.2125f, 0.0f);
	_leg.draw(ftransform * temp1, pr, l, fs);

	// _jointLegL2
	temp1.rotx(PI / 2);
	temp2.translation(_radius / 4, 0.1375f, 0.0f);
	_jointLeg.draw(ftransform * temp2 * temp1, pr, l, fs);

	// Drawing Left Leg 2
	temp1.translation(_radius / 4, 0.0625f, 0.0f);
	_leg.draw(ftransform * temp1, pr, l, fs);

	// Drawing Right Leg
	// _jointLegR1
	temp1.rotx(PI / 2);
	temp2.translation(-_radius / 4, 0.2875f, 0.0f);
	_jointLeg.draw(ftransform * temp2 * temp1, pr, l, fs);

	// Drawing Right Leg 1
	temp1.translation(-_radius / 4, 0.2125f, 0.0f);
	_leg.draw(ftransform * temp1, pr, l, fs);

	// _jointLegR2
	temp1.rotx(PI / 2);
	temp2.translation(-_radius / 4, 0.1375, 0.0f);
	_jointLeg.draw(ftransform * temp2 * temp1, pr, l, fs);

	// Drawing Right Leg 2
	temp1.translation(-_radius / 4, 0.0625f, 0.0f);
	_leg.draw(ftransform * temp1, pr, l, fs);
}

void SoObject::drawBody(const GsMat & pr, const GsLight & l, const float & fs, const GsVec lcoord)
{
	GsMat temp1, temp2;
	temp1.translation(0.0f, 0.5f, 0.0f);
	_body.draw(ftransform * temp1, pr, l, fs);				// draw body
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
