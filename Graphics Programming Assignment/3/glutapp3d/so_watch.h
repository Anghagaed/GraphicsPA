/*
*	Author:		Hang V Liang
*	Purpose:	Codes for implementing a 3-d Stop Watch for Programming Assignment 3. CSE 170 Computer Graphics Class at UC Merced
*/

#ifndef SO_WATCH_H
#define SO_WATCH_H

#define PI 3.141592654

# include <vector>
# include <gsim/gs_color.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"
# include <math.h>

class SoWatch : public GlObjects {
private:
	std::vector<GsVec>   P; // coordinates for Lines
	//std::vector<GsVec>   T;	// coordinates for Triangles
	std::vector<GsColor> C; // colors
	int _numpoints;         // saves the number of points
	float x, y, z;			// Circular center point of the stopwatch
	float r;				// Radius from center point -> edge of stopwatch
	float mlen, slen;		// Length of the minute and second hand from center
	float mq, sq;			// Angle of the minute and second hand

	void buildCircle();
	void buildTicky();
	void buildHand();
public:
	SoWatch();
	void init(const GlProgram& prog);
	void build(float r, float mlen, float slen);

	void draw(GsMat& tr, GsMat& pr);

	
};

#endif // SO_WATCH_H

#ifdef WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif
double get_time()
{
	#ifdef WIN32
		// if better precision is needed in Windows, use QueryPerformanceCounter
		_timeb tp;
		_ftime_s(&tp);
		return 0.001*(double)tp.millitm + (double)tp.time;
	#else
		timeval tp;
		if (gettimeofday(&tp, 0) == -1) return 0;
		return 0.000001*(double)tp.tv_usec + (double)tp.tv_sec;
	#endif
}