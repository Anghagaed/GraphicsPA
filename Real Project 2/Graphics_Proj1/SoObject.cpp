
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
	_legLeft1.init(0.0f, 0.0f, 0.0f, "../metaltexture.jpg");
	_legRight1.init(0.0f, 0.0f, 0.0f, "../metaltexture.jpg");
	_jointLegL.init(0.0f, 0.0f, 0.0f, "../pinktexture.jpg");

	// left arm
	_armLeftJoint1.init(0.0f, 0.0f, 0.0f, "../pinktexture.jpg");
	_armLeftJoint2.init(0.0f, 0.0f, 0.0f, "../pinktexture.jpg");
	_armLeft1.init(0.0f, 0.0f, 0.0f, "../metaltexture.jpg");
	_armLeft2.init(0.0f, 0.0f, 0.0f, "../metaltexture.jpg");
	
	// right arm
	_armRightJoint1.init(0.0f, 0.0f, 0.0f, "../pinktexture.jpg");
	_armRightJoint2.init(0.0f, 0.0f, 0.0f, "../pinktexture.jpg");
	_armRight1.init(0.0f, 0.0f, 0.0f, "../metaltexture.jpg");
	_armRight2.init(0.0f, 0.0f, 0.0f, "../metaltexture.jpg");
}

