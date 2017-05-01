
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
	_radiusB = 0.5f;
	_radiusT = 0.5f;

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

	// "sculpt parts"
	// body
	float m1[16] = {
		0.1, 0, 0, 0,
		0, 0.125, 0, 0.45,
		0, 0, 0.1, 0,
		0, 0, 0, 1
	};
	mBody = GsMat(m1);

	// joint between body and head
	float mj1[16] = {
		0.1, 0, 0, 0,
		0, 0.01, 0, 0.585,
		0, 0, 0.1, 0,
		0, 0, 0, 1
	};
	mJoint1 = GsMat(mj1);

	// joint between body and leg1
	float mj2[16] = {
		0.01, 0, 0, 0.06,
		0, 0.025, 0, 0.35,
		0, 0, 0.025, 0,
		0, 0, 0, 1
	};
	mJoint2 = GsMat(mj2);

	// joint between body and leg2
	float mj3[16] = {
		0.01, 0, 0, -0.06,
		0, 0.025, 0, 0.35,
		0, 0, 0.025, 0,
		0, 0, 0, 1
	};
	mJoint3 = GsMat(mj3);

	// joint between body and arm1
	float mj4[16] = {
		0.025, 0, 0, 0.062,
		0, 0.01, 0, 0.5,
		0, 0, 0.025, 0,
		0, 0, 0, 1
	};
	mJoint4 = GsMat(mj4);

	// joint between body and arm2
	float mj5[16] = {
		0.025, 0, 0, -0.062,
		0, 0.01, 0, 0.5,
		0, 0, 0.025, 0,
		0, 0, 0, 1
	};
	mJoint5 = GsMat(mj5);

	// head
	float m2[16] = {
		0.1, 0, 0, 0,
		0, 0.075, 0, 0.67,
		0, 0, 0.1, 0,
		0, 0, 0, 1
	};
	mHead = GsMat(m2);

	// leg1
	float m3[16] = {
		0.025, 0, 0, 0.0625,
		0, 0.175, 0, 0.175,
		0, 0, 0.025, 0,
		0, 0, 0, 1
	};
	mLeg1 = GsMat(m3);

	// leg2
	float m4[16] = {
		0.025, 0, 0, -0.0625,
		0, 0.175, 0, 0.175,
		0, 0, 0.025, 0,
		0, 0, 0, 1
	};
	mLeg2 = GsMat(m4);

	// arm1
	float m5[16] = {
		0.05, 0, 0, 0.075,
		0, 0.025, 0, 0.5,
		0, 0, 0.05, 0,
		0, 0, 0, 1
	};
	mArm1 = GsMat(m5);

	// arm2
	float m6[16] = {
		0.05, 0, 0, -0.075,
		0, 0.025, 0, 0.5,
		0, 0, 0.05, 0,
		0, 0, 0, 1
	};
	mArm2 = GsMat(m6);
}

void SoObject::init()
{
	_body.init();
	_joint1.init();
	_joint2.init();
	_joint3.init();
	_joint4.init();
	_joint5.init();
	_head.init();
	_leg1.init();
	_leg2.init();
	_arm1.init();
	_arm2.init();
}

