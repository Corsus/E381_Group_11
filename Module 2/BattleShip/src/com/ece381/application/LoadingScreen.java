package com.ece381.application;

import java.io.IOException;
import java.io.InputStream;
import java.util.Timer;
import java.util.TimerTask;

import com.ece381.application.R;


import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;

public class LoadingScreen extends Activity {

	private ReadySignalHandler readySignalHandler = new ReadySignalHandler();
	private Timer tcp_timer = new Timer();
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_loading_screen);
		
		tcp_timer.schedule(readySignalHandler, 0, 500);
	}
	
	private void finishLoading()
	{
		tcp_timer.cancel();
		setResult(1);
		finish();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.loading_screen, menu);
		return true;
	}
	
	
	
	@Override
	public void onBackPressed() {
		//we don't want to go back here, so do nothing
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
						if (msgReceived.charAt(0) == 'R')
						{
							readySignalHandler.cancel();
							finishLoading();
						}	
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}

}
