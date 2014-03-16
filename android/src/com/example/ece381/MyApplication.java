package com.example.ece381;

import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;

import android.app.Application;
import android.util.Log;
import android.widget.EditText;

public class MyApplication extends Application {
	///////////////////////////
	//Static Application Data//
	///////////////////////////
	Socket sock = null; //is socket open?
	boolean ack = true; //has the sent packet been read?
	TCPReadThread tcp_read;
	MyActivity currentActivity;
	//TextView currentTextView;
	
	Object lock = new Object();
	
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
								currentActivity.handlesTCPData(d);
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
