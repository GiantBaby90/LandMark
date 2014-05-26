#include "gdal_priv.h"
#include <GL\freeglut.h>
#include "CFrustum.h"
#include "CMipMap.h"
#include <iostream>


////

CMipMap *map;

Ccamera camera;
CFrustum pFrustum;

int nXSize;
int nYSize;
int nBands;
unsigned int texture_list[10];

//motion
int prev_x,prev_y;
int curr_x,curr_y;
int prev_left=0;

float rotx=0.0f,roty=0.0f,rotz=0.0f;

void wheel(int wheel, int direction, int x, int y){
	//std::cout << "wheel:  " << wheel << std::endl;
	//std::cout << "direction: " << direction << std::endl;

	Vertex e,c;


	e = camera.getEye();
	c = camera.getCenter();

	if (direction < 0) //zoom in
	{
		const double ratio = 0.9;

		e.z *= ratio;
		//	c.x *= ratio;
		//	c.y *= ratio;

	}
	else if(direction > 0)//zomm out
	{
		const double ratio = 1.1;

		e.z *= ratio;
		//	c.x *= ratio;
		//	c.y *= ratio;
	}

	camera.setEye(e);
	camera.setCenter(c);

	//std::cout << "center x y z: " << camera.getCenter().x <<" "<< camera.getCenter().y <<" " << camera.getCenter().z << std::endl;
	//std::cout << "eye x y z: " << camera.getEye().x <<" "<< camera.getEye().y <<" " << camera.getEye().z << std::endl;

	glutPostRedisplay();
}
void keyboard(unsigned char key,int x,int y)
{
	Vertex e,u,c;

	e = camera.getEye();
	u = camera.getUp();
	c = camera.getCenter();

	switch(key){
	case 'a':
	case 'A':
		e.x -= 10.0f;
		e.y += 10.0f;
		c.x -= 10.0f;
		c.y += 10.0f;
		break;
	case 's':
	case 'S':
		e.x -= 10.0f;
		e.y -= 10.0f;
		c.x -= 10.0f;
		c.y -= 10.0f;
		break;
	case 'd':
	case 'D':
		e.x += 10.0f;
		e.y -= 10.0f;
		c.x += 10.0f;
		c.y -= 10.0f;
		break;
	case 'w':
	case 'W':
		e.x += 10.0f;
		e.y += 10.0f;
		c.x += 10.0f;
		c.y += 10.0f;
		break;
	case 'q':
	case 'Q':
		c.x -= 3.0f;
		c.y += 3.0f;
		//	u.y += 1.01f;
		u.z += 0.03f;
		break;
	case 'e':
	case 'E':
		c.x += 3.0f;
		c.y -= 3.0f;
		//	u.x -= 0.01f;
		//	u.y -= 1.01f;
		u.z -= 0.03f;
		break;
	case 27:
		exit(0);
	}

	camera.setEye(e);
	camera.setCenter(c);
	camera.setUp(u);

	//std::cout << "center x y z: " << camera.getCenter().x <<" "<< camera.getCenter().y <<" " << camera.getCenter().z << std::endl;
	//std::cout << "eye x y z: " << camera.getEye().x <<" "<< camera.getEye().y <<" " << camera.getEye().z << std::endl;
	//std::cout << "up x y z: " << camera.getUp().x <<" "<< camera.getUp().y <<" " << camera.getUp().z << std::endl;

	glutPostRedisplay();
}

void mouse(int button,int state,int x,int y){
	if(button==GLUT_LEFT_BUTTON && state == GLUT_DOWN) //button==GLUT_LEFT_BUTTON 이 조건이면 왼쪽버튼이 땔때, 눌릴때 2번 들어감,state == GLUT_DOWN 추가
	{

		//std::cout << "GLUT_LEFT_BUTTON  " << std::endl;
		prev_x=curr_x=x;
		prev_y=curr_y=y;
	}

}

