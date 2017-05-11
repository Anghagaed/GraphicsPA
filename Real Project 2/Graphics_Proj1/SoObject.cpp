
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
	moveOffset = 0.05f/80;
	forwardVec = GsVec(0,0,1)*moveOffset;

	// Drawing cylinder stuff
	_nfaces = 16;
	_radius = 0.25f/4;

	// Frame Counter for animation
	frameCounter1 = 0;
	frameCounter2 = 0;
	frameCounter3 = 0;
	frameCounter4 = 0;
	frameCounterMove = 0;

	// Jump animation stuff
	animateUp = false;
	animateDown = false;

	// Default object rotation state
	rotateby = 0;

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
	for (int i = 0; i < numsegs; ++i)
		P.push() = eval_bezier(i * 1.0f / numsegs, ctrlpnts);
}

void SoObject::initAnimation1() {
	// Frame 1 Rotate Arm and Leg
	{
		// Leg Right 
		{
			float dist;
			GsMat translate, rot, final;
			GsVec vec;
			dist = 0.30f;
			translate.translation(0.0f, -dist, 0.0f);
			rot.rotx(-(PI / 3));
			final = translate.inverse()*rot*translate;
			ani1Frame1.leg[2] = final;
			ani1Frame1.leg[3] = final;
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
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani1Frame1.arm[2] = final;
			ani1Frame1.arm[3] = final;
			ani1Frame1.armJoint[2] = final;
			ani1Frame1.armJoint[3] = final;
		}
	}
	//CurrentFrame.CopyFrom(ani1Frame1);
	ani1Frame2.CopyFrom(ani1Frame1);
	// Frame 2 Affects both arm and one leg right 
	{
		// Leg Right
		{
			float alegRadius = _radius / 6; float alegHeight = _radius * 2;
			float alegJointRadius = 0.025f / 2; float alegJointHeight = _radius / 3;

			GsMat translate, rot, final;
			GsVec vec;
			vec = ani1Frame1.leg[3] * iniPosLegR2*(_leg.center() + GsVec(0.0f, alegHeight / 2, 0.0f));
			//cout << vec << endl;
			translate.translation(-vec.x, -vec.y, -vec.z);
			rot.rotx(PI / 2);
			final = translate.inverse()*rot*translate;
			ani1Frame2.leg[3] = final*ani1Frame1.leg[3];
			//CurrentFrame.leg[3] = final*CurrentFrame.leg[3];
		}
		// Arm Left
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			vec = ani1Frame1.arm[1] * iniPosArmL2*(_arm.center() + GsVec(0, aHeight / 2, -aRadius));
			translate.translation(-vec.x, -vec.y, -vec.z);
			rot.roty(-PI / 3);
			//final = translate;
			final = translate.inverse()*rot*translate;
			ani1Frame2.arm[1] = final*ani1Frame1.arm[1];
			//CurrentFrame.arm[1] = final*ani1Frame1.arm[1];
		}
		// Arm Right
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			vec = ani1Frame1.arm[3] * iniPosArmR2*(_arm.center() + GsVec(0, -aHeight / 2, -aRadius));
			translate.translation(-vec.x, -vec.y, -vec.z);
			rot.roty(PI / 3);
			//final = translate;
			final = translate.inverse()*rot*translate;
			ani1Frame2.arm[3] = final*ani1Frame1.arm[3];
			//CurrentFrame.arm[3] = final*ani1Frame1.arm[3];
		}
	}
	//CurrentFrame.CopyFrom(ani1Frame2);
	ani1Frame3.CopyFrom(ani1Frame2);
	// Frame 3 Twirl
	{
		// done in actual animation
	}
	//CurrentFrame.CopyFrom(ani1Frame3);
	// Frame 4 Rotate Arm
}

