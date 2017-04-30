
# include "SoObject.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

SoObject::SoObject()
{
	PI = new float[9]{
		((float)M_PI/2), ((float)M_PI), (3* (float)M_PI/2), (2* (float)M_PI),
		(-(float)M_PI/2), (-(float)M_PI), (-3* (float)M_PI/2), (-2* (float)M_PI),
		(0.0f)
	};
	index = 8;
	_nfaces = 16;
	_radiusB = 0.5f;
	_radiusT = 0.5f;
	moveYby = 0.0f;
	moveZby = 0.0f;
	moveXby = 0.0f;
	rotateby = 0.0f;
	rotateL1by = 135.0f*(M_PI/180.0f);
	rotateL2by = -135.0f*(M_PI/180.0f);
	rotateA1by = -30.0f*(M_PI / 180.0f);
	rotateA2by = 150.0f*(M_PI/180.0f);
	rotateHby = 0.0f;
	animateUp = false;
	animateDown = false;

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
		const float rate = 0.025f;
		std::cout << rotateby << std::endl;
		std::cout << index << std::endl;
		if (index == 8 || index == 3 || index == 7)
			moveZby += (front == true) ? - rate : rate;
		else
		if (index == 0 || index == 6)
			moveXby += (front == true) ? - rate : rate;
		else
		if (index == 1 || index == 5)
			moveZby += (front == true) ? rate : - rate;
		else
		if (index == 2 || index == 4)
			moveXby += (front == true) ? rate : - rate;
		animate = false;
	}
}

