
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
	SoArm _arm1;
	SoArm _arm2;
	SoLeg _leg1;
	SoLeg _leg2;
	SoJoint _joint1;				// between head and body
	SoJoint _joint2;				// between body and leg1
	SoJoint _joint3;				// between body and leg2
	SoJoint _joint4;				// between body and arm1
	SoJoint _joint5;				// between body and arm2
	
	// For building
	float _nfaces;						// 16 faces
	float _radiusB;						// 0.5 top radius
	float _radiusT;						// 0.5 bot radius
	
	// For drawing
	GsMat mBody;
	GsMat mHead;
	GsMat mArm1;
	GsMat mArm2;
	GsMat mArmRot;
	GsMat mLeg1;
	GsMat mLeg2;
	GsMat mJoint1;
	GsMat mJoint2;
	GsMat mJoint3;
	GsMat mJoint4;
	GsMat mJoint5;

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
	float* PI;
	int index;

public:
	SoObject();
	void init();
	void build();
	void jump(bool& animate);					// check and animate (will be used in idle() function in app_window)
	void move(bool& animate, bool& front);		// check and animate (will be used in idle() function in app_window)
	int find(float value);
	void getXYZ(float x, float y, float z);
	void turn(bool& animate, bool& left);
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