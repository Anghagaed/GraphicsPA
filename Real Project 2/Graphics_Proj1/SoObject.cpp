
# include "SoObject.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

SoObject::SoObject()
{
	// Whole object translation stuff
	moveYby = 0.0f;
	moveZby = 0.0f;
	moveXby = 0.0f;
	moveOffset = 0.025f;
	forwardVec = GsVec(0,0,-1)*moveOffset;

	// Drawing cylinder stuff
	_nfaces = 16;
	_radius = 0.25f/4;

	// Jump animation stuff
	animateUp = false;
	animateDown = false;

	// Default object rotation state
	rotateby = 0;
	rotateL1by = 135.0f*(M_PI/180.0f);
	rotateL2by = -135.0f*(M_PI/180.0f);
	rotateA1by = -30.0f*(M_PI / 180.0f);
	rotateA2by = 150.0f*(M_PI/180.0f);
	rotateHby = 0.0f;
}

void SoObject::init()
{
	// body
	_body.init(0, 0.5, 0,"../metaltexture.jpg");

	// left arm
	_armLeftJoint1.init(0, 0, 0, "../pinktexture.jpg");
	_armLeftJoint2.init(0, 0, 0, "../pinktexture.jpg");
	_armLeft1.init(0, 0, 0, "../metaltexture.jpg");
	_armLeft2.init(0, 0, 0, "../metaltexture.jpg");
	_armRightJoint1.init(0, 0, 0, "../pinktexture.jpg");
	_armRightJoint2.init(0, 0, 0, "../pinktexture.jpg");
	_armRight1.init(0, 0, 0, "../metaltexture.jpg");
	_armRight2.init(0, 0, 0, "../metaltexture.jpg");
}

void SoObject::build()
{
	// body
	_body.build(_radius, 0.4, _nfaces);
	
	// left arm
	_armLeftJoint1.build(0.025, 0.05, _nfaces);
	_armLeftJoint2.build(0.025, 0.05, _nfaces);
	_armLeft1.build(0.025, 0.05, _nfaces);
	_armLeft2.build(0.025, 0.05, _nfaces);

	// right arm
	_armRightJoint1.build(0.025, 0.05, _nfaces);
	_armRightJoint2.build(0.025, 0.05, _nfaces);
	_armRight1.build(0.025, 0.05, _nfaces);
	_armRight2.build(0.025, 0.05, _nfaces);
}

