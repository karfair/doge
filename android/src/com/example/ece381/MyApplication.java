package com.example.ece381;

import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;

import android.app.Application;
import android.util.Log;
import android.widget.EditText;

public class MyApplication extends Application {
	Socket sock = null;
	boolean ack = true;
	Thread TCPread;
	
	@Override
	public void onCreate(){
		super.onCreate();
		
	}
	
	// This is a timer Task.  Be sure to work through the tutorials
	// on Timer Tasks before trying to understand this code.
	/*
	public class TCPReadThread extends Thread {
		//data
		public byte buffer[] = new byte[256];
		public byte dataType;
		public int  size;
		
		//data
		public byte buffer[] = new byte[256];
		public byte dataType;
		public int  size;
		
		@Override
		public void run() {

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
						
						int amount = in.read(buffer,0,size-1);
						Log.i("COMM", "Data Received -> Expected: " + String.valueOf(size-1) + " Got: " + String.valueOf(amount));
						//read the rest of the data if it wasn't read in properly
						if(amount < size) {
							for(int i = amount; i < size; i++) {
								buffer[i] = (byte)in.read();
							}
						}
					}
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}*/
}
