package com.example.myfirstapp;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class WinnerScreen extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_winner_screen);
		
		Intent intent = getIntent();
		String message = intent.getStringExtra(SetupGameBoard.EXTRA_MESSAGE);
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
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.winner_screen, menu);
		return true;
	}
	
	public void returnToMain(View view)
	{
		Intent intent = new Intent(this, MainActivity.class);
		startActivity(intent);
	}
}
