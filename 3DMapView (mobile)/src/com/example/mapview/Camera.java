package com.example.mapview;

public class Camera {
	
	public Vertex eye = new Vertex();
	public Vertex center= new Vertex();
	public Vertex up= new Vertex();

	public float fovy;
	public float aspect;
	public float zNeer;
	public float zFar;


	public Camera(){
			fovy=45.0f;	  aspect=1.0f;	  zNeer=0.1f;	  zFar=1000.f;
			eye.x=0.0f;	eye.y=0.0f;	eye.z=300.0f;
			center.x =400.0f;	center.y = 400.0f;	center.z = 000.0f;
			up.x = 1.0f;	up.y = 1.0f;	up.z = 1.0f;
		}

	public	void setfovy(float f){fovy = f;}
	public	void setaspect(float f){aspect = f;}
	public	void setzFar(float f){zFar = f;}
	public	void setzNeer(float f){zNeer = f;}

	public	void setEye(float X, float Y, float Z){	eye.x=X;	eye.y=Y;	eye.z=Z; }
	public	void setEye(Vertex c){	eye.x = c.x;	eye.y = c.y;	eye.z = c.z; }

	public	void setCenter(float X, float Y, float Z){	center.x = X;	center.y = Y;	center.z = Z;}
	public	void setCenter(Vertex c){	center.x = c.x;	center.y = c.y;	center.z = c.z;}

	public	void setUp(float X, float Y, float Z){	up.x = X;	up.y = Y;	up.z = Z;}
	public	void setUp(Vertex c){	up.x = c.x;	up.y = c.y;	up.z = c.z;}


	public	float getfovy(){	return fovy;}
	public	float getaspect(){	return aspect;}
	public	float getzFar(){	return zFar;}
	public	float getzNeer(){	return zNeer;}

	public	Vertex getEye(){	return eye;}
	public	Vertex getCenter(){	return center;}
	public	Vertex getUp(){	return up;}
		
}
