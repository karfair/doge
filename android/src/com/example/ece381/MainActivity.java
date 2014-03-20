package com.example.ece381;

import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.StrictMode;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

import com.example.ece381.MyApplication.TcpData;

public class MainActivity extends Activity {

	//stuff I wrote
	//speed test and latency test
	/*public void speedTest(View view){
		
		final int BYTES_TO_SEND = 30;
		final int ITERATIONS = 200;
		
		MyApplication app = (MyApplication) getApplication();
		
		//fake data to send
		byte buf[] = new byte[BYTES_TO_SEND];

		//stream data
		OutputStream out;
		
		
		try {
			//init
			out = app.sock.getOutputStream();

			long latS;
			long latE;
			long startTime = System.nanoTime();
			
			for(int i = 0; i < ITERATIONS; i++){
				latS = System.nanoTime();
				
				//write fake message
				app.ack = false;
				out.write(buf, 0, BYTES_TO_SEND);
				
				while(app.ack==false);

				latE = System.nanoTime();
				Log.i("MainActivity", "latency is: " + String.valueOf((latE-latS)/1000000) + "mS" + " IT: " + String.valueOf(i));
				//is this an ack????
				//assert(buf[0] == 0);
			}
			
			long endTime = System.nanoTime();
			
			double speed = ((double)BYTES_TO_SEND*ITERATIONS)/(((double)(endTime-startTime)))*1000000000;
			
			Log.i("MainActivity", "speed is: " + String.valueOf(speed) + "Bps");
			Log.i("MainActivity", "speed is: " + String.valueOf(8*speed) + "bps");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}*/

	MyApplication app;
	EditText nameText;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		
		// This call will result in better error messages if you
		// try to do things in the wrong thread.
		
		StrictMode.setThreadPolicy(new StrictMode.ThreadPolicy.Builder()
				.detectDiskReads().detectDiskWrites().detectNetwork()
				.penaltyLog().build());

		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		app = (MyApplication) getApplication();

		/*EditText et = (EditText) findViewById(R.id.RecvdMessage);
		et.setKeyListener(null);
		et = (EditText) findViewById(R.id.error_message_box);
		et.setKeyListener(null);

*/

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}

	// Route called when the user presses "connect"
	
	public void openSocket(View view) {


		//If no name is inputted in the name field, the program will notify the user and will return.
		nameText = (EditText) findViewById(R.id.NameText);
		String name = nameText.getText().toString();
		if(name.contentEquals("")){
			Toast.makeText(getApplicationContext(),"Enter a Name",Toast.LENGTH_LONG).show();
			return;
		}
		//Copy name into a buffer
		app.nameData.data = name.getBytes();
		app.nameData.dataType = 10;

		// Make sure the socket is not already opened 
		//app.ack = true;
		
		if (app.sock != null && app.sock.isConnected() && !app.sock.isClosed()) {
			Toast t = Toast.makeText(this,"Socket is already open",Toast.LENGTH_LONG);
			t.show();
			return;
		}
		
		// open the socket.  SocketConnect is a new subclass
	    // (defined below).  This creates an instance of the subclass
		// and executes the code in it.
		
		new SocketConnect().execute((Void) null);
		
		Intent intent = new Intent(this, bombGame.class);
		intent.putExtra("playerName",app.nameData.data);
		startActivity(intent);
	}

//	public void startGame(View view){
//		//New intent to start bombGame
//		Intent intent = new Intent(this, bombGame.class);
//		intent.putExtra("playerName",name_buff);
//		startActivity(intent);
//	}


	// Construct an IP address from the four boxes
	
	public String getConnectToIP() {
		String addr = "";
		EditText text_ip;
		text_ip = (EditText) findViewById(R.id.ip1);
		addr += text_ip.getText().toString();
		text_ip = (EditText) findViewById(R.id.ip2);
		addr += "." + text_ip.getText().toString();
		text_ip = (EditText) findViewById(R.id.ip3);
		addr += "." + text_ip.getText().toString();
		text_ip = (EditText) findViewById(R.id.ip4);
		addr += "." + text_ip.getText().toString();
		return addr;
	}

	// Gets the Port from the appropriate field.
	
	public Integer getConnectToPort() {
		Integer port;
		EditText text_port;

		text_port = (EditText) findViewById(R.id.port);
		port = Integer.parseInt(text_port.getText().toString());

		return port;
	}


    // This is the Socket Connect asynchronous thread.  Opening a socket
	// has to be done in an Asynchronous thread in Android.  Be sure you
	// have done the Asynchronous Tread tutorial before trying to understand
	// this code.
	
	public class SocketConnect extends AsyncTask<Void, Void, Socket> {

		// The main parcel of work for this thread.  Opens a socket
		// to connect to the specified IP.

		protected Socket doInBackground(Void... voids) {
			Socket s = null;
			String ip = getConnectToIP();
			Integer port = getConnectToPort();

			try {
				s = new Socket(ip, port);
			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
			return s;
		}

		// After executing the doInBackground method, this is 
		// automatically called, in the UI (main) thread to store
		// the socket in this app's persistent storage
		
		protected void onPostExecute(Socket s) {
			MyApplication app = (MyApplication) MainActivity.this.getApplication();
			app.sock = s;
			app.sendData(app.nameData);
			
//			app.sock = s;
//
//			OutputStream out;
//			try{
//				out = app.sock.getOutputStream();
//				try{
//					app.ack= false;
//					out.write(name_buff,0,name_buff.length);
//
//				}catch (IOException e){
//					e.printStackTrace();
//				}
//			}catch (IOException e){
//				e.printStackTrace();
//			}
		}
	}

	
	/*public void handlesTCPData(TcpData d) {
		switch(d.dataType){
		case 0:
			//speedtest
			//do nothing
			break;
		case 1:
			try{
				// If so, read them in and create a sring
				final String s = new String(d.data, 0, d.data.length, "US-ASCII");
	
				// As explained in the tutorials, the GUI can not be
				// updated in an asyncrhonous task.  So, update the GUI
				// using the UI thread.
				runOnUiThread(new Runnable() {
					public void run() {
						EditText et = (EditText) findViewById(R.id.RecvdMessage);
						et.setText(s);
					}
				});
			}catch(Exception E){}
			break;

	}*/

}

