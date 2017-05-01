
// Ensure the header file is included only once in multi-file projects
#ifndef SO_OBJECT
#define SO_OBJECT

// Include needed header files
# include "SoHead.h"
# include "SoLeg.h"
# include "SoArm.h"
# include "SoBody.h"
# include "SoJoint.h"
# include <gsim/gs_mat.h>
# include <gsim/gs_light.h>

// Defines:
/*
#define PIOVER2 M_PI / 2
#define PI M_PI
#define THREEPI 3 * M_PI / 2
#define TWOPI 2 * M_PI
#define NEGPIOVER2 -M_PI / 2
#define NEGPI -M_PI
#define NEGTHREEPI -3 * M_PI / 2
#define NEGTWOPI -2 * M_PI
*/

// Basically... This is the main object for this project!!
// Guess what? It jumps up and down with a press of a button!
// WOAH!! THAT IS SO COOL!!!
// Do you know what else it does? It can rotate, too!
// WOW!! THAT IS UNBELIEVABLE!!!
// But, wait a second. Do you know what's the coolest part?
// All parts are made out of the same textured cylinder!!!
// OMG!! THAT IS AMAZING!!! AUTOMATIC A, RIGHT???
class SoObject : public GlObjects
{
private:
	// Objects
	SoHead _head;
	SoBody _body;
	SoArm _armLeft1;
	SoArm _armLeft2;
	SoArm _armRight1;
	SoArm _armRight2;
	SoLeg _legLeft1;
	SoLeg _legLeft2;
	SoLeg _legRight1;
	SoLeg _legRight2;
	SoJoint _joint1;				// between head and body
	SoJoint _joint2;				// between body and leg1
	SoJoint _joint3;				// between body and leg2

	SoJoint _joint4;				// between body and arm1
	SoJoint _joint5;				// between body and arm2

	SoJoint _jointLegL1;			// between body and left Leg 1
	SoJoint _jointLegL2;			// between left leg 1 and left leg 2
	SoJoint _jointLegR1;			// between body and right leg 1
	SoJoint _jointLegR2;			// between right leg 1 and body

	SoJoint _jointLegL;
	SoJoint _jointLegR;

	SoJoint _armLeftJoint1;			// between body and arm1
	SoJoint _armLeftJoint2;			// between body and arm1
	SoJoint _armRightJoint1;		// between body and arm2
	SoJoint _armRightJoint2;		// between body and arm2
	
	// For building
	float _nfaces;						// 16 faces
	float _radius;
	
	// For drawing
	GsMat mBody;
	GsMat mHead;
	GsMat mArmLeft1;
	GsMat mArmLeft2;
	GsMat mArmRight1;
	GsMat mArmRight2;
	GsMat mArmRot;
	GsMat mLegLeft1;
	GsMat mLegLeft2;
	GsMat mLegRight1;
	GsMat mLegRight2;
	GsMat mJoint1;
	GsMat mJoint2;
	GsMat mJoint3;
	GsMat mJoint4;
	GsMat mJoint5;

	bool spinJointArmBool[4];		// 1 = left arm outer joint, 0 = left arm inner joint, 3 = right arm outer joint, 2 = right arm inner joint


	// For animation
	bool animateUp;
	bool animateDown;
	float moveYby;						// rigid movement (jump part)
	float moveZby;						// rigid movement (move part)
	float moveXby;						// rigid movement (move part)
	float rotateHby;					// global rotation angle (around y-axis)
	float rotateL1by;					// rotation angle for leg1
	float rotateL2by;					// rotation angle for leg2
	float rotateA1by;					// rotation angle for arm1
	float rotateA2by;					// rotation angle for arm2
	float rotateby;						// model rotation

	// foward vector (Doggo's)
	GsVec forwardVec;
	float moveOffset;

public:
	SoObject();
	void init();
	void build();
	void jump(bool& animate);					// check and animate (will be used in idle() function in app_window)
	void move(bool& animate, bool& front);		// check and animate (will be used in idle() function in app_window)
	void getXYZ(float x, float y, float z);
	void turn(bool& animate, bool& left);
	void keyFrame1();
	void rotateHCW();
	void rotateHCCW();
	void rotateL1CW();					// rotate the leg1 clockwise by a constant
	void rotateL1CCW();					// rotate the leg2 counterclockwise by a constant
	void rotateL2CW();
	void rotateL2CCW();
	void rotateA1CW();
	void rotateA1CCW();
	void rotateA2CW();
	void rotateA2CCW();
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs, const GsVec lcoord);
};

#endif // SO_OBJECT