int SoObject::find(float value)
{
	for (int i = 0; i < 9; ++i)
	{
		if (PI[i] == value)
			return i;
	}
	return -1;
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
		{
			if (index == 3)
			{
				rotateby = PI[0];
				index = 0;
			}
			else
				if (index == 8)
				{
					rotateby = PI[0];
					index = 0;
				}
				else
					if (index < 3)
						rotateby = PI[++index];
					else
						if (index == 4)
						{
							rotateby = PI[8];
							index = 8;
						}
						else
							if (index > 4 && index < 8)
								rotateby = PI[--index];
							else
								std::cout << index << std::endl;
		}
		else
		{
			if (index == 7)
			{
				rotateby = PI[4];
				index = 4;
			}
			else
				if (index == 8)
				{
					rotateby = PI[4];
					index = 4;
				}
				else
					if (index >= 4 && index < 7)
						rotateby = PI[++index];
					else
						if (index == 0)
						{
							rotateby = PI[8];
							index = 8;
						}
						else
							if (index > 0 && index <= 3)
								rotateby = PI[--index];
							else
								std::cout << index << std::endl;
		}
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
	// General matricies
	GsMat ftransform;	// final transform matrix
	// rigid move matrix
	float m[16] = {
		cos(rotateby), 0, sin(rotateby), moveXby,
		0, 1, 0, moveYby,
		-sin(rotateby), 0, cos(rotateby), moveZby,
		0, 0, 0, 1
	};
	GsMat move(m);

	// rotation matrix (for actual rotation)
	// y-axis rotation
	float m3[16] = {
		cos(rotateHby), 0, sin(rotateHby), 0,
		0, 1, 0, 0,
		-sin(rotateHby), 0, cos(rotateHby), 0,
		0, 0, 0, 1
	};
	float m6[16] = {
		cos(rotateA1by), 0, sin(rotateA1by), 0,
		0, 1, 0, 0,
		-sin(rotateA1by), 0, cos(rotateA1by), 0,
		0, 0, 0, 1
	};
	float m7[16] = {
		cos(rotateA2by), 0, sin(rotateA2by), 0,
		0, 1, 0, 0,
		-sin(rotateA2by), 0, cos(rotateA2by), 0,
		0, 0, 0, 1
	};
	float m8[16] = {
		cos(rotateby), 0, sin(rotateby), 0,
		0, 1, 0, 0,
		-sin(rotateby), 0, cos(rotateby), 0,
		0, 0, 0, 1
	};

	float cof = 0;
	float sha[16] = {
		1, -lcoord.x / lcoord.y, 0, lcoord.x * (cof) / lcoord.y,
		0, 0, 0, -(cof),
		0, -lcoord.z / lcoord.y, 1, lcoord.z * (cof) / lcoord.y,
		0, 0, 0, 1
	};

	GsMat shadow(sha);

	// z-axis
	/*float m4[16] = {
		cos(rotateL1by), -sin(rotateL1by), 0, 0,
		-sin(rotateL1by), cos(rotateL1by), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};*/
	// rotation matrix (to position the arms)
	float angle = 90 * (M_PI / 180.0f);				// angle for rotation (to position the arms)
	float m2[16] = {
		cos(angle), -sin(angle), 0, 0,
		-sin(angle), cos(angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	GsMat armRot(m2);
	
	// x-axis rotations
	float m4[16] = {
		1, 0, 0, 0,
		0, cos(rotateL1by), -sin(rotateL1by), 0,
		0, -sin(rotateL1by), cos(rotateL1by), 0,
		0, 0, 0, 1
	};
	float m5[16] = {
		1, 0, 0, 0,
		0, cos(rotateL2by), -sin(rotateL2by), 0,
		0, -sin(rotateL2by), cos(rotateL2by), 0,
		0, 0, 0, 1
	};
	
	GsMat rotationH(m3);
	GsMat rotationL1(m4);
	GsMat rotationL2(m5);
	GsMat rotationA1(m6);
	GsMat rotationA2(m7);

	// Drawing Part
	// body
	ftransform = stransf*move*mBody;
	_body.draw(ftransform, sproj, l, fs);			// draw body
	ftransform = stransf*shadow*move*mBody;
	_body.draw(ftransform, sproj, l, fs);			// draw body shadow

	// joint1
	ftransform = stransf*move*mJoint1;
	_joint1.draw(ftransform, sproj, l, fs);			// draw joint (between head and body)
	ftransform = stransf*shadow*move*mJoint1;
	_joint1.draw(ftransform, sproj, l, fs);			// draw joint (between head and body) shadow

	// joint2
	ftransform = stransf*move*mJoint2*armRot;
	_joint2.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg1)
	ftransform = stransf*shadow*move*mJoint2*armRot;
	_joint2.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg1) shadow

	// joint3
	ftransform = stransf*move*mJoint3*armRot;
	_joint3.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2)
	ftransform = stransf*shadow*move*mJoint3*armRot;
	_joint3.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2) shadow

	// joint4
	ftransform = stransf*move*mJoint4;
	_joint4.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2)
	ftransform = stransf*shadow*move*mJoint4;
	_joint4.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2) shadow

	// joint5
	ftransform = stransf*move*mJoint5;
	_joint5.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2)
	ftransform = stransf*shadow*move*mJoint5;
	_joint4.draw(ftransform, sproj, l, fs);			// draw joint (between body and leg2) shadow

	// head
	ftransform = GsMat(stransf*move*mHead*rotationH);
	_head.draw(ftransform, sproj, l, fs);			// draw head
	ftransform = GsMat(stransf*shadow*move*mHead*rotationH);
	_head.draw(ftransform, sproj, l, fs);			// draw head shadow

	// leg1
	float rotatehelperL[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0.35,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	GsMat rotateHelper(rotatehelperL);
	ftransform = stransf*move*rotateHelper*rotationL1*mLeg1;
	_leg1.draw(ftransform, sproj, l, fs);			// draw leg1
	ftransform = stransf*shadow*move*rotateHelper*rotationL1*mLeg1;
	_leg1.draw(ftransform, sproj, l, fs);			// draw leg1 shadow

	// leg2
	ftransform = stransf*move*rotateHelper*rotationL2*mLeg2;
	_leg2.draw(ftransform, sproj, l, fs);			// draw leg1
	ftransform = stransf*shadow*move*rotateHelper*rotationL2*mLeg2;
	_leg2.draw(ftransform, sproj, l, fs);			// draw leg1

	// arm1
	float rotatehelperA[16] = {
		1, 0, 0, 1,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	rotateHelper = GsMat(rotatehelperA);
	ftransform = stransf*move*mArm1*rotationA1*rotateHelper*armRot;
	_arm1.draw(ftransform, sproj, l, fs);			// draw arm2
	ftransform = stransf*shadow*move*mArm1*rotationA1*rotateHelper*armRot;
	_arm1.draw(ftransform, sproj, l, fs);			// draw arm2 shadow

	// arm2
	ftransform = stransf*move*mArm2*rotationA2*rotateHelper*armRot;	
	_arm2.draw(ftransform, sproj, l, fs);			// draw arm2
	ftransform = stransf*shadow*move*mArm2*rotationA2*rotateHelper*armRot;
	_arm2.draw(ftransform, sproj, l, fs);			// draw arm2 shadow
}

