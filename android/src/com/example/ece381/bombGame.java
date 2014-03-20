package com.example.ece381;



import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;

import android.app.ActionBar;
import android.content.Intent;
import android.view.*;
import android.widget.Toast;
import android.graphics.Point;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector.OnGestureListener;
import android.widget.ImageView;
import android.widget.TextView;

//import android.opengl.GLSurfaceView;

/**
 * Created by Kit on 18/03/14.
 */
public class bombGame extends MyActivity implements OnGestureListener {
	//private GLSurfaceView surfaceViewer;
	private GestureDetector gestureDetector;
    boolean hasBomb;
	byte[] player;
	String[] playerNames;
	int playerNameIndex;

	
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		gestureDetector = new GestureDetector(this,this);
		Intent intent = getIntent();
		player = intent.getByteArrayExtra("playerName");
		playerNames = new String[3];
        //ImageView img = (ImageView) findViewById(R.id.bombImage);

		//img.setVisibility(View.INVISIBLE);

		setContentView(R.layout.bomb_game);

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.bomb, menu);
		return true;
	}
	



    @Override
    public boolean onTouchEvent(MotionEvent e){
        return gestureDetector.onTouchEvent(e);
    }



	@Override
	public boolean onDown (MotionEvent e){
		return false;
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
		int maxX;
	    int maxY;
	    Point point = new Point();
	    Display display = getWindowManager().getDefaultDisplay();

	    display.getSize(point);
        maxY = point.y;
        maxX = point.x;

        float onDown[] = new float[2];
        float onUp[] = new float[2];
		
		onDown[0] = e1.getRawX();
		onDown[1] = e1.getRawY();
		onUp[0] = e2.getRawX();
		onUp[1] = e2.getRawY();

        if(( onDown[0] < 830 || onDown[0] >1100 || onDown[1] >1000 || onDown[1]<800)){

			Toast.makeText(this,"not the bomb", Toast.LENGTH_SHORT).show();
			return false;
		}

        if(onUp[0] <550){
           // passBomb(0);
			Toast.makeText(this,"passed bomb to player 0", Toast.LENGTH_SHORT).show();
        }
        else if(onUp[0] <1300){
			//passBomb(2);
			Toast.makeText(this,"passed bomb to player 2", Toast.LENGTH_SHORT).show();
        }
        else{
			//passBomb(1);
			Toast.makeText(this,"passed bomb to player 1", Toast.LENGTH_SHORT).show();
        }
        //ImageView img = (ImageView) findViewById(R.id.bombImage);
		//img.setVisibility(View.INVISIBLE);

		return true;
	}

	@Override
	public void onLongPress(MotionEvent e){
		return;
	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY){
		return false;
	}

	@Override
	public void onShowPress (MotionEvent e){
		return;
	}

	@Override
	public boolean onSingleTapUp (MotionEvent e){
		return false;
	}

	@Override
	public void handlesTCPData(MyApplication.TcpData d) {
		
        switch(d.dataType){
			case 10:
				try{
					if(Arrays.equals(d.data, player))
						break;
					playerNames[playerNameIndex++] = new String(d.data,0,d.data.length,"US-ASCII");

				}catch (Exception e){
					e.printStackTrace();
				}
				break;
            case 11:
                hasBomb=true;
                //ImageView img = (ImageView) findViewById(R.id.bombImage);
				//img.setVisibility(View.VISIBLE);

			case 12:
				setPlayerName();
        }

	//;
	}


	private void setPlayerName()
	{
		TextView player0= (TextView) findViewById(R.id.player0);
        TextView player1= (TextView) findViewById(R.id.player1);
        TextView player2= (TextView) findViewById(R.id.player2);
    	
		for(int i =0; i<playerNames.length; i++)
		{
			switch(i){
				case 0:
					player0.setText(playerNames[i]);

				case 1:
					player1.setText(playerNames[i]);

				case 2:
					player2.setText(playerNames[i]);
			}
		}
	}

	private void passBomb(int i){
		
		MyApplication app= (MyApplication) getApplication();
		byte buf[] = new byte[playerNames[i].length()+1];
		buf[0] = 11; //data type: bomb
		System.arraycopy(playerNames[i].getBytes(), 0, buf, 1, playerNames[i].length());

		OutputStream out;
		try {
			out = app.sock.getOutputStream();
			try {
				out.write(buf, 0, playerNames[i].length()+1);
				app.ack = false;
				Log.i("ack", "false");
			} catch (IOException e) {
				e.printStackTrace();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}

