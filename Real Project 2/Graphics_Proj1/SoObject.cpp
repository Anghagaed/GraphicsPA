
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
	_radius = 0.25f;

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
}

void SoObject::init()
{
	_body.init(0,0.50,0,"../metaltexture.jpg");
}

void SoObject::build()
{
	_body.build(_radius/4, 0.5, _nfaces);
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

