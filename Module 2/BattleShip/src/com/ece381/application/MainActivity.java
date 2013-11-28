package com.ece381.application;

import java.io.IOException;
import java.net.Socket;

import com.example.myfirstapp.R;

import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

public class MainActivity extends Activity  {
	
	public final static String EXTRA_MESSAGE = "com.example.myfirstapp.MESSAGE";
	private SoundPool sp;
    private int soundId;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// creates the activity
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		// set up sound
        this.setVolumeControlStream(AudioManager.STREAM_MUSIC);	
		sp = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
		soundId = sp.load(this, R.raw.menu, 1);
		// close any opened sockets
		closeSocket();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		// Adds the button into the action bar + action overflow
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.main, menu);
		return super.onCreateOptionsMenu(menu);
	}
	
	public void playAI(View view)
	{
		Intent intent = new Intent(this, BattleShipGame.class);
		intent.putExtra(EXTRA_MESSAGE, "SINGLE");
		sp.play(soundId, 100, 100, 1, 0, 1f);
		startActivity(intent);
	}
	
	public void playMulti(View view)
	{
		Intent intent = new Intent(this, ConnectToHost.class);
		sp.play(soundId, 100, 100, 1, 0, 1f);
		startActivity(intent);
	}
	
	// Called when the user closes a socket
	private void closeSocket() {
		BattleShipApp app = (BattleShipApp) getApplication();
		Socket s = app.sock;
		try {
			if (s != null)
			{
				s.getOutputStream().close();
				s.close();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
}