void SoObject::initAnimation2() {
	// Frame 1 Rotate Body
	{
		float radius = _radius;
		float height = 0.05;
		GsMat translate, rot, final;
		GsVec vec;
		rot.rotz(-PI / 4);
		vec = iniPosBody*_body.center();
		translate.translation(-vec.x, -vec.y + 0.2, -vec.z);
		final = translate.inverse()*rot*translate;
		//final = translate;
		for (int i = 0; i < 4; ++i)
		{
			ani2Frame1.arm[i] = final;
			ani2Frame1.armJoint[i] = final;
		}
		ani2Frame1.body = final;
		ani2Frame1.head = final;
	}
	//ani2Frame1.Identity();
	// Leg Right 
	{
		float dist;
		GsMat translate, rot, final;
		GsVec vec;
		dist = 0.30f;
		translate.translation(0.0f, -dist, 0.0f);
		rot.rotz(-(PI / 4));
		final = translate.inverse()*rot*translate;
		ani2Frame1.leg[3] = final*ani2Frame1.leg[3];
		ani2Frame1.leg[2] = final*ani2Frame1.leg[2];
		ani2Frame1.legJoint[2] = final*ani2Frame1.legJoint[2];
		ani2Frame1.legJoint[3] = final*ani2Frame1.legJoint[3];
	}
	ani2Frame2.CopyFrom(ani2Frame1);
	// Frame2
	// Arm Left
	{
		float aRadius = _radius / 2.5;
		float aHeight = 0.05;
		GsMat translate, rot, final;
		GsVec vec;
		rot.rotz(PI);
		vec = ani2Frame2.armJoint[0]*iniPosArmLJ1*(_armJoint.center() - GsVec(-aRadius, aHeight / 2, 0));
		//cout << vec << endl;
		translate.translation(-vec.x, -vec.y, -vec.z);
		final = translate.inverse()*rot*translate;
		ani2Frame2.arm[0] = final*ani2Frame2.arm[0];
		ani2Frame2.arm[1] = final*ani2Frame2.arm[1];
		//ani2Frame1.armJoint[0] = final*ani2Frame1.armJoint[0];
		ani2Frame2.armJoint[1] = final*ani2Frame2.armJoint[1];
	}
	// Arm Right
	{
		float aRadius = _radius / 2.5;
		float aHeight = 0.05;
		GsMat translate, rot, final;
		GsVec vec;
		rot.rotz(-PI);
		vec = ani2Frame2.armJoint[2]*iniPosArmRJ1*(_armJoint.center() - GsVec(aRadius, aHeight / 2, 0));
		//cout << vec << endl;
		translate.translation(-vec.x, -vec.y, -vec.z);
		final = translate.inverse()*rot*translate;
		//final = translate.inverse()*rot*translate;
		ani2Frame2.arm[2] = final*ani2Frame2.arm[2];
		ani2Frame2.arm[3] = final*ani2Frame2.arm[3];
		//ani2Frame1.armJoint[2] = final*ani2Frame1.armJoint[2];
		ani2Frame2.armJoint[3] = final*ani2Frame2.armJoint[3];
	}
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
		//cout << vec << endl;
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
		//cout << vec << endl;
		translate.translation(-vec.x, -vec.y, -vec.z);
		final = translate.inverse()*rot*translate;
		ani3Frame1.arm[2] = final;
		ani3Frame1.arm[3] = final;
		ani3Frame1.armJoint[2] = final;
		ani3Frame1.armJoint[3] = final;
	}
	//
	ani3Frame2.CopyFrom(ani3Frame1);
	// Frame 2 Affects both arm
	// 2nd Arm Left
	{
		float aRadius = _radius / 2.5;
		float aHeight = 0.05;
		GsMat translate, rot, final;
		GsVec vec;
		rot.rotx(-PI / 3);
		vec =  ani3Frame1.arm[1]*iniPosArmL2 * (_arm.center() + GsVec(-aRadius, aHeight / 2, 0));
		//cout << vec << endl;
		translate.translation(-vec.x, -vec.y, -vec.z);
		final = translate.inverse()*rot*translate;
		ani3Frame2.arm[1] = final*ani3Frame1.arm[1];
	}
	// 2nd Arm Right
	{
		float aRadius = _radius / 2.5;
		float aHeight = 0.05;
		GsMat translate, rot, final;
		GsVec vec;
		rot.rotx(-PI / 8);
		vec = ani3Frame1.arm[3]*iniPosArmR2 * (_arm.center() - GsVec(aRadius, aHeight / 2, 0));
		//cout << vec << endl;
		translate.translation(-vec.x, -vec.y, -vec.z);
		final = translate.inverse()*rot*translate;
		ani3Frame2.arm[3] = final*ani3Frame1.arm[3];	
	}
	// Frame 3 Affects both arm
	ani3Frame3.CopyFrom(ani3Frame1);
	// 2nd Arm Left
	{
		float aRadius = _radius / 2.5;
		float aHeight = 0.05;
		GsMat translate, rot, final;
		GsVec vec;
		rot.rotx(-PI / 8);
		vec = ani3Frame1.arm[1] * iniPosArmL2 * (_arm.center() + GsVec(-aRadius, aHeight / 2, 0));
		//cout << vec << endl;
		translate.translation(-vec.x, -vec.y, -vec.z);
		final = translate.inverse()*rot*translate;
		ani3Frame3.arm[1] = final*ani3Frame1.arm[1];
	}
	// 2nd Arm Right
	{
		float aRadius = _radius / 2.5;
		float aHeight = 0.05;
		GsMat translate, rot, final;
		GsVec vec;
		rot.rotx(-PI / 3);
		vec = ani3Frame1.arm[3] * iniPosArmR2 * (_arm.center() - GsVec(aRadius, aHeight / 2, 0));
		//cout << vec << endl;
		translate.translation(-vec.x, -vec.y, -vec.z);
		final = translate.inverse()*rot*translate;
		ani3Frame3.arm[3] = final*ani3Frame1.arm[3];
	}
	// Frame 3 Twirl 180 degrees
	// pretty sure I can copy Hang's 360 degrees part
	//CurrentFrame.CopyFrom(ani3Frame2);
}

