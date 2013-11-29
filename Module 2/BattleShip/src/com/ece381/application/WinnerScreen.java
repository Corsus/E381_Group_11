package com.ece381.application;

import com.ece381.application.R;

import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.widget.TextView;

public class WinnerScreen extends Activity {

	private SoundPool sp;
    private int soundId;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_winner_screen);
		
		// set up sound
        this.setVolumeControlStream(AudioManager.STREAM_MUSIC);	
		sp = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
		soundId = sp.load(this, R.raw.menu, 1);
		
		Intent intent = getIntent();
		String message = intent.getStringExtra(BattleShipGame.EXTRA_MESSAGE);
		if (message != null && message.equals("WIN"))
		{
			TextView tv = (TextView) findViewById(R.id.result_text);
			tv.setText(R.string.winner_string);
		}
		else if (message != null && message.equals("LOSE"))
		{
			TextView tv = (TextView) findViewById(R.id.result_text);
			tv.setText(R.string.loser_string);
		}
	}

	@Override
	public void onBackPressed() {
		//do nothing, because you should be able to go back from here
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.winner_screen, menu);
		return true;
	}
	
	public void returnToMain(View view)
	{
		sp.play(soundId, 100, 100, 1, 0, 1f);
		Intent intent = new Intent(this, MainActivity.class);
		intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		startActivity(intent);
	}
}