void motion(int x,int y)
{
	double modelview_mat[16];
	double proj_mat[16];
	int viewport[4];
	double rprev_x, rprev_y, rprev_z;
	double rcurr_x, rcurr_y, rcurr_z;

	glPushMatrix();
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview_mat);
	glGetDoublev(GL_PROJECTION_MATRIX, proj_mat);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glPopMatrix();
	// winX and winY is given from the function parameters.


	gluUnProject(prev_x, prev_y, 0,
		modelview_mat, proj_mat, viewport,
		&rprev_x, &rprev_y, &rprev_z);


	std::cout << "x: " << x <<" y: " << y << std::endl;
	curr_x=x;
	curr_y=y;

	gluUnProject(curr_x, curr_y, 0,
		modelview_mat, proj_mat, viewport,
		&rcurr_x, &rcurr_y, &rcurr_z);



	//calc camera movement from(curr - prev)
	double disp_x=(rcurr_x - rprev_x);
	double disp_y=(rcurr_y - rprev_y);
	//double ratio_x=abs(rcurr_x - rprev_x);
	//double ratio_y=abs(rcurr_y - rprev_y);


	/*
	if(disp_x > 0){
	eyex -=0.003;//*ratio_x;
	centerx -=0.003;//*ratio_x;
	}
	else if(disp_x < 0)
	{
	eyex +=0.003;//*ratio_x;
	centerx +=0.003;//*ratio_x;
	}*/
	//eyex -=(rcurr_x - rprev_x);
	//centerx -=(rcurr_x - rprev_x);


	/*
	if(disp_y > 0)
	{
	eyey +=0.003;//*ratio_y;
	centery +=0.003;//*ratio_y;
	}
	else if(disp_y < 0)
	{
	eyey -=0.003;//*ratio_y;
	centery -=0.003;//*ratio_y;
	}*/
	//eyey +=(rcurr_y - rprev_y);
	//centery +=(rcurr_y - rprev_y);

	//update prev
	prev_x=curr_x;
	prev_y=curr_y;

	glutPostRedisplay();

}

void mydisplay()
{
	Vertex eye = camera.getEye();
	Vertex center = camera.getCenter();

	Point p = map->refresh(Point(eye.x, eye.y));

	int level = map->getCurrentLevel();

	//if((HEIGHT >> level) > eye.z) map->levelUp(Point(eye.x, eye.y));
	//if((HEIGHT >> (level-1)) < eye.z) map->levelDown(Point(eye.x, eye.y));

	eye.x += p.x;
	center.x += p.x;

	eye.y += p.y;
	center.y += p.y;	

	camera.setEye(eye);
	camera.setCenter(center);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera.getEye().x, camera.getEye().y, camera.getEye().z,
		camera.getCenter().x, camera.getCenter().y, camera.getCenter().z,
		camera.getUp().x, camera.getUp().z, camera.getUp().z);

	pFrustum.ExtractFrustum();

	
	map->GenerateIndex(pFrustum, 0.1);
	map->Draw();

	glutSwapBuffers();
}

void initialize(Ccamera camera)
{

	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	/*
	//지역변수 설정 LIGHT 생성(LIGHT0)
	GLfloat light_amient[]={0.0f,0.0f,0.0f,1.0f};//La
	GLfloat light_diffuse[]={1.0f,1.0f,1.0f,1.0f};//Ld
	GLfloat light_specular[]={1.0f,1.0f,1.0f,1.0f};//Ls
	GLfloat light_position[]={1.0f,1.0f,1.0f,0.0f};//위에서 만든 빛의 위치
	//그래픽 카드에 넘겨주기 (LIGHT0)
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_amient);//fv에서 v는 인자를 포인터로 주겠다는것
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glEnable(GL_LIGHTING);//lighting 계산을 하겠다.
	glEnable(GL_LIGHT0);//light0을 켠다
	//K 설정
	//표면의 material정보
	//RGB 가이니고 RGB를 얼마나 반사하는가의 값.
	GLfloat mat_ambient[] = {0.8f, 0.8f, 0.2f, 1.0f}; //Ka 
	GLfloat mat_diffuse[] = {0.1f, 0.5f, 0.8f, 1.0f}; //Kd
	GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};//Ks //RBG가 들어오는 그대로 뿜어낸다. 
	GLfloat mat_shininess = 50.0f;//반짝거리는 정도 alpha값
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);//이함수가 한번호출된다음에는 무조건 이재질
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);//다른 재질은 다시 함수 호출 
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
	*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera.getfovy(), camera.getaspect(), camera.getzNeer(), camera.getzFar());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera.getEye().x, camera.getEye().y, camera.getEye().z,
		camera.getCenter().x, camera.getCenter().y, camera.getCenter().z,
		camera.getUp().x, camera.getUp().z, camera.getUp().z);

	map = new CMipMap();
}

int main()
{
	pFrustum.setCamera(&camera);

	int argc;
	char *argv[1] = {"my_program"};
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0,0);
	glutCreateWindow("simple");

	glutMouseWheelFunc(wheel);
	glutMouseFunc(mouse);
//	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
		
	initialize(camera);

	glutDisplayFunc(mydisplay);

	glutMainLoop();

}