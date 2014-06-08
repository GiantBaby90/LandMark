package com.example.mapview;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView.Renderer;
import android.opengl.GLU;

public class MyRenderer implements Renderer {
	private Context context = null;

	private float red = 1.0f;
	private float green = 1.0f;
	private float blue = 1.0f;

	private TextureCube cube = null;
	private Map map = null;

	float angleX = 0;
	float angleY = 0;
	float speedX = 0;
	float speedY = 0;
	float z = -6.0f;

	float angle = 0.0f;
	float speed = 0.5f;

	public MyRenderer(Context context) {
		this.context = context;

		// cube = new TextureCube();
		map = new Map(this.context);

		MainConst.camera = new Camera();
		MainConst.qt = new ZQuadTree(MainConst.nXSize, MainConst.nYSize);
		MainConst.qt.Build();
		MainConst.pFrustum = new Frustum();
		MainConst.pFrustum.setCamera(MainConst.camera);

	}

	public void setColor(float r, float g, float b) {
		red = r;
		green = g;
		blue = b;
	}

	@Override
	public void onDrawFrame(GL10 gl) {
		gl.glClearColor(red, green, blue, 1.0f);
		gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);

		gl.glDisable(GL10.GL_LIGHTING);

		gl.glDisable(GL10.GL_BLEND);
		gl.glEnable(GL10.GL_DEPTH_TEST);

		gl.glLoadIdentity();
		gl.glTranslatef(0, 0, z);
		gl.glRotatef(angleX, 1.0f, 0.0f, 0.0f);
		gl.glRotatef(angleY, 0.0f, 1.0f, 0.0f);

		// square.draw(gl);
		// cube.draw(gl);
		map.draw(gl);
		//
		angleX += speedX;
		angleY += speedY;
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		gl.glViewport(0, 0, width, height);

		gl.glMatrixMode(GL10.GL_PROJECTION);
		gl.glLoadIdentity();

		GLU.gluPerspective(gl, MainConst.camera.getfovy(),
				MainConst.camera.getaspect(), MainConst.camera.getzNeer(),
				MainConst.camera.getzFar());
		// GLU.gluPerspective(gl, 45.0f, (float)width/(float)height, 0.1f,
		// 100.0f);

		gl.glMatrixMode(GL10.GL_MODELVIEW);
		gl.glLoadIdentity();

		GLU.gluLookAt(gl, MainConst.camera.getEye().x,
				MainConst.camera.getEye().y, MainConst.camera.getEye().z,
				MainConst.camera.getCenter().x, MainConst.camera.getCenter().y,
				MainConst.camera.getCenter().z, MainConst.camera.getUp().x,
				MainConst.camera.getUp().z, MainConst.camera.getUp().z);
	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		gl.glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		gl.glEnable(GL10.GL_DEPTH_TEST);
		gl.glEnable(GL10.GL_CULL_FACE);
		gl.glFrontFace(GL10.GL_CCW);
		gl.glClearDepthf(1.0f);

		gl.glDepthFunc(GL10.GL_LEQUAL);
		gl.glHint(GL10.GL_PERSPECTIVE_CORRECTION_HINT, GL10.GL_NICEST);
		// gl.glShadeModel(GL10.GL_SMOOTH);
		gl.glDisable(GL10.GL_DITHER);

		// cube.loadTexture(gl,context);
		map.loadTexture(gl, context);

		gl.glEnable(GL10.GL_TEXTURE_2D);

		gl.glColor4f(1.0f, 1.0f, 1.0f, 0.0f);

	}
}
