package com.example.mapview;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.util.Iterator;
import java.util.Vector;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.opengles.GL11;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;
import android.util.Log;

public class Map {
	private FloatBuffer vertexBuffer; // Buffer for vertex-array
	private FloatBuffer texBuffer; // Buffer for texture-coords-array (NEW)

	private float[] vertices = { // Vertices for a face
	-1.0f, -1.0f, 0.0f, // 0. left-bottom-front
			1.0f, -1.0f, 0.0f, // 1. right-bottom-front
			-1.0f, 1.0f, 0.0f, // 2. left-top-front
			1.0f, 1.0f, 0.0f // 3. right-top-front
	};

	float[] texCoords = { 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };

	float[] color = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 1.0f, };

	int[] textureIDs = new int[1];
	Bitmap bitmap = null;

	final static float DEPTH = 0.2f;

	public GeoMapData srcData;

	public Vector<Point> vArray = new Vector<Point>();
	public Vector<Float> vertexTemp = new Vector<Float>();

	public Map(Context context) {
		// FloatBuffer vertexBuffer = BufferUtil.makeFloatBuffer(vertices);
		FloatBuffer texBuffer = BufferUtil.makeFloatBuffer(texCoords);
		FloatBuffer colorBuffer = BufferUtil.makeFloatBuffer(color);

		InputStream istream = context.getResources().openRawResource(
				R.raw.test);
		try {
			// Read and decode input as bitmap
			bitmap = BitmapFactory.decodeStream(istream);

		} finally {
			try {
				MainConst.nXSize = bitmap.getWidth();
				MainConst.nYSize = bitmap.getHeight();
				MainConst.imageBuf = new byte[MainConst.nXSize
						* MainConst.nYSize * MainConst.nBands];
				ByteBuffer imageBuffer = ByteBuffer.wrap(MainConst.imageBuf);
				bitmap.copyPixelsToBuffer(imageBuffer);

				for (int i = 0; i < (MainConst.nXSize * MainConst.nYSize); ++i) {
					// BGR-565 to RGB-565
					byte v = MainConst.imageBuf[i];
					MainConst.imageBuf[i] = (byte) (((v & 0x1f) << 11)
							| (v & 0x7e0) | ((v & 0xf800) >> 11));
				}
				
				imageBuffer.rewind();

				istream.close();
			} catch (IOException e) {
			}
		}

	}

	public void draw(GL10 gl) {
		gl.glFrontFace(GL10.GL_CCW); // Front face in counter-clockwise
										// orientation
		gl.glEnable(GL10.GL_CULL_FACE); // Enable cull face
		// gl.glCullFace(GL10.GL_BACK); // Cull the back face (don't display)
		FloatBuffer colorBuffer = BufferUtil.makeFloatBuffer(color);

		MainConst.pFrustum.ExtractFrustum(gl);
		MainConst.qt.GenerateIndex(vArray, MainConst.pFrustum, 0.1f);

		for (int i = 0; i < vArray.size(); i++) {
			vertexTemp.add((float) vArray.elementAt(i).x);
			vertexTemp.add((float) vArray.elementAt(i).y);
			vertexTemp.add(MainConst.imageBuf[vArray.elementAt(i).y
					* MainConst.nXSize * MainConst.nBands
					+ vArray.elementAt(i).x * MainConst.nBands]
					* DEPTH);
			Log.i("Map",
					"vertexTemp index " + i + " value : "
							+ vertexTemp.elementAt(i));

		}

		float[] temp = new float[vertexTemp.size()];
		for (int i = 0; i < vertexTemp.size(); i++) {
			temp[i] = vertexTemp.elementAt(i);
		}

		FloatBuffer temp_buffer = BufferUtil.makeFloatBuffer(temp);

		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		// gl.glVertexPointer(3, GL10.GL_FLOAT, 0, vertexBuffer);
		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, temp_buffer);
		gl.glEnableClientState(GL10.GL_COLOR_ARRAY);
		gl.glColorPointer(4, GL10.GL_FLOAT, 0, colorBuffer);
		// gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY); // Enable
		// texture-coords-array (NEW)
		// gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, texBuffer); // Define
		// texture-coords buffer (NEW)

		/*
		 * // front gl.glPushMatrix(); gl.glTranslatef(0.0f, 0.0f, 1.0f);
		 * gl.glDrawArrays(GL10.GL_TRIANGLE_STRIP, 0, 4); gl.glPopMatrix();
		 */
		gl.glDisableClientState(GL10.GL_TEXTURE_COORD_ARRAY); // Disable
																// texture-coords-array
																// (NEW)
		gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glDisable(GL10.GL_CULL_FACE);
	}

	// Load an image into GL texture
	public void loadTexture(GL10 gl, Context context) {
		gl.glGenTextures(1, textureIDs, 0); // Generate texture-ID array

		gl.glBindTexture(GL10.GL_TEXTURE_2D, textureIDs[0]); // Bind to texture
																// ID
		// Set up texture filters
		gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER,
				GL10.GL_NEAREST);
		gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER,
				GL10.GL_LINEAR);

		// Build Texture from loaded bitmap for the currently-bind texture ID
		GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);
		bitmap.recycle();
	}
}