void SoObject::build()
{
	_body.build(_nfaces, _radiusT, _radiusB);
	_joint1.build(_nfaces, _radiusT, _radiusB);
	_joint2.build(_nfaces, _radiusT, _radiusB);
	_joint3.build(_nfaces, _radiusT, _radiusB);
	_joint4.build(_nfaces, _radiusT, _radiusB);
	_joint5.build(_nfaces, _radiusT, _radiusB);
	_head.build(_nfaces, _radiusT, _radiusB);
	_leg1.build(_nfaces, _radiusT, _radiusB);
	_leg2.build(_nfaces, _radiusT, _radiusB);
	_arm1.build(_nfaces, _radiusT, _radiusB);
	_arm2.build(_nfaces, _radiusT, _radiusB);
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

void SoObject::draw(const GsMat& stransf, const GsMat& sproj, const GsLight& l, const float& fs, const GsVec lcoord)
{
	GsMat ftransform;									// final transform matrix
	
	// Whole object translation
	GsMat translationMatrix, temp1, temp2;
	temp1.translation(moveXby, moveYby, moveZby);		// rigid translation
	temp2.roty(rotateby);								// rotation matrix
	translationMatrix = temp1 * temp2;

	// Shadow matrix
	float cof = 0;
	float sha[16] = {
		1, -lcoord.x / lcoord.y, 0, lcoord.x * (cof) / lcoord.y,
		0, 0, 0, -(cof),
		0, -lcoord.z / lcoord.y, 1, lcoord.z * (cof) / lcoord.y,
		0, 0, 0, 1
	};
	GsMat shadow(sha);

	// Head:
	// Head rotation
	GsMat rotationH;
	rotationH.roty(rotateHby);
	ftransform = stransf*translationMatrix*mBody;
	// Drawing head
	ftransform = GsMat(stransf*translationMatrix*mHead*rotationH);
	_head.draw(ftransform, sproj, l, fs);			// draw head
	ftransform = GsMat(stransf*shadow*translationMatrix*mHead*rotationH);
	_head.draw(ftransform, sproj, l, fs);			// draw head shadow

	// Body:
	// Drawing body
	ftransform = stransf*translationMatrix*mBody;
	_body.draw(ftransform, sproj, l, fs);			// draw body
	ftransform = stransf*shadow*translationMatrix*mBody;
	_body.draw(ftransform, sproj, l, fs);			// draw body shadow

	// Legs:
	// Leg rotation
	GsMat rotationL1;
	rotationL1.rotx(rotateL2by);
	GsMat rotationL2;
	rotationL2.rotx(rotateL1by);
	// Drawing Legs
	GsMat rotateHelper;								// helps legs rotate
	rotateHelper.translation(0, 0.35, 0);
	// leg1
	ftransform = stransf*translationMatrix*rotateHelper*rotationL1*mLeg1;
	_leg1.draw(ftransform, sproj, l, fs);			// draw leg1
	ftransform = stransf*shadow*translationMatrix*rotateHelper*rotationL1*mLeg1;
	_leg1.draw(ftransform, sproj, l, fs);			// draw leg1 shadow
	// leg2
	ftransform = stransf*translationMatrix*rotateHelper*rotationL2*mLeg2;
	_leg2.draw(ftransform, sproj, l, fs);			// draw leg1
	ftransform = stransf*shadow*translationMatrix*rotateHelper*rotationL2*mLeg2;
	_leg2.draw(ftransform, sproj, l, fs);			// draw leg1
	
	// Arms:
	// Arm rotation
	GsMat rotationA1;
	rotationA1.roty(rotateA1by);
	GsMat rotationA2;
	rotationA2.roty(rotateA2by);
	GsMat armRot;									// rotation matrix (to position the arms)
	armRot.rotz(-90 * (M_PI / 180.0f));
	// Drawing Arms
	// arm1
	rotateHelper.translation(1, 0, 0);				// helps arms rotate
	ftransform = stransf*translationMatrix*mArm1*rotationA1*rotateHelper*armRot;
	_arm1.draw(ftransform, sproj, l, fs);			// draw arm2
	ftransform = stransf*shadow*translationMatrix*mArm1*rotationA1*rotateHelper*armRot;
	_arm1.draw(ftransform, sproj, l, fs);			// draw arm2 shadow
	// arm2
	ftransform = stransf*translationMatrix*mArm2*rotationA2*rotateHelper*armRot;
	_arm2.draw(ftransform, sproj, l, fs);			// draw arm2
	ftransform = stransf*shadow*translationMatrix*mArm2*rotationA2*rotateHelper*armRot;
	_arm2.draw(ftransform, sproj, l, fs);			// draw arm2 shadow

	// Joints:
	// Drawing joint1 (between head and body)
	ftransform = stransf*translationMatrix*mJoint1;
	_joint1.draw(ftransform, sproj, l, fs);			// draw joint (between head and body)
	ftransform = stransf*shadow*translationMatrix*mJoint1;
	_joint1.draw(ftransform, sproj, l, fs);			// draw joint (between head and body) shadow
	// Drawing joint2 (between body and leg1)
	ftransform = stransf*translationMatrix*mJoint2*armRot;
	_joint2.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg1)
	ftransform = stransf*shadow*translationMatrix*mJoint2*armRot;
	_joint2.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg1) shadow
	// Drawing joint3 (between body and leg2)
	ftransform = stransf*translationMatrix*mJoint3*armRot;
	_joint3.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2)
	ftransform = stransf*shadow*translationMatrix*mJoint3*armRot;
	_joint3.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2) shadow
	// Drawing joint4 (between body and leg2)
	ftransform = stransf*translationMatrix*mJoint4;
	_joint4.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2)
	ftransform = stransf*shadow*translationMatrix*mJoint4;
	_joint4.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2) shadow
	// Drawing joint5 (between body and leg2)
	ftransform = stransf*translationMatrix*mJoint5;
	_joint5.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2)
	ftransform = stransf*shadow*translationMatrix*mJoint5;
	_joint4.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2) shadow
}

