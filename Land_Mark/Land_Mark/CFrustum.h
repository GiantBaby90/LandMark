#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include <GL\freeglut.h>
#include "CPixel.h"
#include "Ccamera.h"

class CFrustum
{
private:
	float **frustum;
	Ccamera *camera;

public:
	CFrustum();
	~CFrustum();

	void ExtractFrustum();
	void initialize(const Vertex& c);


	float** CFrustum::GetFrustum(){	return frustum; }

	bool PointInFrustum(float x, float y, float z);
	bool SphereInFrustum(float x, float y, float z, float radius);

	void setCamera(Ccamera* c) { camera = c;	}
	Vertex& GetCamera()	{ return camera->getEye();	}

};

#endif