void SoObject::initAnimation4()
{
	//GsMat recoveryRot1, recoveryRot2;
	//recoveryRot2.rotx(PI / 2); recoveryRot1.rotx(-PI / 2);
	{
		// Frame1
		// rotate body, head, leg and arms (x-axis)
		// Frame 1 Rotate Arm and Leg
		// Body
		{
			float radius = _radius;
			float height = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-PI / 4);
			vec = iniPosBody*_body.center();
			translate.translation(-vec.x, -vec.y+0.2, -vec.z);
			final = translate.inverse()*rot*translate;
			//final = translate;
			for (int i = 0; i < 4; ++i)
			{
				ani4Frame1.arm[i] = final;
				ani4Frame1.armJoint[i] = final;
			}
			ani4Frame1.body = final;
			ani4Frame1.head = final;
		}
		// Leg Right 
		{
			float dist;
			GsMat translate, rot, final;
			GsVec vec;
			dist = 0.30f;
			translate.translation(0.0f, -dist, 0.0f);
			rot.rotx(-(2*PI / 3));
			final = translate.inverse()*rot*translate;
			ani4Frame1.leg[3] = final;
			ani4Frame1.leg[2] = final;
			ani4Frame1.legJoint[2] = final;
			ani4Frame1.legJoint[3] = final;
		}
		ani4Frame2.CopyFrom(ani4Frame1);
		// Frame2
		// rotate arms (x-axis)
		// Frame 1 Rotate Arm and Leg
		// Arm Left
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-2*PI / 3);
			vec = ani4Frame1.arm[0]*iniPosArmLJ1*(_armJoint.center() + GsVec(-aRadius, aHeight / 2, 0));
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani4Frame2.arm[0] = final*ani4Frame1.arm[0];
			ani4Frame2.arm[1] = final*ani4Frame1.arm[0];
			ani4Frame2.armJoint[0] = final*ani4Frame1.armJoint[0];
			ani4Frame2.armJoint[1] = final*ani4Frame1.armJoint[1];
		}
		// Arm Right
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-2*PI / 3);
			vec = ani4Frame1.arm[2] * iniPosArmRJ1*(_armJoint.center() + GsVec(aRadius, aHeight / 2, 0));
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani4Frame2.arm[2] = final*ani4Frame1.arm[2];
			ani4Frame2.arm[3] = final*ani4Frame1.arm[3];
			ani4Frame2.armJoint[2] = final*ani4Frame1.armJoint[2];
			ani4Frame2.armJoint[3] = final*ani4Frame1.armJoint[3];
		}
		// 2nd Arm Right
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-PI / 3);
			vec = ani4Frame2.arm[3] * iniPosArmR2*(_armJoint.center() - GsVec(aRadius, aHeight / 2, 0));
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani4Frame2.arm[3] = final*ani4Frame2.arm[3];
		}
		// 2nd Arm Left
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-PI / 3);
			vec = ani4Frame2.arm[1] * iniPosArmL2*(_armJoint.center() + GsVec(-aRadius, aHeight / 2, 0));
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani4Frame2.arm[1] = final*ani4Frame2.arm[1];
		}
		// 2nd Leg Right 
		{
			float alegRadius = _radius / 6; float alegHeight = _radius * 2;
			float alegJointRadius = 0.025f / 2; float alegJointHeight = _radius / 3;

			GsMat translate, rot, final;
			GsVec vec;
			vec = ani4Frame2.leg[3] * iniPosLegR2*(_leg.center() + GsVec(0.0f, alegHeight / 2, 0.0f));
			//cout << vec << endl;
			translate.translation(-vec.x, -vec.y, -vec.z);
			rot.rotx(PI / 2);
			final = translate.inverse()*rot*translate;
			ani4Frame2.leg[3] = final*ani4Frame2.leg[3];
			//CurrentFrame.leg[3] = final*CurrentFrame.leg[3];
		}
		//CurrentFrame.CopyFrom(ani4Frame2);
	}
}

