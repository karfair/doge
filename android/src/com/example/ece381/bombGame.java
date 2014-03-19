package com.example.ece381;


import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.Display;
import android.graphics.Point;
import android.widget.ImageView

/**
 * Created by Kit on 18/03/14.
 */
public abstract class bombGame extends MyActivity implements GestureDetector.OnGestureListener {
	//private GLSurfaceView surfaceViewer;
	private GestureDetector gestureDetector;
    private int maxX;
    private int maxY;
    private float onDown[] = new float[2];
    private float onUp[] = new float[2];
    private boolean hasBomb;
    ImageView img;

    Point point = new Point();
    Display display = getWindowManager().getDefaultDisplay();

	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.bomb_game);
		gestureDetector = new GestureDetector(this,this);

        display.getSize(point);
        maxY = point.y;
        maxX = point.x;
        img = (ImageView) findViewById(R.id.bombImage);
        Bitmap bombBMP = BitmapFactory.decodeResource(getResources(),R.drawable.ic_launcher);
	}

    @Override
    public boolean onTouchEvent(MotionEvent e){
        return gestureDetector.onTouchEvent(e);
    }
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
		onDown[0] = e1.getRawX();
		onDown[1] = e1.getRawY();
		onUp[0] = e2.getRawX();
		onUp[1] = e2.getRawY();

        if(( onDown[0] < 880 || onDown[0] >1050 || onDown[1] >880 || onDown[1]<800)|| !hasBomb)
            return false;

        if(onUp[0] <550){
            ;
        }
        else if(onUp[0] <1300){
            ;
        }
        else{
            ;
        }




		return true;
	}

	@Override
	public void handlesTCPData(MyApplication.TcpData d) {
        switch(d.dataType){
            case 1:
                hasBomb=true;
                img.setImageBitmap();
        }
	}
}