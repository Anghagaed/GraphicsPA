
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
	// Head
	SoHead _head;
	
	// Body
	SoBody _body;
	
	// Arms
	SoArm _armLeft1;
	SoArm _armLeft2;
	SoArm _armRight1;
	SoArm _armRight2;
	SoJoint _armLeftJoint1;					// between body and arm1
	SoJoint _armLeftJoint2;					// between body and arm1
	SoJoint _armRightJoint1;				// between body and arm2
	SoJoint _armRightJoint2;				// between body and arm2
	
	// Legs
	SoLeg _legLeft1;
	SoLeg _legLeft2;
	SoLeg _legRight1;
	SoLeg _legRight2;
	SoJoint _jointLegL1;					// between body and left Leg 1
	SoJoint _jointLegL2;					// between left leg 1 and left leg 2
	SoJoint _jointLegR1;					// between body and right leg 1
	SoJoint _jointLegR2;					// between right leg 1 and body
	SoJoint _jointLegL;
	SoJoint _jointLegR;
	
	// For building
	float _nfaces;							// 16 faces
	float _radius;
	
	// For drawing
	bool spinJointArmBool[4];				// 1 = left arm outer joint, 0 = left arm inner joint, 3 = right arm outer joint, 2 = right arm inner joint
	int frame_num;

	// For animation
	bool animateUp;
	bool animateDown;
	float moveYby;							// rigid movement (jump part)
	float moveZby;							// rigid movement (move part)
	float moveXby;							// rigid movement (move part)
	float rotateby;							// model rotation

	float angleLeftArm1;
	float angleLeftArm2;

	// foward vector (Doggo's)
	GsVec forwardVec;
	float moveOffset;

	// Curve
	int numsegs;
	GsArray<GsVec> ctrlpnts;				// control points
	GsArray<GsVec> P;						// coordinates

	GsMat vLArm1;							// handles vertical rotation for left arm1
	GsMat hLArm1;							// handles horizontal rotation for left arm1
	GsMat vLArm2;							// handles vertical rotation for left arm2
	GsMat hLArm2;							// handles horizontal rotation for left arm2

	GsMat vRArm1;							// handles vertical rotation for right arm1
	GsMat hRArm1;							// handles horizontal rotation for right arm1
	GsMat vRArm2;							// handles vertical rotation for right arm2
	GsMat hRArm2;							// handles horizontal rotation for right arm2

public:
	SoObject();
	void init();
	void build();
	void jump(bool& animate);				// check and animate (will be used in idle() function in app_window)
	void move(bool& animate);				// check and animate (will be used in idle() function in app_window)
	void turn(bool& animate, bool& left);
	void keyFrame1(bool& animate);

	// rotations
	// amount = radian
	void vturnLArm1(float amount);			// rotate left arm1 vertically
	void hturnLArm1(float amount);			// rotate left arm1 horizontally
	void vturnLArm2(float amount);			// rotate left arm2 vertically
	void hturnLArm2(float amount);			// rotate left arm2 horizontally
	
	void vturnRArm1(float amount);			// rotate right arm1 vertically
	void hturnRArm1(float amount);			// rotate right arm1 horizontally
	void vturnRArm2(float amount);			// rotate right arm2 vertically
	void hturnRArm2(float amount);			// rotate right arm2 horizontally
	
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs, const GsVec lcoord);
};

#endif // SO_OBJECT
