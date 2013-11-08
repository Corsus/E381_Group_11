package com.example.myfirstapp;

import android.os.Build;
import android.os.Bundle;
import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Activity;
import android.content.Intent;
import android.support.v4.app.NavUtils;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.GridLayout;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.Toast;
import android.widget.ViewFlipper;

import com.example.models.*;
import com.example.models.Battleship.ShipOrientation;

public class SetupGameBoard extends Activity {

	private View setupPanel;
	private View playPanel;
	private GridLayout myBoardLayout;
	private GridLayout fogBoardLayout;

	private int crossfadeAnimationDuration;

	private ViewFlipper viewflipper;
	private GameMapOnTouchListener mapTouchListener;
	private float previous_x_position;
	private float current_x_position;

	private Battleship shipOnHand;
	private GameBoard gameBoard;

	private ShipOrientation setupOrientation = ShipOrientation.HORIZONTAL;

	private int[] fire_coordinates;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_setup_game_board);

		// Make sure we're running on Honeycomb or higher to use ActionBar APIs
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
			// Show the Up button in the action bar.
			getActionBar().setDisplayHomeAsUpEnabled(true);
		}

		// reference view flipper
		viewflipper = (ViewFlipper) findViewById(R.id.GameMapsLayout);
		mapTouchListener = new GameMapOnTouchListener();

		// create the game boards
		this.gameBoard = new GameBoard();

		// reference the button panels
		playPanel = findViewById(R.id.playPanel);
		setupPanel = findViewById(R.id.setupPanel);

		// reference the map layouts
		fogBoardLayout = (GridLayout) findViewById(R.id.fogBoardLayout);
		myBoardLayout = (GridLayout) findViewById(R.id.myBoardLayout);

		// initially hide this play panel
		playPanel.setVisibility(View.GONE);

		// set the animation time for crossfade
		crossfadeAnimationDuration = getResources().getInteger(
				android.R.integer.config_mediumAnimTime);
		
		//after all initialization, we draw the map
		drawGameBoard();
		drawFogBoard();
	}

	//==================Layout Related================//
	
	private void drawGameBoard() {
		Tile theTile;
		ImageView imageView;
		for (int i = 0; i < GameBoard.getBoardWidth(); i++) {
			for (int j = 0; j < GameBoard.getBoardHeight(); j++) {
				theTile = this.gameBoard.getTileAt(i, j);
				if (theTile.getTileObject() == null) {
					imageView = new ImageView(this);
					imageView.setImageResource(R.drawable.ic_launcher);
					imageView.setOnTouchListener(mapTouchListener);
					myBoardLayout.addView(imageView,
							new GridLayout.LayoutParams(GridLayout.spec(j),
									GridLayout.spec(i)));
				}
			}
		}
	}
	
	private void drawFogBoard() {
		ImageView imageView;
		for (int i = 0; i < GameBoard.getBoardWidth(); i++) {
			for (int j = 0; j < GameBoard.getBoardHeight(); j++) {
				// initialize the tile image
				imageView = new ImageView(this);
				imageView.setImageResource(R.drawable.space);
				imageView.setOnTouchListener(mapTouchListener);
				// add to layout
				fogBoardLayout.addView(imageView, new GridLayout.LayoutParams(
						GridLayout.spec(j), GridLayout.spec(i)));
			}
		}
	}

	// the index goes by columns
	// index 0 = (0,0)
	// index 1 = (0,1)
	// index++ goes down the column rather than across the row
	private int[] indexToCoordinates(int index) {
		int[] coordinates = new int[2];
		// figure out which column (x)
		coordinates[0] = (int) (Math.floor(index / GameBoard.getBoardHeight()));
		// figure out which row (y)
		coordinates[1] = (int) (index % GameBoard.getBoardHeight());
		return coordinates;
	}

	private void drawShipOnMap(Battleship ship, int index) {
		GridLayout gl = (GridLayout) findViewById(R.id.myBoardLayout);
		switch (ship.getOrientation()) {
		case HORIZONTAL: 
			for (int i = 0; i < ship.getSize(); i++) {
				ImageView iv = (ImageView) gl.getChildAt(index + i
						* GameBoard.getBoardHeight());
				iv.setImageResource(R.drawable.ship);
			}
			break;
		case VERTICAL: 
			for (int i = 0; i < ship.getSize(); i++) {
				ImageView iv = (ImageView) gl.getChildAt(index + i);
				iv.setImageResource(R.drawable.ship);
			}
			break;
		}
	}

	//=================Game Logic Related========================//
	
	// onClick handler for the 4 ship selector buttons
	public void selectShipOnClick(View view) {
		ImageButton button_clicked = (ImageButton) view;
		switch (button_clicked.getId()) {
		case R.id.SelectScout:
			Battleship ship1 = new Battleship(1, setupOrientation);
			shipOnHand = ship1;
			break;
		case R.id.SelectCruiser:
			Battleship ship2 = new Battleship(2, setupOrientation);
			shipOnHand = ship2;
			break;
		case R.id.SelectDestroyer:
			Battleship ship3 = new Battleship(3, setupOrientation);
			shipOnHand = ship3;
			break;
		case R.id.SelectMothership:
			Battleship ship4 = new Battleship(4, setupOrientation);
			shipOnHand = ship4;
			break;
		}
	}

	// Disables the ship selector button when ship has been placed.
	// This is called after the ship has been placed onto the board.
	private void disableSelectShipButton(Battleship ship) {
		ImageButton ib = null;
		switch (ship.getSize()) {
		case 1:
			ib = (ImageButton) findViewById(R.id.SelectScout);
			break;
		case 2:
			ib = (ImageButton) findViewById(R.id.SelectCruiser);
			break;
		case 3:
			ib = (ImageButton) findViewById(R.id.SelectDestroyer);
			break;
		case 4:
			ib = (ImageButton) findViewById(R.id.SelectMothership);
			break;
		}
		if (ib != null) {
			ib.setEnabled(false);
			ib.setClickable(false);
		}
	}

	// Check to see if the setup is complete.
	// If so, enables the ready button.
	// Called after a ship has been placed on the board.
	private void isBoardReady() {
		if (this.gameBoard.getShipOnBoard().size() == 4) {
			Button b = (Button) findViewById(R.id.readyButton);
			b.setEnabled(true);
			b.setClickable(true);
			b = (Button) findViewById(R.id.changeOrientation);
			b.setEnabled(false);
			b.setClickable(false);
		}
	}

	private void disableSetupClickListeners() {
		GridLayout gridlayout = (GridLayout) findViewById(R.id.myBoardLayout);
		for (int i = 0; i < gridlayout.getChildCount(); i++) {
			gridlayout.getChildAt(i).setOnClickListener(null);
			gridlayout.getChildAt(i).setClickable(false);
		}
	}

	//==================Activity Related=====================//
	
	@Override
	// when child activities return, we handle it here
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		if (requestCode == 1) {
			Toast.makeText(this, "Loading completed...", Toast.LENGTH_SHORT)
					.show();
			crossfadePanels();
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.main, menu);
		return super.onCreateOptionsMenu(menu);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case android.R.id.home:
			// This ID represents the Home or Up button. In the case of this
			// activity, the Up button is shown.
			NavUtils.navigateUpFromSameTask(this);
			return true;
		default:
			return super.onOptionsItemSelected(item);
		}
	}
	
	//================Button OnClick Handlers==========//
	
	// TODO: Send fire coordinates to Middleman
	public void send_fire_command(View view) {
		// send
		// reset coordinates
		fire_coordinates = null;
		// unhighlight the tile
	}
	
	// onClick handler for the Change Orientation button
	// Changes the orientation of the ships that are to be created
	public void changeOrientation(View view) {
		switch (this.setupOrientation) {
		// TODO: Change the image of the ship selector button to reflect this
		// change
		case HORIZONTAL:
			setupOrientation = ShipOrientation.VERTICAL;
			break;
		case VERTICAL:
			setupOrientation = ShipOrientation.HORIZONTAL;
			break;
		}
	}
	
	// onClick handler for the Ready button
	// Loads the LoadingScreen activity (to wait for other player)
	public void setupComplete(View view) {
		// disable clicking for the gameboard
		disableSetupClickListeners();
		// TODO: send signal to Middleman
		Intent intent = new Intent(this, LoadingScreen.class);
		// we want to handle its return
		startActivityForResult(intent, 1);
	}
	
	//================Animations related==============//
	
	private void crossfadePanels() {
		// alpha 0 means it is transparent
		playPanel.setAlpha(0f);
		playPanel.setVisibility(View.VISIBLE);

		// animate (change alpha to 1) over the animation duration
		playPanel.animate().alpha(1f).setDuration(crossfadeAnimationDuration)
				.setListener(null);

		setupPanel.animate().alpha(0f).setDuration(crossfadeAnimationDuration)
				.setListener(new AnimatorListenerAdapter() {
					@Override
					public void onAnimationEnd(Animator animation) {
						setupPanel.setVisibility(View.GONE);
					}
				});
	}
	
	private void clickGrid(View view) {
		ImageView iv = (ImageView) view;
		if (iv.getParent() == findViewById(R.id.myBoardLayout)) {
			if (shipOnHand != null) {
				int index = myBoardLayout.indexOfChild(iv);
				if (gameBoard.insertIntoGameBoard(shipOnHand,
						indexToCoordinates(index))) {
					drawShipOnMap(shipOnHand, index);
					disableSelectShipButton(shipOnHand);
					isBoardReady();
				}
				shipOnHand = null;
			}
		} else if (iv.getParent() == findViewById(R.id.fogBoardLayout)) {
			// click, store the coordinates of the selected tile
			int index = fogBoardLayout.indexOfChild(iv);
			fire_coordinates = indexToCoordinates(index);
			Toast.makeText(getApplicationContext(), "Clicked",
					Toast.LENGTH_SHORT).show();
			// TODO: highlight this tile
		}
	}

	private void swipeLeftRight() {
		// in from left
		setViewFlipperAnimation(R.anim.in_left, R.anim.out_right);
		viewflipper.showNext();
	}

	private void swipeRightLeft() {
		// in from right
		setViewFlipperAnimation(R.anim.in_right, R.anim.out_left);
		viewflipper.showPrevious();
	}

	private void setViewFlipperAnimation(int resource_in, int resource_out) {
		viewflipper.setInAnimation(this, resource_in);
		viewflipper.setOutAnimation(this, resource_out);
	}

	// onTouchListener for the Game Map
	class GameMapOnTouchListener implements OnTouchListener {
		@Override
		public boolean onTouch(View v, MotionEvent event) {
			// TODO Auto-generated method stub
			if (event.getAction() == MotionEvent.ACTION_DOWN) {
				// touch
				previous_x_position = event.getX();
			}
			if (event.getAction() == MotionEvent.ACTION_UP) {
				// click
				current_x_position = event.getX();
				if (current_x_position == previous_x_position) {
					clickGrid(v);
				} else if (current_x_position > previous_x_position) {
					swipeLeftRight();
				} else if (current_x_position < previous_x_position) {
					swipeRightLeft();
				}
			}
			return true;
		}
	}
}


	

