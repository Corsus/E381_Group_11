package com.ece381.application;

import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import com.ece381.application.R;

import android.media.AudioManager;
import android.media.SoundPool;
import android.os.AsyncTask;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class ConnectToHost extends Activity {

	private SoundPool sp;
    private int soundId;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_connect_to_host);
		// set up sound
        this.setVolumeControlStream(AudioManager.STREAM_MUSIC);	
		sp = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
		soundId = sp.load(this, R.raw.menu, 1);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.connect_to_host, menu);
		return true;
	}
	
	// Route called when the user presses "connect"
	public void openSocket(View view) {
		findViewById(R.id.connect_button).setEnabled(false);
		findViewById(R.id.connect_button).setClickable(false);
		BattleShipApp app = (BattleShipApp) getApplication();
		
		sp.play(soundId, 100, 100, 1, 0, 1f);

		// Make sure the socket is not already opened 
		
		if (app.sock != null && app.sock.isConnected() && !app.sock.isClosed()) {
			Toast.makeText(this, "Socket already open...", Toast.LENGTH_SHORT)
			.show();
			return;
		}
		
		// open the socket.  SocketConnect is a new subclass
	    // (defined below).  This creates an instance of the subclass
		// and executes the code in it.
		new SocketConnect().execute((Void) null);
	}
		
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
			BattleShipApp myApp = (BattleShipApp) ConnectToHost.this
					.getApplication();
			myApp.sock = s;
			Intent intent = new Intent(ConnectToHost.this, BattleShipGame.class);
			startActivity(intent);
		}
	}
}
