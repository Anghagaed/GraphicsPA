# include "so_capsule.h"

SoCapsule::SoCapsule() {
	init();
}

void SoCapsule::init() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
	len = 0.0;
	rt = 0.0;
	rb = 0.0;
	_numfaces = 0;
}

void SoCapsule::build(float len, float rt, float rb, int nfaces) {
	this->len = len;
	this->rt = rt;
	this->rb = rb;
	this->_numfaces = nfaces;
}

void SoCapsule::draw() {

}

void SoCapsule::drawTop() {

}

