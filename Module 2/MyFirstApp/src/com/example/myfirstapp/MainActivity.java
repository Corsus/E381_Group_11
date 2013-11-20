package com.example.myfirstapp;

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

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// creates the activity
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		// Adds the button into the action bar + action overflow
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.main, menu);
		return super.onCreateOptionsMenu(menu);
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		//this is where we handle the action button presses
		int itemId = item.getItemId();
		switch(itemId)
		{
			case R.id.action_search:
				Toast t_search = Toast.makeText(getApplicationContext(), "Search", Toast.LENGTH_LONG);
				t_search.show();
				return true;
			case R.id.action_settings:
				Toast t_settings = Toast.makeText(getApplicationContext(), "Settings", Toast.LENGTH_LONG);
				t_settings.show();
				return true;
			default:
				return super.onOptionsItemSelected(item);
		}
	}
	
	public void playAI(View view)
	{
		Intent intent = new Intent(this, SetupGameBoard.class);
		intent.putExtra(EXTRA_MESSAGE, "SINGLE");
		startActivity(intent);
	}
	
	public void playMulti(View view)
	{
		Intent intent = new Intent(this, ConnectToHost.class);
		startActivity(intent);
	}
	
}