void SoObject::build()
{
	// head
	_head.build(_radius, 0.2, _nfaces);

	// body
	_body.build(_radius, 0.4, _nfaces);

	// legs
	_jointLegL.build(0.025f/2, _radius / 3, _nfaces);
	_legLeft1.build(_radius / 6, 0.125f, _nfaces);
	_legRight1.build(_radius / 6, 0.125f, _nfaces);
	
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

void SoObject::keyFrame1(bool& animate)
{
	/*spinJointArmBool[0] = false;
	spinJointArmBool[1] = true;
	spinJointArmBool[2] = false;*/
//	spinJointArmBool[3] = true;
}

void SoObject::vturnLArm1(float amount)
{

}

void SoObject::hturnLArm1(float amount)
{

}

void SoObject::vturnLArm2(float amount)
{

}

void SoObject::hturnLArm2(float amount)
{

}

void SoObject::vturnRArm1(float amount)
{

}

void SoObject::hturnRArm1(float amount)
{

}

void SoObject::vturnRArm2(float amount)
{

}

void SoObject::hturnRArm2(float amount)
{

}

void SoObject::draw(const GsMat& stransf, const GsMat& sproj, const GsLight& l, const float& fs, const GsVec lcoord)
{
	GsMat ftransform;									// final transform matrix

	// Whole object translation
	GsMat translationMatrix, temp1, temp2;
	temp1.translation(moveXby, moveYby, moveZby);		// rigid translation
	temp2.roty(rotateby);								// rotation matrix
	translationMatrix = temp1 * temp2;

	// Head:
	// Drawing head
	ftransform = stransf*translationMatrix;
	temp1.translation(0.0f, 0.8f, 0.0f);
	_head.draw(ftransform * temp1, sproj, l, fs);				// draw head

	// Body:
	// Drawing body
	ftransform = stransf*translationMatrix;
	temp1.translation(0.0f, 0.5f, 0.0f);
	_body.draw(ftransform * temp1, sproj, l, fs);				// draw body
	
	// Drawing Right Leg
	// _jointLegL1
	temp1.rotx(PI / 2);
	temp2.translation(_radius / 4, 0.2875f, _radius/2);
	_jointLegL.draw(ftransform * temp2 * temp1, sproj, l, fs);

	// Drawing Left Leg 1
	temp1.translation(_radius / 4, 0.2125f, _radius / 2);
	_legLeft1.draw(ftransform * temp1, sproj, l, fs);

	// _jointLegL2
	temp1.rotx(PI / 2);
	temp2.translation(_radius / 4, 0.1375, _radius / 2);
	_jointLegL.draw(ftransform * temp2 * temp1, sproj, l, fs);

	// Drawing Left Leg 2
	temp1.translation(_radius / 4, 0.0625f, _radius / 2);
	_legLeft1.draw(ftransform * temp1, sproj, l, fs);

	// Drawing Right Leg
	// _jointLegR1
	temp1.rotx(PI / 2);
	temp2.translation(- _radius / 4, 0.2875f, - _radius / 2);
	_jointLegL.draw(ftransform * temp2 * temp1, sproj, l, fs);

	// Drawing Right Leg 1
	temp1.translation(- _radius / 4, 0.2125f, - _radius / 2);
	_legRight1.draw(ftransform * temp1, sproj, l, fs);

	// _jointLegR2
	temp1.rotx(PI / 2);
	temp2.translation(- _radius / 4, 0.1375, - _radius / 2);
	_jointLegL.draw(ftransform * temp2 * temp1, sproj, l, fs);

	// Drawing Right Leg 2
	temp1.translation(- _radius / 4, 0.0625f, - _radius / 2);
	_legRight1.draw(ftransform * temp1, sproj, l, fs);

	// Arms:
	GsMat armRot, spin;
	armRot.rotz(M_PI/2);
	// Drawing arms
	// Left arm joint1
	GsMat armLJoint1Pos;
	armLJoint1Pos.translation(_radius + 0.025, 0.6, 0);
	if (spinJointArmBool[0] == true) 
	{
		spin.rotz(PI / 2);
	}
	else 
	{
		spin.identity();
	}
	ftransform = stransf*translationMatrix*armLJoint1Pos*spin;
	_armLeftJoint1.draw(ftransform, sproj, l, fs);		// draw joint (between body and left arm)
	// Left arm joint2
	GsMat armLJoint2Pos;
	armLJoint2Pos.translation(_radius + 0.025 + 0.1, 0.6, 0);
	if (spinJointArmBool[1] == true) 
	{
		spin.roty(PI / 2);
	}
	else 
	{
		spin.identity();
	}
	ftransform = stransf*translationMatrix*armLJoint2Pos*spin;
	_armLeftJoint2.draw(ftransform, sproj, l, fs);		// draw joint (between left arms)
	// Left arm1
	GsMat lArm1Pos;
	lArm1Pos.translation(_radius + 0.075, 0.6, 0);
	ftransform = stransf*translationMatrix*lArm1Pos*armRot*spin;
	_armLeft1.draw(ftransform, sproj, l, fs);		// draw joint (between left arms)
	// Left arm2
	GsMat lArm2Pos, temp3;
	lArm2Pos.translation(_radius + 0.025 + 0.1 + 0.05, 0.6, 0);
	//temp2.identity();
	//temp3.identity();
	//temp2.rotz(PI / 2);
	//temp3.roty(-PI / 2);
	temp1 = armLJoint2Pos * spin * temp3 * armLJoint2Pos.inverse();
	ftransform = stransf*temp1*translationMatrix*lArm2Pos*armRot;
	_armLeft2.draw(ftransform, sproj, l, fs);		// draw joint (between left arms)

	// Right arm joint1
	GsMat armRJoint1Pos;
	armRJoint1Pos.translation(-(_radius + 0.025), 0.6, 0);
	if (spinJointArmBool[2] == true)
	{
		spin.rotx(PI / 2);
	}
	else 
	{
		spin.identity();
	}
	ftransform = stransf*translationMatrix*armRJoint1Pos*spin;
	_armLeftJoint1.draw(ftransform, sproj, l, fs);		// draw joint (between body and left arm)
	// Right arm joint2
	GsMat armRJoint2Pos;
	armRJoint2Pos.translation(-(_radius + 0.025 + 0.1), 0.6, 0);
	if (spinJointArmBool[3] == true) 
	{
		spin.rotx(PI / 2);
	}
	else 
	{
		spin.identity();
	}
	ftransform = stransf*translationMatrix*armRJoint2Pos*spin;
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
