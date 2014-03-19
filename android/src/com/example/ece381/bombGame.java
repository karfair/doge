package com.example.ece381;


import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.GestureDetector;
import android.view.MotionEvent;

/**
 * Created by Kit on 18/03/14.
 */
public abstract class bombGame extends MyActivity implements GestureDetector.OnGestureListener {
	private GLSurfaceView m
	private GestureDetector gestureDetector;
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.bomb_game);
		gestureDetector = new GestureDetector(this,this);

	}


	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
		float onDown[] = new float[2];
		float onUp[] = new float[2];
		onDown[0] = e1.getRawX();
		onDown[1] = e1.getRawY();
		onUp[0] = e2.getRawX();
		onUp[1] = e2.getRawY();



		return false;
	}

	@Override
	public void handlesTCPData(MyApplication.TcpData d) {
		;
	}
}