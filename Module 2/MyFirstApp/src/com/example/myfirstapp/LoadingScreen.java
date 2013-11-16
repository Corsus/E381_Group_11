package com.example.myfirstapp;

import java.io.IOException;
import java.io.InputStream;
import java.util.Timer;
import java.util.TimerTask;


import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.widget.Toast;

public class LoadingScreen extends Activity {

	private ReadySignalHandler readySignalHandler = new ReadySignalHandler();
	private Timer tcp_timer = new Timer();
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_loading_screen);
		
		tcp_timer.schedule(readySignalHandler, 3000, 500);
	}
	
	private void finishLoading()
	{
		
		Toast.makeText(this, "Loading Completed...", Toast.LENGTH_SHORT).show();
		setResult(1);
		finish();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.loading_screen, menu);
		return true;
	}
	
	public class ReadySignalHandler extends TimerTask {
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

						String msgReceived = new String(buf, 0, bytes_avail, "US-ASCII");
						//if we get the Ready signal, we move on to next activity
						if (msgReceived.equals("R"))
						{
							runOnUiThread(new Runnable() {
								public void run() {
									tcp_timer.cancel();
									finishLoading();
								}
							});
						}	
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}

}
