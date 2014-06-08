package com.example.mapview;

import java.nio.ByteBuffer;

import android.content.Context;
import android.graphics.Bitmap;

public class MainConst {

	public static Context context;
	public static Camera camera;
	
	public static byte[] imageBuf;
	public static int nXSize;
	public static int nYSize;
	public static int nBands = 3;
	
	public static Frustum pFrustum;
	public static ZQuadTree qt;

}
