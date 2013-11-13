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
		startActivity(intent);
	}
	
	public void playMulti(View view)
	{
		Intent intent = new Intent(this, ConnectToHost.class);
		startActivity(intent);
	}
	
	// function invoked when SEND button is pressed
	public void sendMessage(View view)
	{
		// For onClick functions, they must be public, they must return void, they must take a View as param
		// Do something in response to button
		
		// Intent is an object that gives runtime binding between separate components (such as activities)
		// "Intent to do something", a link between activities
		
		// In this case, we are linking MainActivity and DisplayMessageActivity
		Intent intent = new Intent(this, DisplayMessageActivity.class);
		
		// Intents can also carry bundle of data from one component to another
	/*	EditText editText = (EditText) findViewById(R.id.edit_message);
		String message = editText.getText().toString();
		intent.putExtra(EXTRA_MESSAGE, message);*/
		
		// in this case, we get the edit_message view
		// then, we get the text from this view
		// finally, we bundle it with the intent as an "extra"
		// EXTRA_MESSAGE is a key used to query the data in the next activity
		// It's good practice to prefix the key with the package name "com.example.myfirstapp"
		
		startActivity(intent);		//this starts the second activity	
		
	}
}
