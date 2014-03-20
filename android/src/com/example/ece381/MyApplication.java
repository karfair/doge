package com.example.ece381;

import android.app.Application;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class MyApplication extends Application {
	///////////////////////////
	//Static Application Data//
	///////////////////////////
	Socket sock = null; //is socket open?
	boolean ack = true; //has the sent packet been read?
	TCPReadThread tcp_read;
	MyActivity currentActivity;
	//TextView currentTextView;
	
	
	//name data
	TcpData nameData = new TcpData();
	
	//lock for currentActivity
	Object lock = new Object();
	
	//ack_lock
	Object ack_lock = new Object();
	
	@Override
	public void onCreate(){
		super.onCreate();
		tcp_read = new TCPReadThread();
		tcp_read.start();
	}
	
	public synchronized boolean sendData(TcpData d){
		// Now send through the output stream of the socket
		if(!ack){
			return false;
		}else{
			ack = false;
			Log.i("COMM","ack = false");
		}
		
		byte buffer[] = new byte[d.data.length+1];
		Log.i("COMM","dataType:" + String.valueOf(d.dataType) + "length" + String.valueOf(d.data.length));
		buffer[0] = d.dataType;
		System.arraycopy(d.data, 0, buffer, 1, d.data.length);
		
		OutputStream out;
		try {
			out = sock.getOutputStream();
			out.write(buffer, 0, d.data.length+1);
			Log.i("COMM", "sendData():sent");

		} catch (IOException e) {
			e.printStackTrace();
			Log.i("COMM", "sendData() error");
		}
		Log.i("COMM", "returning");
		return true;
	}
	
	public static class TcpData {
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
						synchronized(lock) {
							if(currentActivity != null) {
								currentActivity.handlesAck();
							}
						}
						Log.i("COMM","ack = true");
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