void SoObject::draw(const GsMat& stransf, const GsMat& sproj, const GsLight& l, const float& fs, const GsVec lcoord)
{
	GsMat ftransform;									// final transform matrix

	// Whole object translation
	GsMat translationMatrix, temp1, temp2;
	temp1.translation(moveXby, moveYby, moveZby);		// rigid translation
	temp2.roty(rotateby);								// rotation matrix
	translationMatrix = temp1 * temp2;

	// Body:
	// Drawing body
	ftransform = stransf*translationMatrix;
	_body.draw(ftransform, sproj, l, fs);				// draw body

	// Arms:
	GsMat armRot;
	armRot.rotz(M_PI/2);
	// Drawing arms
	// Left arm joint1
	GsMat armLJoint1Pos;
	armLJoint1Pos.translation(_radius + 0.025, 0.6, 0);
	ftransform = stransf*translationMatrix*armLJoint1Pos;
	_armLeftJoint1.draw(ftransform, sproj, l, fs);		// draw joint (between body and left arm)
	// Left arm joint2
	GsMat armLJoint2Pos;
	armLJoint2Pos.translation(_radius + 0.025 + 0.1, 0.6, 0);
	ftransform = stransf*translationMatrix*armLJoint2Pos;
	_armLeftJoint2.draw(ftransform, sproj, l, fs);		// draw joint (between left arms)
	// Left arm1
	GsMat lArm1Pos;
	lArm1Pos.translation(_radius + 0.075, 0.6, 0);
	ftransform = stransf*translationMatrix*lArm1Pos*armRot;
	_armLeft1.draw(ftransform, sproj, l, fs);		// draw joint (between left arms)
	// Left arm2
	GsMat lArm2Pos;
	lArm2Pos.translation(_radius + 0.025 + 0.1 + 0.05, 0.6, 0);
	ftransform = stransf*translationMatrix*lArm2Pos*armRot;
	_armLeft2.draw(ftransform, sproj, l, fs);		// draw joint (between left arms)

	// Right arm joint1
	GsMat armRJoint1Pos;
	armRJoint1Pos.translation(-(_radius + 0.025), 0.6, 0);
	ftransform = stransf*translationMatrix*armRJoint1Pos;
	_armLeftJoint1.draw(ftransform, sproj, l, fs);		// draw joint (between body and left arm)
	// Right arm joint2
	GsMat armRJoint2Pos;
	armRJoint2Pos.translation(-(_radius + 0.025 + 0.1), 0.6, 0);
	ftransform = stransf*translationMatrix*armRJoint2Pos;
	_armLeftJoint2.draw(ftransform, sproj, l, fs);		// draw joint (between left arms)
	// Right arm1
	GsMat rArm1Pos;
	rArm1Pos.translation(-(_radius + 0.075), 0.6, 0);
	ftransform = stransf*translationMatrix*rArm1Pos*armRot;
	_armRight1.draw(ftransform, sproj, l, fs);		// draw joint (between left arms)
	// Right arm2
	GsMat rArm2Pos;
	rArm2Pos.translation(-(_radius + 0.025 + 0.1 + 0.05), 0.6, 0);
	ftransform = stransf*translationMatrix*rArm2Pos*armRot;
	_armRight2.draw(ftransform, sproj, l, fs);		// draw joint (between left arms)
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
		const float rate = 0.001f;
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

void SoObject::move(bool& animate, bool& front)
{
	if (animate == true)
	{
		rotateL1by = -rotateL1by;
		rotateL2by = -rotateL2by;
		rotateA1by = -rotateA1by;
		rotateA2by = -rotateA2by;
		GsMat temp;
		temp.roty(rotateby);
		GsVec newVec;
		newVec = temp * forwardVec;
		moveXby += (front == true) ? newVec.x : -newVec.x;
		moveZby += (front == true) ? newVec.z : -newVec.z;
		animate = false;
	}
}

void SoObject::getXYZ(float x, float y, float z)
{
	x = moveXby;
	y = moveYby;
	z = moveZby;
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

void SoObject::rotateHCW()				// -5 degrees
{
	const float rad = 5 * (M_PI / 180);
	rotateHby -= rad;
}

void SoObject::rotateHCCW()				// +5 degrees
{
	const float rad = 5 * (M_PI / 180);
	rotateHby += rad;
}

void SoObject::rotateL1CW()
{
	const float rad = 5 * (M_PI / 180);
	rotateL1by -= rad;
}

void SoObject::rotateL1CCW()
{
	const float rad = 5 * (M_PI / 180);
	rotateL1by += rad;
}

void SoObject::rotateL2CW()
{
	const float rad = 5 * (M_PI / 180);
	rotateL2by -= rad;
}

void SoObject::rotateL2CCW()
{
	const float rad = 5 * (M_PI / 180);
	rotateL2by += rad;
}

void SoObject::rotateA1CW()
{
	const float rad = 5 * (M_PI / 180);
	rotateA1by -= rad;
}

void SoObject::rotateA1CCW()
{
	const float rad = 5 * (M_PI / 180);
	rotateA1by += rad;
}

void SoObject::rotateA2CW()
{
	const float rad = 5 * (M_PI / 180);
	rotateA2by -= rad;
}

void SoObject::rotateA2CCW()
{
	const float rad = 5 * (M_PI / 180);
	rotateA2by += rad;
}

