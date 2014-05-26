#include "CPixel.h"

#ifndef CAMERA_H
#define CAMERA_H

class Ccamera
{
private:
	Vertex eye;
	Vertex center;
	Vertex up;

	float fovy;
	float aspect;
	float zNeer;
	float zFar;


public:
	Ccamera(){
		fovy=65.0;	  aspect=1.0f;	  zNeer=0.1f;	  zFar=1000.f;
		eye.x=0.0f;	eye.y=0.0f;	eye.z=300.0f;
		center.x =400.0f;	center.y = 400.0f;	center.z = 000.0f;
		up.x = 1.0f;	up.y = 1.0f;	up.z = 1.0f;
	}

	~Ccamera(){}

	void setfovy(float f){fovy = f;}
	void setaspect(float f){aspect = f;}
	void setzFar(float f){zFar = f;}
	void setzNeer(float f){zNeer = f;}

	void setEye(float X, float Y, float Z){	eye.x=X;	eye.y=Y;	eye.z=Z; }
	void setEye(const Vertex& c){	eye.x = c.x;	eye.y = c.y;	eye.z = c.z; }

	void setCenter(float X, float Y, float Z){	center.x = X;	center.y = Y;	center.z = Z;}
	void setCenter(const Vertex& c){	center.x = c.x;	center.y = c.y;	center.z = c.z;}

	void setUp(float X, float Y, float Z){	up.x = X;	up.y = Y;	up.z = Z;}
	void setUp(const Vertex& c){	up.x = c.x;	up.y = c.y;	up.z = c.z;}


	float getfovy(){	return fovy;}
	float getaspect(){	return aspect;}
	float getzFar(){	return zFar;}
	float getzNeer(){	return zNeer;}

	Vertex& getEye(){	return eye;}
	Vertex& getCenter(){	return center;}
	Vertex& getUp(){	return up;}
};

#endif