void SoObject::initWalkAnimation() {
	// Define a translation matrix
	GsMat moveUnit, translCount;
	int stepCounter = 1;
	//moveUnit.translation(0.0125f, 0.0f, 0.0125f);
	moveUnit.translation(0.05f, 0.0f, 0.05f);
	float Q = PI / 2;
	// translCount always stepCounter * moveUnit
	// Left Leg
	// Frame 1
	{	
		float dist = 0.30f;
		GsMat translate1, rot1, final1;
		GsMat translate2, rot2, final2;
		GsVec vec;
		rot1.rotx(-Q);
		rot2.rotx(Q);

		translate1.translation(0.0f, -dist, 0.0f);
		final1 = translate1.inverse()*rot1*translate1;
		
		vec = final1 * iniPosLegLJ2 * _jointLeg.center();
		translate2.translation(-vec.x, -vec.y, -vec.z);
		final2 = translate2.inverse()*rot2*translate2;

		
		walkFrame1.leg[0] = final1;
		walkFrame1.legJoint[0] = final1;
		walkFrame1.legJoint[1] = final1;
		walkFrame1.leg[1] = final2 * final1;
	}
	// Frame 2 Everything except non-active leg is translated as we undo Frame 1
	Q = PI / 2;
	walkFrame2.CopyFrom(walkFrame1);
	{
		// Everything except non-active leg is moved up
		// Undo Frame 1
		float dist = 0.30f;
		GsMat translate1, rot1, final1;
		GsMat translate2, rot2, final2;
		GsVec vec;
		rot1.rotx(Q);
		rot2.rotx(-Q);

		translate1.translation(0.0f, -dist, 0.0f);
		final1 = translate1.inverse()*rot1*translate1;

		vec =  final1 * walkFrame1.legJoint[1]* iniPosLegLJ2 * _jointLeg.center();
		translate2.translation(-vec.x, -vec.y, -vec.z);
		final2 = translate2.inverse()*rot2*translate2;

		//moveUnit.identity();

		walkFrame2.leg[0] = moveUnit * final1 * walkFrame1.leg[0];
		walkFrame2.legJoint[0] = moveUnit * final1 * walkFrame1.legJoint[0];
		walkFrame2.legJoint[1] = moveUnit * final1 * walkFrame1.legJoint[1];
		walkFrame2.leg[1] = moveUnit * final2 * final1 * walkFrame1.leg[1];

		walkFrame2.body = moveUnit * walkFrame1.body;
		walkFrame2.head = moveUnit * walkFrame1.head;
		for (int i = 0; i < 4; ++i) {
			walkFrame2.arm[i] = moveUnit * walkFrame1.arm[i];
			walkFrame2.armJoint[i] = moveUnit * walkFrame1.armJoint[i];
		}

	}
	walkFrame3.CopyFrom(walkFrame2);
	// Frame 3 Move the Other Leg (Right)
	{
		float dist = 0.30f;
		GsMat translate1, rot1, final1;
		GsMat translate2, rot2, final2;
		GsVec vec;
		rot1.rotx(-Q);
		rot2.rotx(Q);

		translate1.translation(0.0f, -dist, 0.0f);
		final1 = translate1.inverse()*rot1*translate1;

		vec = final1 * iniPosLegRJ2 * _jointLeg.center();
		translate2.translation(-vec.x, -vec.y, -vec.z);
		final2 = translate2.inverse()*rot2*translate2;

		
		walkFrame3.leg[2] = moveUnit*moveUnit*final1;
		walkFrame3.legJoint[2] = moveUnit*moveUnit*final1;
		walkFrame3.legJoint[3] = moveUnit*moveUnit*final1;
		walkFrame3.leg[3] = moveUnit*moveUnit * final2 * final1;

		walkFrame3.body = moveUnit * walkFrame2.body;
		walkFrame3.head = moveUnit * walkFrame2.head;
		for (int i = 0; i < 4; ++i) {
			walkFrame3.arm[i] = moveUnit * walkFrame2.arm[i];
			walkFrame3.armJoint[i] = moveUnit * walkFrame2.armJoint[i];
		}
	}
	walkFrame4.CopyFrom(walkFrame3);
	Q = PI / 2;
	// Frame 4 Undo Frame 3 Leg movement but move left leg up
	{
		float alegRadius = _radius / 6; float alegHeight = _radius * 2;
		float alegJointRadius = 0.025f / 2; float alegJointHeight = _radius / 3;
		float dist = 0.30f;
		GsMat translate1, rot1, final1;
		GsMat translate2, rot2, final2;
		GsMat offset;
		//offset.translation(0, -0.005, 0);
		GsVec vec;
		rot1.rotx(Q);
		rot2.rotx(-Q);

		vec = walkFrame3.legJoint[2] * iniPosLegRJ1 * (_jointLeg.center() + GsVec(0, -alegRadius, 0));
		translate1.translation(-vec.x, -vec.y, -vec.z);
		final1 = translate1.inverse()*rot1*translate1;
		//final1 = translate1;

		vec = final1 * walkFrame3.legJoint[3] * iniPosLegRJ2 * _jointLeg.center();
		translate2.translation(-vec.x, -vec.y, -vec.z);
		final2 = translate2.inverse()*rot2*translate2;


		walkFrame4.leg[2] = final1 * walkFrame3.leg[2];
		walkFrame4.legJoint[2] = final1 * walkFrame3.legJoint[2];
		walkFrame4.legJoint[3] = final1 * walkFrame3.legJoint[3];
		walkFrame4.leg[3] = final2 * final1 * walkFrame3.leg[3];
		
		walkFrame4.leg[0] = moveUnit * walkFrame3.leg[0];
		walkFrame4.leg[1] = moveUnit * walkFrame3.leg[1];
		walkFrame4.legJoint[0] = moveUnit * walkFrame3.legJoint[0];
		walkFrame4.legJoint[1] = moveUnit * walkFrame3.legJoint[1];
		
	}
	//CurrentFrame.CopyFrom(walkFrame1);
	//CurrentFrame.CopyFrom(walkFrame2);
	//CurrentFrame.CopyFrom(walkFrame3);
	//CurrentFrame.CopyFrom(walkFrame4);
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


	// Head 
	temp1.translation(0.0f, 0.8f, 0.0f);
	temp2.roty(M_PI / 2);
	iniPosHead = temp1 * temp2;

	// Body
	temp1.translation(0.0f, 0.5f, 0.0f);
	iniPosBody = temp1;

	// Setting up initFrame
	initFrame.arm[0] = iniPosArmL1;
	initFrame.arm[1] = iniPosArmL2;
	initFrame.arm[2] = iniPosArmR1;
	initFrame.arm[3] = iniPosArmR2;

	initFrame.armJoint[0] = iniPosArmLJ1;
	initFrame.armJoint[1] = iniPosArmLJ2;
	initFrame.armJoint[2] = iniPosArmRJ1;
	initFrame.armJoint[3] = iniPosArmRJ2;

	initFrame.leg[0] = iniPosLegL1;
	initFrame.leg[1] = iniPosLegL2;
	initFrame.leg[2] = iniPosLegR1;
	initFrame.leg[3] = iniPosLegR2;

	initFrame.legJoint[0] = iniPosLegLJ1;
	initFrame.legJoint[1] = iniPosLegLJ2;
	initFrame.legJoint[2] = iniPosLegRJ1;
	initFrame.legJoint[3] = iniPosLegLJ2;

	initFrame.head = iniPosHead;
	initFrame.body = iniPosBody;


	/*initAnimation1();
	initAnimation3();
	initWalkAnimation();*/
}


void SoObject::reset()
{
	//CurrentFrame.CopyFrom(initFrame);
	CurrentFrame.Identity();
}

