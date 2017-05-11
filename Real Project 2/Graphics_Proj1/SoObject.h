
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
using namespace std;
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
	SoArm _arm;
	SoJoint _armJoint;
	
	// Legs
	SoLeg _leg;
	SoJoint _jointLeg;
	
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
	float angleLeftJoint1;
	float angleLeftJoint2;

	// foward vector (Doggo's)
	GsVec forwardVec;
	float moveOffset;

	GsMat ftransform;						// final transform matrix
	GsMat translationMatrix;				// matrix for whole object translation

	// initial matrix
	GsMat iniPosArmL1, iniPosArmL2;			
	GsMat iniPosArmR1, iniPosArmR2;
	GsMat iniPosArmLJ1, iniPosArmLJ2;
	GsMat iniPosArmRJ1, iniPosArmRJ2;
	double armRadius, armHeight;

	GsMat iniPosLegL1, iniPosLegL2;
	GsMat iniPosLegR1, iniPosLegR2;
	GsMat iniPosLegLJ1, iniPosLegLJ2;
	GsMat iniPosLegRJ1, iniPosLegRJ2;

	// new matrix
	GsMat newPosArmL1, newPosArmL2;
	GsMat newPosArmR1, newPosArmR2;
	GsMat newPosArmLJ1, newPosArmLJ2;
	GsMat newPosArmRJ1, newPosArmRJ2;

	GsMat newPosLegL1, newPosLegL2;
	GsMat newPosLegR1, newPosLegR2;
	GsMat newPosLegLJ1, newPosLegLJ2;
	GsMat newPosLegRJ1, newPosLegRJ2;

	int frame;
	bool animate;

	// Curve
	int numsegs;
	GsArray<GsVec> ctrlpnts;				// control points
	GsArray<GsVec> P;						// coordinates

	
	double wholeArmRot = 0;

private:
	void drawHead(const GsMat& pr, const GsLight& l, const float& fs, const GsVec lcoord);
	void drawArms(const GsMat& pr, const GsLight& l, const float& fs, const GsVec lcoord);
	void drawLegs(const GsMat& pr, const GsLight& l, const float& fs, const GsVec lcoord);
	void drawBody(const GsMat& pr, const GsLight& l, const float& fs, const GsVec lcoord);
public:
	SoObject();
	void init();
	void build();
	void jump(bool& animate);				// check and animate (will be used in idle() function in app_window)
	void move(bool& animate);				// check and animate (will be used in idle() function in app_window)
	void turn(bool& animate, bool& left);
	void keyFrame1(bool &_animate);
	void draw(const GsMat& tr, const GsMat& pr, const GsLight& l, const float& fs, const GsVec lcoord);
};

#endif // SO_OBJECT
