package com.example.ece381;

import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;

import android.app.Activity;
import android.app.Application;
import android.util.Log;
import android.widget.EditText;
import android.widget.TextView;

public class MyApplication extends Application {
	///////////////////////////
	//Static Application Data//
	///////////////////////////
	Socket sock = null; //is socket open?
	boolean ack = true; //has the sent packet been read?
	TCPReadThread tcp_read;
	Activity currentActivity;
	TextView currentTextView;
	
	Object lock;
	
	@Override
	public void onCreate(){
		super.onCreate();
		tcp_read = new TCPReadThread();
		tcp_read.start();
	}
	
	public class TcpData {
		public byte dataType;
		public byte data[];
	}

	public class TCPReadThread extends Thread {
		//data
		private byte buffer[] = new byte[512];
		private byte dataType;
		private int size;
		
		@Override
		public void run() {
			//wait for socket to be connected
			while(!(sock != null && sock.isConnected() && !sock.isClosed()));
				
			try {
				InputStream in = sock.getInputStream();


				while(true) {
					// See if any bytes are available from the Middleman
					size = (byte) in.read();
					if(size == 0) {
						ack = true;
						Log.i("ack","true");
					} else{
						dataType = (byte) in.read();
						size--;
						
						int amount = in.read(buffer,0,size);
						Log.i("COMM", "Data Received -> Expected: " + String.valueOf(size) + " Got: " + String.valueOf(amount));
						//read the rest of the data if it wasn't read in properly
						if(amount < size) {
							for(int i = amount; i < size; i++) {
								buffer[i] = (byte)in.read();
							}
						}
						
						TcpData d = new TcpData();
						d.data = new byte[size];
						int i;
						for(i=0;i<size;i++){
							d.data[i] = buffer[i];
						}
						d.dataType = dataType;
						
						synchronized(lock) {
							if(currentActivity != null) {
								switch(dataType){
									case 0:
										//speedtest
										//do nothing
										break;
									case 1:
										// If so, read them in and create a sring
										final String s = new String(buffer, 0, size-1, "US-ASCII");
						
										// As explained in the tutorials, the GUI can not be
										// updated in an asyncrhonous task.  So, update the GUI
										// using the UI thread.
										currentActivity.runOnUiThread(new Runnable() {
											public void run() {
												EditText et = (EditText) currentActivity.findViewById(R.id.RecvdMessage);
												et.setText(s);
											}
										});
										break;
								}
							}
						}
					}
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
}