// Return 0 if animation is done 1 if animation is still going on
int SoObject::animationOne() 
{
	if (frameCounter1 < 40) 
	{
		// Frame 1
		//cout << "Frame1" << "\n";
		frameCounter1++;
		//CurrentFrame.CopyFrom(ani1Frame1);
		// Leg Right 
		{
			float dist;
			GsMat translate, rot, final;
			GsVec vec;
			dist = 0.30f;
			translate.translation(0.0f, -dist, 0.0f);
			rot.rotx(- frameCounter1*(PI / 3.0) / 40.0);
			final = translate.inverse()*rot*translate;
			ani1Frame1.leg[2] = final;
			ani1Frame1.leg[3] = final;
			ani1Frame1.legJoint[2] = final;
			ani1Frame1.legJoint[3] = final;
		}
		// Arm Left
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-frameCounter1*(PI / 2.0) / 40.0);
			vec = iniPosArmLJ1*(_armJoint.center() + GsVec(-aRadius, aHeight / 2, 0));
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
			rot.rotx(-frameCounter1*(PI / 2.0)/40.0);
			vec = iniPosArmRJ1*(_armJoint.center() + GsVec(aRadius, aHeight / 2, 0));
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani1Frame1.arm[2] = final;
			ani1Frame1.arm[3] = final;
			ani1Frame1.armJoint[2] = final;
			ani1Frame1.armJoint[3] = final;
		}
		CurrentFrame.CopyFrom(ani1Frame1);
		return 1;
	}
	else if (frameCounter1 <= 80) 
	{
		// Frame 2
		//cout << "Frame2" << "\n";
		frameCounter1++;
		//CurrentFrame.CopyFrom(ani1Frame2);
		ani1Frame2.CopyFrom(ani1Frame1);
		// Frame 2 Affects both arm and one leg right 
		int tempCounter = frameCounter1 - 40;
		// Leg Right
		{
			float alegRadius = _radius / 6; float alegHeight = _radius * 2;
			float alegJointRadius = 0.025f / 2; float alegJointHeight = _radius / 3;

			GsMat translate, rot, final;
			GsVec vec;
			vec = ani1Frame1.leg[3] * iniPosLegR2*(_leg.center() + GsVec(0.0f, alegHeight / 2, 0.0f));
			//cout << vec << endl;
			translate.translation(-vec.x, -vec.y, -vec.z);
			rot.rotx(tempCounter*(PI / 2.0)/40.0);
			final = translate.inverse()*rot*translate;
			ani1Frame2.leg[3] = final*ani1Frame1.leg[3];
			//CurrentFrame.leg[3] = final*CurrentFrame.leg[3];
		}
		// Arm Left
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			vec = ani1Frame1.arm[1] * iniPosArmL2*(_arm.center() + GsVec(0, aHeight / 2, -aRadius));
			translate.translation(-vec.x, -vec.y, -vec.z);
			rot.roty(-tempCounter*(PI / 3.0)/40.0);
			//final = translate;
			final = translate.inverse()*rot*translate;
			ani1Frame2.arm[1] = final*ani1Frame1.arm[1];
			//CurrentFrame.arm[1] = final*ani1Frame1.arm[1];
		}
		// Arm Right
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			vec = ani1Frame1.arm[3] * iniPosArmR2*(_arm.center() + GsVec(0, -aHeight / 2, -aRadius));
			translate.translation(-vec.x, -vec.y, -vec.z);
			rot.roty(tempCounter*(PI / 3.0)/40.0);
			//final = translate;
			final = translate.inverse()*rot*translate;
			ani1Frame2.arm[3] = final*ani1Frame1.arm[3];
			//CurrentFrame.arm[3] = final*ani1Frame1.arm[3];
		}
		CurrentFrame.CopyFrom(ani1Frame2);
		return 1;
	}
	else if (frameCounter1 <= 120) {
		// Frame 3
		//cout << "Frame3" << "\n";
		frameCounter1++;
		// THIS IS WHERE THE TWIRL HAPPENS
		rotateby = (frameCounter1-80)* 2 * PI / 40;
		return 1;
	}
	else if (frameCounter1 > 120) {
		//cout << "End animation" << "\n";
		rotateby = 0;	// 2 PI = 0
		frameCounter1 = 0;
		return 0;
	}

}

int SoObject::animationTwo() 
{
	if (frameCounter2 <= 40) 
	{
		// Frame 1
		frameCounter2++;
		ani2Frame1.Identity();
		//cout << "Frame1" << "\n";
		{
			float radius = _radius;
			float height = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotz(-frameCounter2*(PI / (4.0*40.0)));
			vec = iniPosBody*_body.center();
			translate.translation(-vec.x, -vec.y + 0.2, -vec.z);
			final = translate.inverse()*rot*translate;
			//final = translate;
			for (int i = 0; i < 4; ++i)
			{
				ani2Frame1.arm[i] = final;
				ani2Frame1.armJoint[i] = final;
				//ani2Frame1.legJoint[i] = final;
			}
			//ani2Frame1.leg[2] = final;
			//ani2Frame1.leg[2] = final;
			//ani2Frame1.legJoint[0] = final;
			//ani2Frame1.legJoint[2] = final;
			ani2Frame1.body = final;
			ani2Frame1.head = final;
		}
		//ani2Frame1.Identity();
		// Leg Right 
		{
			float dist;
			GsMat translate, rot, final;
			GsVec vec;
			dist = 0.30f;
			translate.translation(0.0f, -dist, 0.0f);
			rot.rotz(-frameCounter2*(PI / 4.0)/40.0);
		//	cout << -frameCounter2*(PI / 4.0) / 40.0<< endl;
			final = translate.inverse()*rot*translate;
			ani2Frame1.leg[3] = final*ani2Frame1.leg[3];
			ani2Frame1.leg[2] = final*ani2Frame1.leg[2];
			ani2Frame1.legJoint[2] = final*ani2Frame1.legJoint[2];
			ani2Frame1.legJoint[3] = final*ani2Frame1.legJoint[3];
		}
		CurrentFrame.CopyFrom(ani2Frame1);
		return 1;
	}
	else if (frameCounter2 <= 80) 
	{
		// Frame 2
	//	cout << "Frame2" << "\n";
		frameCounter2++;
		ani2Frame2.CopyFrom(ani2Frame1);
		int tempCounter = frameCounter2 - 40;
		// Arm Left
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotz(tempCounter * PI / 40.0);
			vec = ani2Frame1.armJoint[0] * iniPosArmLJ1*(_armJoint.center() - GsVec(-aRadius, aHeight / 2, 0));
			//cout << vec << endl;
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani2Frame2.arm[0] = final*ani2Frame1.arm[0];
			ani2Frame2.arm[1] = final*ani2Frame1.arm[1];
			//ani2Frame1.armJoint[0] = final*ani2Frame1.armJoint[0];
			ani2Frame2.armJoint[1] = final*ani2Frame1.armJoint[1];
		}
		// Arm Right
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotz(-tempCounter*PI/40.0);
			vec = ani2Frame2.armJoint[2] * iniPosArmRJ1*(_armJoint.center() - GsVec(aRadius, aHeight / 2, 0));
			//cout << vec << endl;
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			//final = translate.inverse()*rot*translate;
			ani2Frame2.arm[2] = final*ani2Frame2.arm[2];
			ani2Frame2.arm[3] = final*ani2Frame2.arm[3];
			//ani2Frame1.armJoint[2] = final*ani2Frame1.armJoint[2];
			ani2Frame2.armJoint[3] = final*ani2Frame2.armJoint[3];
		}
		CurrentFrame.CopyFrom(ani2Frame2);
		return 1;
	}
	else if (frameCounter2 <= 120) {
		// rotation
		frameCounter2++;
		rotateby = (frameCounter2 - 120)*2* PI / 40.0;
		CurrentFrame.CopyFrom(ani2Frame2);
		return 1;
	}
	else if (frameCounter2 > 120) {
		// Frame 1
		rotateby = 0;
		frameCounter2 = 0;
		return 0;
	}
}

