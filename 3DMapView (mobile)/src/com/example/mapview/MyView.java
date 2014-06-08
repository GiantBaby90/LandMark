package com.example.mapview;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class MyView extends GLSurfaceView {
	private MyRenderer renderer = null;
	
	private final float TOUCH_SCALE_FACTOR = 90.0f / 320.0f;
	private float previousX;
	private float previousY;
	
	public MyView(Context context) {
		super(context);
		renderer = new MyRenderer(context);
		setRenderer(renderer);
		
		this.requestFocus();
		this.setFocusableInTouchMode(true);
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent evt) {
		switch(keyCode) {
	    case KeyEvent.KEYCODE_DPAD_LEFT:   // Decrease Y-rotational speed
	    	renderer.speedY -= 0.1f;
	        break;
	    case KeyEvent.KEYCODE_DPAD_RIGHT:  // Increase Y-rotational speed
	    	renderer.speedY += 0.1f;
	        break;
	    case KeyEvent.KEYCODE_DPAD_UP:     // Decrease X-rotational speed
	    	renderer.speedX -= 0.1f;
	        break;
	    case KeyEvent.KEYCODE_DPAD_DOWN:   // Increase X-rotational speed 
	    	renderer.speedX += 0.1f;
	        break;
	    case KeyEvent.KEYCODE_A:           // Zoom out (decrease z)
	    	renderer.z -= 0.2f;
	        break;
	    case KeyEvent.KEYCODE_Z:           // Zoom in (increase z)
	    	renderer.z += 0.2f;
	        break;
	    }
		
		return true;  // Event handled
	}

	
	public boolean onTouchEvent(final MotionEvent evt) {
		/*
		queueEvent(new Runnable() {
			public void run() {
				//renderer.setColor(evt.getX()/getWidth(), evt.getY()/getHeight(), 1.0f);
			}
		});
		
		return true;
		*/
		
		float currentX = evt.getX();
	      float currentY = evt.getY();
	      float deltaX, deltaY;
	      switch (evt.getAction()) {
	         case MotionEvent.ACTION_MOVE:
	            // Modify rotational angles according to movement
	            deltaX = currentX - previousX;
	            deltaY = currentY - previousY;
	            renderer.angleX += deltaY * TOUCH_SCALE_FACTOR;
	            renderer.angleY -= deltaX * TOUCH_SCALE_FACTOR;
	      }
	      // Save current x, y
	      previousX = currentX;
	      previousY = currentY;
	      return true;  // Event handled
		
		
	}
}
