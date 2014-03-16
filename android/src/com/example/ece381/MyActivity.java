package com.example.ece381;

import com.example.ece381.MyApplication.TcpData;

import android.app.Activity;

public abstract class MyActivity extends Activity {
	public abstract void handlesTCPData(TcpData d);
	
	@Override
	public void onResume(){
		super.onResume();
		synchronized(((MyApplication)getApplication()).lock){
			((MyApplication)getApplication()).currentActivity = this;
		}
	}
	
	@Override
	public void onPause(){
		super.onPause();
		synchronized(((MyApplication)getApplication()).lock){
			((MyApplication)getApplication()).currentActivity = null;
		}
	}
}