int SoObject::animationThree()
{
	if (frameCounter3 < 40) 
	{
		// Frame 1
		frameCounter3++;
		// Frame 1 Rotate Arm
		// Arm Left
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-frameCounter3*(PI / 2.0)/40.0);
			vec = iniPosArmLJ1*(_armJoint.center() + GsVec(-aRadius, aHeight / 2, 0));
			//cout << vec << endl;
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
			rot.rotx(-frameCounter3*(PI / 2.0)/40.0);
			vec = iniPosArmRJ1*(_armJoint.center() + GsVec(aRadius, aHeight / 2, 0));
			//cout << vec << endl;
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani3Frame1.arm[2] = final;
			ani3Frame1.arm[3] = final;
			ani3Frame1.armJoint[2] = final;
			ani3Frame1.armJoint[3] = final;
		}
		CurrentFrame.CopyFrom(ani3Frame1);
		return 1;
	}
	else if (frameCounter3 <= 80) 
	{
		// Frame 2
		frameCounter3++;
		int tempCounter = frameCounter3 - 40;
		ani3Frame2.CopyFrom(ani3Frame1);
		// 2nd Arm Left
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-tempCounter*(PI / 3.0)/40.0);
			vec = ani3Frame1.arm[1] * iniPosArmL2 * (_arm.center() + GsVec(-aRadius, aHeight / 2, 0));
			//cout << vec << endl;
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani3Frame2.arm[1] = final*ani3Frame1.arm[1];
		}
		// 2nd Arm Right
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-tempCounter*(PI / 8.0)/40.0);
			vec = ani3Frame1.arm[3] * iniPosArmR2 * (_arm.center() - GsVec(aRadius, aHeight / 2, 0));
			//cout << vec << endl;
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani3Frame2.arm[3] = final*ani3Frame1.arm[3];
		}
		CurrentFrame.CopyFrom(ani3Frame2);
		return 1;
	}
	else if (frameCounter3 <= 120) 
	{
		// Frame 3
		frameCounter3++;
		int tempCounter = frameCounter3 - 80;
		ani3Frame3.CopyFrom(ani3Frame1);
		// Frame 3 Affects both arm
		// 2nd Arm Left
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-tempCounter*(PI / 8.0)/40.0);
			vec = ani3Frame1.arm[1] * iniPosArmL2 * (_arm.center() + GsVec(-aRadius, aHeight / 2, 0));
			//cout << vec << endl;
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani3Frame3.arm[1] = final*ani3Frame1.arm[1];
		}
		// 2nd Arm Right
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-tempCounter*(PI / 3.0)/40.0);
			vec = ani3Frame1.arm[3] * iniPosArmR2 * (_arm.center() - GsVec(aRadius, aHeight / 2, 0));
			//cout << vec << endl;
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani3Frame3.arm[3] = final*ani3Frame1.arm[3];
		}
		CurrentFrame.CopyFrom(ani3Frame3);
		return 1;
	}
	else if (frameCounter3 <= 160) 
	{
		frameCounter3 ++;
		rotateby = (frameCounter3 - 120)*2*PI/40.0;
		return 1;
	}
	else if (frameCounter3 > 160)
	{
		frameCounter3 = 0;
		rotateby = 0;
		return 0;
	}
}

