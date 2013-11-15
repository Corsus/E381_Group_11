package com.example.myfirstapp;

import java.io.IOException;
import java.io.InputStream;
import java.util.TimerTask;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.widget.Toast;

public class ConnectedActivity extends Activity {
	
	private String msgReceived;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_connected);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.connected, menu);
		return true;
	}
	
	public String getMsgReceived()
	{
		return this.msgReceived;
	}
		
	public class TCPReadTimerTask extends TimerTask {
		public void run() {
			final BattleShipApp app = (BattleShipApp) getApplication();
			if (app.sock != null && app.sock.isConnected()
					&& !app.sock.isClosed()) {
				
				try {
					InputStream in = app.sock.getInputStream();

					// See if any bytes are available from the Middleman
					int bytes_avail = in.available();
					if (bytes_avail > 0) {
						
						// If so, read them in and create a sring
						byte buf[] = new byte[bytes_avail];
						in.read(buf);

						msgReceived = new String(buf, 0, bytes_avail, "US-ASCII");
		
						// GUI can not be updated in an asyncrhonous task.  
						// So, update the GUI using the UI thread.
						runOnUiThread(new Runnable() {
							public void run() {
								Toast.makeText(app, msgReceived, Toast.LENGTH_SHORT).show();
							}
						});
						
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}
}