int SoObject::animationFour()
{
	if (frameCounter4 <= 40) {
		// Frame 1
		frameCounter4++;
		// Frame1
		// rotate body, head, leg and arms (x-axis)
		// Frame 1 Rotate Arm and Leg
		// Body
		{
			float radius = _radius;
			float height = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-frameCounter4*(PI / 4.0)/40.0);
			vec = iniPosBody*_body.center();
			translate.translation(-vec.x, -vec.y + 0.2, -vec.z);
			final = translate.inverse()*rot*translate;
			//final = translate;
			for (int i = 0; i < 4; ++i)
			{
				ani4Frame1.arm[i] = final;
				ani4Frame1.armJoint[i] = final;
			}
			ani4Frame1.body = final;
			ani4Frame1.head = final;
		}
		// Leg Right 
		{
			float dist;
			GsMat translate, rot, final;
			GsVec vec;
			dist = 0.30f;
			translate.translation(0.0f, -dist, 0.0f);
			rot.rotx(-frameCounter4*(2 * PI / 3)/40.0);
			final = translate.inverse()*rot*translate;
			ani4Frame1.leg[3] = final;
			ani4Frame1.leg[2] = final;
			ani4Frame1.legJoint[2] = final;
			ani4Frame1.legJoint[3] = final;
		}
		CurrentFrame.CopyFrom(ani4Frame1);
		ani4Frame2.CopyFrom(ani4Frame1);
		return 1;
	}
	if (frameCounter4 < 80) {
		// Frame 2
		ani4Frame2.CopyFrom(ani4Frame1);
		frameCounter4++;
		// Frame2
		// rotate arms (x-axis)
		// Frame 1 Rotate Arm and Leg
		// Arm Left
		int tempCount = frameCounter4 - 40;
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-tempCount*(2 * PI / 3.0)/40.0);
			vec = ani4Frame1.arm[0] * iniPosArmLJ1*(_armJoint.center() + GsVec(-aRadius, aHeight / 2, 0));
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani4Frame2.arm[0] = final*ani4Frame1.arm[0];
			ani4Frame2.arm[1] = final*ani4Frame1.arm[0];
			ani4Frame2.armJoint[0] = final*ani4Frame1.armJoint[0];
			ani4Frame2.armJoint[1] = final*ani4Frame1.armJoint[1];
		}
		// Arm Right
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-tempCount*(2 * PI / 3.0)/40.0);
			vec = ani4Frame1.arm[2] * iniPosArmRJ1*(_armJoint.center() + GsVec(aRadius, aHeight / 2, 0));
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani4Frame2.arm[2] = final*ani4Frame1.arm[2];
			ani4Frame2.arm[3] = final*ani4Frame1.arm[3];
			ani4Frame2.armJoint[2] = final*ani4Frame1.armJoint[2];
			ani4Frame2.armJoint[3] = final*ani4Frame1.armJoint[3];
		}
		// 2nd Arm Right
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-tempCount*(PI / 3.0)/40.0);
			vec = ani4Frame2.arm[3] * iniPosArmR2*(_armJoint.center() - GsVec(aRadius, aHeight / 2, 0));
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani4Frame2.arm[3] = final*ani4Frame2.arm[3];
		}
		// 2nd Arm Left
		{
			float aRadius = _radius / 2.5;
			float aHeight = 0.05;
			GsMat translate, rot, final;
			GsVec vec;
			rot.rotx(-tempCount*(PI / 3.0)/40.0);
			vec = ani4Frame2.arm[1] * iniPosArmL2*(_armJoint.center() + GsVec(-aRadius, aHeight / 2, 0));
			translate.translation(-vec.x, -vec.y, -vec.z);
			final = translate.inverse()*rot*translate;
			ani4Frame2.arm[1] = final*ani4Frame2.arm[1];
		}
		// 2nd Leg Right 
		{
			float alegRadius = _radius / 6; float alegHeight = _radius * 2;
			float alegJointRadius = 0.025f / 2; float alegJointHeight = _radius / 3;
			//cout << tempCount << endl;
			GsMat translate, rot, final;
			GsVec vec;
			vec = ani4Frame2.leg[3] * iniPosLegR2*(_leg.center() + GsVec(0.0f, alegHeight / 2, 0.0f));
			//cout << vec << endl;
			translate.translation(-vec.x, -vec.y, -vec.z);
			rot.rotx(tempCount*PI / 2.0/ 40.0);
			final = translate.inverse()*rot*translate;
			ani4Frame2.leg[3] = final*ani4Frame2.leg[3];
			//CurrentFrame.leg[3] = final*CurrentFrame.leg[3];
		}
		CurrentFrame.CopyFrom(ani4Frame2);
		return 1;
	}
	if (frameCounter4 <= 120) {
		// Frame 3
		rotateby = (frameCounter4-80) *2* PI / 40.0;
		frameCounter4++;
		return 1;
	}
	else if (frameCounter4 > 120) {
		frameCounter4 = 0;
		rotateby = 0;
		return 0;
	}
}
int SoObject::animateMove()
{
	// Define a translation matrix
	GsMat moveUnit, translCount;
	int stepCounter = 1;
	//moveUnit.translation(0.0125f, 0.0f, 0.0125f);
	//moveUnit.translation(0.05f, 0.0f, 0.05f);
	moveUnit.translation(0.0, 0.0, 0.025/80.0);
	float Q = PI / 2;
	if (frameCounterMove <= 20)
	{
		// frame1
		frameCounterMove++;
		{
			float dist = 0.30f;
			GsMat translate1, rot1, final1;
			GsMat translate2, rot2, final2;
			GsVec vec;
			rot1.rotx(-Q);
			rot2.rotx(Q);

			translate1.translation(0.0f, -dist, 0.0f);
			final1 = translate1.inverse()*rot1*translate1;

			vec = final1 * iniPosLegLJ2 * _jointLeg.center();
			translate2.translation(-vec.x, -vec.y, -vec.z);
			final2 = translate2.inverse()*rot2*translate2;


			walkFrame1.leg[0] = final1;
			walkFrame1.legJoint[0] = final1;
			walkFrame1.legJoint[1] = final1;
			walkFrame1.leg[1] = final2 * final1;
		}
		CurrentFrame.CopyFrom(walkFrame1);
		return 1;
	}
	else if (frameCounterMove <= 40)
	{
		// frame2
		frameCounterMove++;
		Q = PI / 2;
		walkFrame2.CopyFrom(walkFrame1);
		{
			// Everything except non-active leg is moved up
			// Undo Frame 1
			float dist = 0.30f;
			GsMat translate1, rot1, final1;
			GsMat translate2, rot2, final2;
			GsVec vec;
			rot1.rotx(Q);
			rot2.rotx(-Q);

			translate1.translation(0.0f, -dist, 0.0f);
			final1 = translate1.inverse()*rot1*translate1;

			vec = final1 * walkFrame1.legJoint[1] * iniPosLegLJ2 * _jointLeg.center();
			translate2.translation(-vec.x, -vec.y, -vec.z);
			final2 = translate2.inverse()*rot2*translate2;

			//moveUnit.identity();

			walkFrame2.leg[0] = moveUnit * final1 * walkFrame1.leg[0];
			walkFrame2.legJoint[0] = moveUnit * final1 * walkFrame1.legJoint[0];
			walkFrame2.legJoint[1] = moveUnit * final1 * walkFrame1.legJoint[1];
			walkFrame2.leg[1] = moveUnit * final2 * final1 * walkFrame1.leg[1];

			walkFrame2.body = moveUnit * walkFrame1.body;
			walkFrame2.head = moveUnit * walkFrame1.head;
			for (int i = 0; i < 4; ++i) {
				walkFrame2.arm[i] = moveUnit * walkFrame1.arm[i];
				walkFrame2.armJoint[i] = moveUnit * walkFrame1.armJoint[i];
			}
		}
		CurrentFrame.CopyFrom(walkFrame2);
		return 1;
	}
	else if (frameCounterMove <= 60)
	{
		// frame3
		walkFrame3.CopyFrom(walkFrame2);
		frameCounterMove++;
		// Frame 3 Move the Other Leg (Right)
		{
			float dist = 0.30f;
			GsMat translate1, rot1, final1;
			GsMat translate2, rot2, final2;
			GsVec vec;
			rot1.rotx(-Q);
			rot2.rotx(Q);

			translate1.translation(0.0f, -dist, 0.0f);
			final1 = translate1.inverse()*rot1*translate1;

			vec = final1 * iniPosLegRJ2 * _jointLeg.center();
			translate2.translation(-vec.x, -vec.y, -vec.z);
			final2 = translate2.inverse()*rot2*translate2;


			walkFrame3.leg[2] = moveUnit*moveUnit*final1;
			walkFrame3.legJoint[2] = moveUnit*moveUnit*final1;
			walkFrame3.legJoint[3] = moveUnit*moveUnit*final1;
			walkFrame3.leg[3] = moveUnit*moveUnit * final2 * final1;

			walkFrame3.body = moveUnit * walkFrame2.body;
			walkFrame3.head = moveUnit * walkFrame2.head;
			for (int i = 0; i < 4; ++i) {
				walkFrame3.arm[i] = moveUnit * walkFrame2.arm[i];
				walkFrame3.armJoint[i] = moveUnit * walkFrame2.armJoint[i];
			}
		}
		CurrentFrame.CopyFrom(walkFrame3);
		return 1;
	}
	else if (frameCounterMove <= 80)
	{
		// frame4
		walkFrame4.CopyFrom(walkFrame3);
		Q = PI / 2;
		frameCounterMove++;
		// Frame 4 Undo Frame 3 Leg movement but move left leg up
		{
			float alegRadius = _radius / 6; float alegHeight = _radius * 2;
			float alegJointRadius = 0.025f / 2; float alegJointHeight = _radius / 3;
			float dist = 0.30f;
			GsMat translate1, rot1, final1;
			GsMat translate2, rot2, final2;
			GsMat offset;
			//offset.translation(0, -0.005, 0);
			GsVec vec;
			rot1.rotx(Q);
			rot2.rotx(-Q);

			vec = walkFrame3.legJoint[2] * iniPosLegRJ1 * (_jointLeg.center() + GsVec(0, -alegRadius, 0));
			translate1.translation(-vec.x, -vec.y, -vec.z);
			final1 = translate1.inverse()*rot1*translate1;
			//final1 = translate1;

			vec = final1 * walkFrame3.legJoint[3] * iniPosLegRJ2 * _jointLeg.center();
			translate2.translation(-vec.x, -vec.y, -vec.z);
			final2 = translate2.inverse()*rot2*translate2;


			walkFrame4.leg[2] = final1 * walkFrame3.leg[2];
			walkFrame4.legJoint[2] = final1 * walkFrame3.legJoint[2];
			walkFrame4.legJoint[3] = final1 * walkFrame3.legJoint[3];
			walkFrame4.leg[3] = final2 * final1 * walkFrame3.leg[3];

			walkFrame4.leg[0] = moveUnit * walkFrame3.leg[0];
			walkFrame4.leg[1] = moveUnit * walkFrame3.leg[1];
			walkFrame4.legJoint[0] = moveUnit * walkFrame3.legJoint[0];
			walkFrame4.legJoint[1] = moveUnit * walkFrame3.legJoint[1];
		}
		CurrentFrame.CopyFrom(walkFrame4);
		return 1;
	}
	else if (frameCounterMove > 80)
	{
		frameCounterMove = 0;
		return 0;
	}
}
void SoObject::draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs, const GsVec lcoord)
{
	GsVec a = forwardVec;
	GsVec b; double tempAngle = 0;
	if (stop == false)
	{
		if (frame_num != numsegs)
			b = P[frame_num++];
		else
		{
			frame_num = 0;
			b = P[frame_num];
		}
		a.normalize();
		b.normalize();
		tempAngle = angle(a, b);
	}
	// Whole object translation
	GsMat temp1, temp2;
	
	temp1.translation(P[frame_num]);									// rigid translation
	temp2.roty(rotateby+tempAngle);										// rotation matrix
	translationMatrix = temp1 * temp2;

	// Calculate the difference

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
	_head.draw(ftransform*CurrentFrame.head*iniPosHead, pr, l, fs);					// draw head
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
	_body.draw(ftransform*CurrentFrame.body*iniPosBody, pr, l, fs);				// draw body
}

//void SoObject::turn(bool & animate, bool & left)
//{
//	if (animate == true)
//	{
//		if (left == true)
//			rotateby += M_PI / 4;
//		else
//			rotateby -= M_PI / 4;
//		animate = false;
//	}
//}
