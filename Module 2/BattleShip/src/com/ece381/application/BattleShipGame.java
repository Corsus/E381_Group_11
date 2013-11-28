package com.ece381.application;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.TransitionDrawable;
import android.support.v4.app.NavUtils;
import android.view.LayoutInflater;
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
import android.widget.RatingBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewFlipper;

import com.ece381.controller.ComputerPlayer;
import com.ece381.models.*;
import com.ece381.models.Battleship.ShipOrientation;
import com.ece381.application.R;

public class BattleShipGame extends Activity {

	public final static String EXTRA_MESSAGE = "com.ece381.application.MESSAGE";

	private View setupPanel;
	private View playPanel;
	private GridLayout myBoardLayout;
	private GridLayout fogBoardLayout;

	private final Handler longPressHandler = new Handler();
	private LongPressRunnable gridLongPress = new LongPressRunnable();
	private boolean isLongPress = false;
	private View longPressView;

	private int crossfadeAnimationShort;
	private int crossfadeAnimationMedium;
	private int crossfadeAnimationLong;

	private ViewFlipper viewflipper;
	private GameMapOnTouchListener mapTouchListener;
	private float previous_x_position;
	private float current_x_position;

	private Battleship shipOnHand;
	private GameBoard gameBoard;

	private ShipOrientation setupOrientation = ShipOrientation.VERTICAL;

	private boolean gameIsPlaying = false;
	private Button fire_button;
	private TextView status_bar;
	private TextView update_bar;
	private RatingBar hp_bar;
	private int[] fire_coordinates;
	private AnimationDrawable selected_fog_cell;

	private boolean myTurn = false;
	private Object acknowledgementWaiter = new Object();
	private String messageToSend;

	private ScheduledExecutorService resend_service;
	
	// Set up a timer task. We will use the timer to check the
	// input queue every 500 ms
	private ServerMessageHandler smh = new ServerMessageHandler();
	private Timer tcp_timer = new Timer();

	// set up timer task for acknowledgement
	private ResendMessageTask rmt = new ResendMessageTask();
	//private Timer resend_timer = new Timer();
	
	//sound components
	private SoundPool sp;
	private int[] soundIds;

	//single player components
	private boolean single_player_mode = false;
	private ComputerPlayer aiPlayer;
	private int single_player_hp = 10;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_game_board);

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
		fire_button = (Button) findViewById(R.id.fire_button);
		status_bar = (TextView) findViewById(R.id.status_bar);
		update_bar = (TextView) findViewById(R.id.update_bar);
		hp_bar = (RatingBar) findViewById(R.id.hp_bar);

		// reference the map layouts
		fogBoardLayout = (GridLayout) findViewById(R.id.fogBoardLayout);
		myBoardLayout = (GridLayout) findViewById(R.id.myBoardLayout);

		// initially hide this play panel
		playPanel.setVisibility(View.GONE);

		// set the animation time for crossfade
		crossfadeAnimationShort = getResources().getInteger(
				android.R.integer.config_shortAnimTime);
		crossfadeAnimationMedium = getResources().getInteger(
				android.R.integer.config_mediumAnimTime);
		crossfadeAnimationLong = getResources().getInteger(
				android.R.integer.config_longAnimTime);

		// after all initialization, we draw the map
		drawGameBoard();
		drawFogBoard();

		Intent intent = getIntent();
		String message = intent.getStringExtra(MainActivity.EXTRA_MESSAGE);
		if (message != null && message.equals("SINGLE")) {
			single_player_mode = true;
			getActionBar().setTitle(R.string.title_activity_game_versus_ai);
		}
		else
		{
			single_player_mode = false;
			getActionBar().setTitle(R.string.title_activity_game_versus_player);
		}
		
		setUpSound();
	}

	// ==================Layout Related================//

	private void drawGameBoard() {		
		Tile theTile;
		ImageView imageView;
		for (int i = 0; i < GameBoard.getBoardWidth(); i++) {
			for (int j = 0; j < GameBoard.getBoardHeight(); j++) {
				theTile = this.gameBoard.getTileAt(i, j);
				if (theTile.getTileObject() == null) {
					imageView = new ImageView(this);
					imageView.setImageResource(R.drawable.grid_border);
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
				imageView.setImageResource(R.drawable.selectable_grid);
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
	
	private void animateDrawShip(AnimationDrawable ad, int frameIndex)
	{
		ad.setEnterFadeDuration(crossfadeAnimationMedium);
		ad.setExitFadeDuration(crossfadeAnimationMedium);
		ad.selectDrawable(frameIndex);
	}

	private void drawShipOnMap(Battleship ship, int index) {
		switch (ship.getSize()) {
		case 1: // SHIP 1
			switch (ship.getOrientation()) {
			case HORIZONTAL: {
				ImageView iv = (ImageView) myBoardLayout.getChildAt(index);
				iv.setBackgroundResource(R.drawable.ship_cell);
				animateDrawShip((AnimationDrawable) iv.getBackground(), 1);
			}
				break;
			case VERTICAL: {
				ImageView iv = (ImageView) myBoardLayout.getChildAt(index);
				iv.setBackgroundResource(R.drawable.ship_cell);
				animateDrawShip((AnimationDrawable) iv.getBackground(), 11);
			}
				break;
			}
			break;

		case 2: // SHIP 2
			switch (ship.getOrientation()) {
			case HORIZONTAL:
				for (int i = 0; i < ship.getSize(); i++) {
					switch (i) {
					case 0: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i
										* GameBoard.getBoardHeight());
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 2);
					}
						break;
					case 1: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i
										* GameBoard.getBoardHeight());
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 3);

					}
						break;
					}
				}
				break;
			case VERTICAL:
				for (int i = 0; i < ship.getSize(); i++) {
					switch (i) {
					case 0: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i);
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 12);
					}
						break;
					case 1: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i);
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 13);
					}
						break;
					}
				}
				break;
			}
			break;

		case 3: // SHIP 3
			switch (ship.getOrientation()) {
			case HORIZONTAL:
				for (int i = 0; i < ship.getSize(); i++) {
					switch (i) {
					case 0: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i
										* GameBoard.getBoardHeight());
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 4);
					}
						break;
					case 1: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i
										* GameBoard.getBoardHeight());
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 5);
					}
						break;
					case 2: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i
										* GameBoard.getBoardHeight());
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 6);
					}
						break;
					}
				}
				break;
			case VERTICAL:
				for (int i = 0; i < ship.getSize(); i++) {
					switch (i) {
					case 0: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i);
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 14);
					}
						break;
					case 1: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i);
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 15);
					}
						break;
					case 2: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i);
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 16);
					}
						break;
					}
				}
				break;
			}
			break;

		case 4: // SHIP 4
			switch (ship.getOrientation()) {
			case HORIZONTAL:
				for (int i = 0; i < ship.getSize(); i++) {
					switch (i) {
					case 0: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i
										* GameBoard.getBoardHeight());
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 7);
					}
						break;
					case 1: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i
										* GameBoard.getBoardHeight());
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 8);
					}
						break;
					case 2: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i
										* GameBoard.getBoardHeight());
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 9);
					}
						break;
					case 3: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i
										* GameBoard.getBoardHeight());
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 10);
					}
						break;
					}
				}
				break;
			case VERTICAL:
				for (int i = 0; i < ship.getSize(); i++) {
					switch (i) {
					case 0: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i);
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 17);
					}
						break;
					case 1: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i);
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 18);
					}
						break;
					case 2: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i);
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 19);
					}
						break;
					case 3: {
						ImageView iv = (ImageView) myBoardLayout
								.getChildAt(index + i);
						iv.setBackgroundResource(R.drawable.ship_cell);
						animateDrawShip((AnimationDrawable) iv.getBackground(), 20);
					}
						break;
					}
				}
				break;
			}
			break;
		}
	}

	private void enableFireButton() {
		if (selected_fog_cell != null) {
			fire_button.setEnabled(true);
			fire_button.setClickable(true);
		}
	}

	private void disableFireButton() {
		fire_button.setEnabled(false);
		fire_button.setClickable(false);
	}

	// =================Game Logic Related========================//

	// onClick handler for the 4 ship selector buttons
	public void selectShipOnClick(View view) {
		
		sp.play(soundIds[0], 100, 100, 1, 0, 1f);
		
		if (shipOnHand != null)
		{
			// reenable that button
			enableSelectShipButton(shipOnHand.getSize());
		}
		
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
		disableSelectShipButton(shipOnHand);
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

	// Re-enables the select ship button
	// Input: size of the ship, so that it can figure out which button to
	// reenable
	private void enableSelectShipButton(int size) {
		ImageButton ib = null;
		switch (size) {
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
			ib.setEnabled(true);
			ib.setClickable(true);
		}
	}

	// Check to see if the setup is complete.
	// If so, enables the ready button.
	// Called after a ship has been placed on the board.
	private void isBoardReady() {
		Button b1 = (Button) findViewById(R.id.readyButton);
		Button b2 = (Button) findViewById(R.id.changeOrientation);
		if (this.gameBoard.getShipOnBoard().size() == 4) {
			b1.setEnabled(true);
			b1.setClickable(true);
			b2.setEnabled(false);
			b2.setClickable(false);
		} else {
			b1.setEnabled(false);
			b1.setClickable(false);
			b2.setEnabled(true);
			b2.setClickable(true);
		}
	}

	private void disableSetupClickListeners() {
		GridLayout gridlayout = (GridLayout) findViewById(R.id.myBoardLayout);
		for (int i = 0; i < gridlayout.getChildCount(); i++) {
			gridlayout.getChildAt(i).setOnClickListener(null);
			gridlayout.getChildAt(i).setClickable(false);
		}
	}

	// ==================Activity Related=====================//

	@Override
	// when child activities return, we handle it here
	// Note: This method will only be invoked in Multiplayer mode...
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		if (requestCode == 1) {
			// send acknowledgement to middleman
			send_message("A");
			// we need to schedule to read from middleman in multiplayer
			tcp_timer.schedule(smh, 0, 500);
			Toast.makeText(this, "Loading Completed...", Toast.LENGTH_SHORT).show();
			gameIsPlaying = true;
			crossfadePanels();
			disableFireButton();
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
	
	@Override
	public void onBackPressed() {
		// only allow leaving a single player game 
		if (single_player_mode)
		{
			LayoutInflater li = LayoutInflater.from(this);
			View confirmationPrompt = li.inflate(R.layout.confirmation_prompt, null);
			
			AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);
			alertDialogBuilder.setView(confirmationPrompt);
			
			alertDialogBuilder
				.setCancelable(false)
				.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						BattleShipGame.super.onBackPressed();
					}
				})
				.setNegativeButton("No", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						// TODO Auto-generated method stub
						dialog.cancel();
					}
				});
			AlertDialog alertDialog = alertDialogBuilder.create();
			alertDialog.show();
		}
		Toast.makeText(this, "You can't leave a multiplayer game.", Toast.LENGTH_SHORT).show();
		//there is no support for leaving a multiplayer game
	}
	
	// ================Button OnClick Handlers==========//

	public void send_fire_command(View view) {
		
		sp.play(soundIds[4], 100, 100, 1, 0, 1f);
		// disable fire button
		myTurn = false;
		disableFireButton();
		// unhighlight the tile
		stopAnimateSelectedGridCell();
		// update status bar
		changeStatusBar(status_bar, 1, "You fired at (" + Integer.toString(fire_coordinates[0])
				+ "," + Integer.toString(fire_coordinates[1]) + ")!");
		// Multiplayer mode: send to DE2 board
		if (single_player_mode == false) {
			sendAndWaitForAck("F" + Integer.toString(fire_coordinates[0])
					+ Integer.toString(fire_coordinates[1]));
		}
		// Singleplayer mode: process command locally
		else {
			// TODO: Single player mode implementation
			myTurn = true;
			//fire at the computer
			fireAtComputer(fire_coordinates);
			if (aiPlayer.getHP() == 0)
			{
				//we win
				Intent intent = new Intent(BattleShipGame.this,	WinnerScreen.class);
				intent.putExtra(EXTRA_MESSAGE, "WIN");
				startActivity(intent);
			}
			//computer fires back at you
			handleResponseFromAi();
		}
	}
	
	//accepts an array of size 2 that represents the x,y coordinates
	private void fireAtComputer(int[] coordinates)
	{
		if (aiPlayer.receiveFireCommand(coordinates))
		{
			//hit :)
			ImageView iv = (ImageView) fogBoardLayout
					.getChildAt(coordinates[0]
							* GameBoard
									.getBoardHeight()
							+ coordinates[1]);
			drawHitCell(iv);
		}
		else
		{
			//missed :(
			ImageView iv = (ImageView) fogBoardLayout
					.getChildAt(coordinates[0]
							* GameBoard
									.getBoardHeight()
							+ coordinates[1]);
			drawMissCell(iv);
		}
	}

	private void handleResponseFromAi()
	{
		int[] coordinates = aiPlayer.computerFireCoordinates();
		WorldObject wo = this.gameBoard.getTileAt(
				coordinates[0], coordinates[1]).getTileObject();
		if (wo != null)
		{
			//got hit
			ImageView iv = (ImageView) myBoardLayout.getChildAt(
					coordinates[0]* GameBoard.getBoardHeight()
							+ coordinates[1]);
			drawHitCell(iv);
			
			changeStatusBar(update_bar, 2, "Your opponent hit you at (" + Integer.toString(coordinates[0]) 
					+ ", " + Integer.toString(coordinates[1]) + ")!");
			
			gameBoard.killTileAt(coordinates[0], coordinates[1]);
			hp_bar.setRating(gameBoard.getHP());
			
			//need to inform ai
			aiPlayer.updateHitMap(coordinates);
			//update target list
			aiPlayer.updateTargetList(coordinates);
			//update player HP
			if (--single_player_hp == 0)
			{
				//you lost
				Intent intent = new Intent(BattleShipGame.this,	WinnerScreen.class);
				intent.putExtra(EXTRA_MESSAGE, "LOSE");
				startActivity(intent);
			}
		}
		else
		{
			//didn't get hit
			ImageView iv = (ImageView) myBoardLayout.getChildAt(
					coordinates[0]* GameBoard.getBoardHeight()
							+ coordinates[1]);
			drawMissCell(iv);
			
			changeStatusBar(update_bar, 1, "Your opponent missed at (" + Integer.toString(coordinates[0]) 
					+ ", " + Integer.toString(coordinates[1]) + ")!");

			//need to inform ai
			aiPlayer.updateHitMap(coordinates);
		}
	}
	
	// onClick handler for the Change Orientation button
	// Changes the orientation of the ships that are to be created
	public void changeOrientation(View view) {
		
		sp.play(soundIds[2], 100, 100, 1, 0, 1f);
		
		switch (this.setupOrientation) {
		case HORIZONTAL: {
			ImageButton scout = (ImageButton) findViewById(R.id.SelectScout);
			scout.setImageResource(R.drawable.ship_1_small);
			ImageButton cruiser = (ImageButton) findViewById(R.id.SelectCruiser);
			cruiser.setImageResource(R.drawable.ship_2_small);
			ImageButton destroyer = (ImageButton) findViewById(R.id.SelectDestroyer);
			destroyer.setImageResource(R.drawable.ship_3_small);
			ImageButton mothership = (ImageButton) findViewById(R.id.SelectMothership);
			mothership.setImageResource(R.drawable.ship_4_small);
			setupOrientation = ShipOrientation.VERTICAL;
		}
			break;
		case VERTICAL: {
			ImageButton scout = (ImageButton) findViewById(R.id.SelectScout);
			scout.setImageResource(R.drawable.ship_1_small_h);
			ImageButton cruiser = (ImageButton) findViewById(R.id.SelectCruiser);
			cruiser.setImageResource(R.drawable.ship_2_small_h);
			ImageButton destroyer = (ImageButton) findViewById(R.id.SelectDestroyer);
			destroyer.setImageResource(R.drawable.ship_3_small_h);
			ImageButton mothership = (ImageButton) findViewById(R.id.SelectMothership);
			mothership.setImageResource(R.drawable.ship_4_small_h);
			setupOrientation = ShipOrientation.HORIZONTAL;
		}
			break;
		}
		
		if(this.shipOnHand != null)
		{
			switch(this.shipOnHand.getOrientation())
			{
			case VERTICAL:
			{
				shipOnHand.setOrientation(ShipOrientation.HORIZONTAL);
			}
			break;
			case HORIZONTAL:
			{
				shipOnHand.setOrientation(ShipOrientation.VERTICAL);
			}
			break;
			}
		}
	}

	// onClick handler for the Ready button
	// Loads the LoadingScreen activity (to wait for other player)
	public void setupComplete(View view) {
		
		sp.play(soundIds[3], 100, 100, 1, 0, 1f);
		// disable clicking for the gameboard
		disableSetupClickListeners();
		// setup hp bar
		hp_bar.setRating(gameBoard.getHP());
		// Build ready signal
		String setupMsg = "R";
		// Append ship positions to signal
		for (int i = 0; i < 4; i++) {
			String size = Integer.toString(gameBoard.getShipOnBoard().get(i)
					.getSize());
			String orientation = gameBoard.getShipOnBoard().get(i)
					.getOrientation().toString();
			String x_pos = Integer.toString(gameBoard.getShipOnBoardCoor().get(
					i)[0]);
			String y_pos = Integer.toString(gameBoard.getShipOnBoardCoor().get(
					i)[1]);
			setupMsg = setupMsg + size + orientation + x_pos + y_pos;
		}
		// Multiplayer: send the signal to middleman
		if (single_player_mode == false) {
			send_message(setupMsg);
			// start Loading activity to wait for response
			Intent intent = new Intent(this, LoadingScreen.class);
			// we want to handle its return
			startActivityForResult(intent, 1);
		}
		// Singleplayer: setup AI locally
		else 
		{
			//create a computer opponent
			aiPlayer = new ComputerPlayer();
			//signal to start the game
			gameIsPlaying = true;
			crossfadePanels();
			disableFireButton();
			myTurn = true;
			//move into the next panel right away
			swipeRightLeft();
			changeStatusBar(status_bar, 0, "Your Turn! Pick a tile.");
		}
	}

	public void sendAndWaitForAck(String msg) {
		send_message(msg);
		resend_service = Executors.newSingleThreadScheduledExecutor();
		resend_service.scheduleAtFixedRate(rmt, 3, 3, TimeUnit.SECONDS);
		try {
			synchronized (acknowledgementWaiter) {
				acknowledgementWaiter.wait();
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		resend_service.shutdown();
	}

	public void send_message(String msg) {
		messageToSend = msg;
		BattleShipApp app = (BattleShipApp) getApplication();

		if (app.sock != null && app.sock.isConnected() && !app.sock.isClosed()) {
			byte buf[] = new byte[msg.length()];
			System.arraycopy(msg.getBytes(), 0, buf, 0, msg.length());

			// Now send through the output stream of the socket

			OutputStream out;
			try {
				out = app.sock.getOutputStream();
				try {
					out.write(buf, 0, msg.length());
				} catch (IOException e) {
					e.printStackTrace();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	// ================Animations related==============//

	private void crossfadePanels() {
		// alpha 0 means it is transparent
		playPanel.setAlpha(0f);
		playPanel.setVisibility(View.VISIBLE);

		// animate (change alpha to 1) over the animation duration
		playPanel.animate().alpha(1f).setDuration(crossfadeAnimationMedium)
				.setListener(null);

		setupPanel.animate().alpha(0f).setDuration(crossfadeAnimationMedium)
				.setListener(new AnimatorListenerAdapter() {
					@Override
					public void onAnimationEnd(Animator animation) {
						setupPanel.setVisibility(View.GONE);
					}
				});
	}

	private void clickGrid(View view) {
		
		sp.play(soundIds[1], 100, 100, 1, 0, 1f);
		
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
				else
				{
					enableSelectShipButton(shipOnHand.getSize());
				}
				shipOnHand = null;
			}
		} else if (iv.getParent() == findViewById(R.id.fogBoardLayout)) {
			// click, store the coordinates of the selected tile
			if (myTurn == true)
			{
				if (iv.getDrawable() instanceof AnimationDrawable)
				{
					//TODO: if view shows hit_cell or miss_cell, we skip this
					animateSelectedGridCell(iv);
					int index = fogBoardLayout.indexOfChild(iv);
					fire_coordinates = indexToCoordinates(index);
					enableFireButton();
				}
			}
		}
	}

	private void animateSelectedGridCell(ImageView iv) {
		if (selected_fog_cell != null) {
			selected_fog_cell.stop();
			selected_fog_cell.selectDrawable(0);
		}

		selected_fog_cell = (AnimationDrawable) iv.getDrawable();
		selected_fog_cell.setEnterFadeDuration(crossfadeAnimationMedium);
		selected_fog_cell.setExitFadeDuration(crossfadeAnimationMedium);
		selected_fog_cell.start();
	}

	private void stopAnimateSelectedGridCell() {
		if (selected_fog_cell != null) {
			selected_fog_cell.stop();
			selected_fog_cell.selectDrawable(0);
			selected_fog_cell = null;
		}
	}

	private void swipeLeftRight() {
		// in from left
		setViewFlipperAnimation(R.anim.in_left, R.anim.out_right);
		viewflipper.getCurrentView().animate().alpha(0f)
				.setDuration(crossfadeAnimationShort).setListener(null);
		viewflipper.showNext();
		viewflipper.getCurrentView().animate().alpha(1f)
				.setDuration(crossfadeAnimationShort).setListener(null);
	}

	private void swipeRightLeft() {
		// in from right
		setViewFlipperAnimation(R.anim.in_right, R.anim.out_left);
		viewflipper.getCurrentView().animate().alpha(0f)
				.setDuration(crossfadeAnimationShort).setListener(null);
		viewflipper.showNext();
		viewflipper.getCurrentView().animate().alpha(1f)
				.setDuration(crossfadeAnimationShort).setListener(null);
	}

	private void setViewFlipperAnimation(int resource_in, int resource_out) {
		sp.play(soundIds[3], 100, 100, 1, 0, 1f);
		viewflipper.setInAnimation(this, resource_in);
		viewflipper.setOutAnimation(this, resource_out);
	}
	
	private void drawHitCell(ImageView iv)
	{
		iv.setImageResource(R.drawable.hit_animation);
		TransitionDrawable td = (TransitionDrawable) iv.getDrawable();
		td.startTransition(crossfadeAnimationLong);
	}
	
	private void drawMissCell(ImageView iv)
	{
		iv.setImageResource(R.drawable.miss_animation);
		TransitionDrawable td = (TransitionDrawable) iv.getDrawable();
		td.startTransition(crossfadeAnimationLong);
	}
	
	private void changeStatusBar(TextView tv, int colorIndex, String s)
	{
		AnimationDrawable ad = (AnimationDrawable) tv.getBackground();
		ad.setEnterFadeDuration(crossfadeAnimationShort);
		ad.setExitFadeDuration(crossfadeAnimationShort);
		ad.selectDrawable(colorIndex);
		tv.setText(s);
	}
	
	
	//load all the sound files
	private void setUpSound() {
		// Set the hardware buttons to control the sound
		this.setVolumeControlStream(AudioManager.STREAM_MUSIC);
				
		sp = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
		soundIds = new int[7];
		soundIds[0] = sp.load(this, R.raw.click_ship, 1);
		soundIds[1] = sp.load(this, R.raw.click_grid, 1);
		soundIds[2] = sp.load(this, R.raw.orientation, 1);
		soundIds[3] = sp.load(this, R.raw.ready, 1);
		soundIds[4] = sp.load(this, R.raw.fire, 1);
		soundIds[5] = sp.load(this, R.raw.turn, 1);
		soundIds[6] = sp.load(this, R.raw.hit, 1);	
	}


	// onTouchListener for the Game Map
	class GameMapOnTouchListener implements OnTouchListener {
		@Override
		public boolean onTouch(View v, MotionEvent event) {
			if (event.getAction() == MotionEvent.ACTION_DOWN) {
				// touch
				previous_x_position = event.getX();
				if (shipOnHand == null && gameIsPlaying == false) {
					// need to keep track of view being long pressed
					longPressView = v;
					longPressHandler.postDelayed(gridLongPress, 500);
				}
			}
			if (event.getAction() == MotionEvent.ACTION_UP) {
				// click
				longPressHandler.removeCallbacks(gridLongPress);
				current_x_position = event.getX();
				// If this click was a long press, then it was already handled
				// by the Runnable
				// Therefore, we can skip this as we don't want to handle it
				// again..
				if (isLongPress == true) {
					// we reset the flag because the long press has now been
					// handled.
					isLongPress = false;
				}
				// If this wasn't a long press, then we want to handle it here.
				else {
					if (Math.abs(current_x_position - previous_x_position) <= 100) {
						clickGrid(v);
					} else if (current_x_position - previous_x_position > 100) {
						if (gameIsPlaying)
							swipeLeftRight();
					} else if (previous_x_position - current_x_position > 100) {
						if (gameIsPlaying)
							swipeRightLeft();
					}
				}
			}
			return true;
		}
	}

	class LongPressRunnable implements Runnable {
		@Override
		public void run() {

			if (longPressView instanceof ImageView) {
				if (longPressView.getParent() == findViewById(R.id.myBoardLayout)) {
					// get index of this cell
					int index = myBoardLayout.indexOfChild(longPressView);
					// convert to matrix coordinates
					int[] coordinates = indexToCoordinates(index);
					// get the tile at that coordinate
					Tile tile = gameBoard.getTileAt(coordinates[0],
							coordinates[1]);
					// check to see if this is actually a ship component
					if (tile.getTileObject() != null
							&& tile.getTileObject() instanceof ShipComponent) {
						// if yes, we find the parent ship of this component
						ShipComponent sc = (ShipComponent) tile.getTileObject();
						Battleship shipToRemove = sc.getParent();
						// we want to get need to get coordinates of the HEAD of
						// the parent ship
						int shipIndex = gameBoard.getShipOnBoard().indexOf(
								shipToRemove);
						coordinates = gameBoard.getShipOnBoardCoor().get(
								shipIndex);
						// remove this ship from the underlying board
						removeShipComponentFromMatrix(shipToRemove.getSize(),
								shipToRemove.getOrientation(), coordinates);
						// undraw this ship from the layout using the
						// coordinates
						undrawShip(shipToRemove.getSize(),
								shipToRemove.getOrientation(), coordinates[0]
										* GameBoard.getBoardHeight()
										+ coordinates[1]);
						// re-enable the corresponding spawn ship button
						enableSelectShipButton(shipToRemove.getSize());
						// finally, get rid of this ship from the board
						gameBoard.removeShipFromBoard(shipIndex);
						// check once again if board is ready
						isBoardReady();
					}
				}
			}

			// set flag
			isLongPress = true;
		}

		private void removeShipComponentFromMatrix(int size,
				ShipOrientation orientation, int[] coor) {
			int x = coor[0];
			int y = coor[1];
			switch (orientation) {
			case HORIZONTAL:
				for (int i = x; i < x + size; i++) {
					gameBoard.getTileAt(i, y).clearTile();
				}
				break;
			case VERTICAL:
				for (int i = y; i < y + size; i++) {
					gameBoard.getTileAt(x, i).clearTile();
				}
				break;
			}
		}

		// helper method to undraw the ship from the board
		// input: size of ship, orientation of ship, and the starting cell index
		// to undraw
		private void undrawShip(int size, ShipOrientation orientation, int index) {
			switch (orientation) {
			case HORIZONTAL:
				for (int i = 0; i < size; i++) {
					ImageView iv = (ImageView) myBoardLayout.getChildAt(index
							+ i * GameBoard.getBoardHeight());
					AnimationDrawable ad = (AnimationDrawable) iv
							.getBackground();
					ad.selectDrawable(0);
				}
				break;
			case VERTICAL:
				for (int i = 0; i < size; i++) {
					ImageView iv = (ImageView) myBoardLayout.getChildAt(index
							+ i);
					AnimationDrawable ad = (AnimationDrawable) iv
							.getBackground();
					ad.selectDrawable(0);
				}
				break;
			}
		}
	}

	public class ResendMessageTask implements Runnable {
		@Override
		public void run() {
			send_message(messageToSend);
		}
	}

	public class ServerMessageHandler extends TimerTask {
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

						final String msgReceived = new String(buf, 0,
								bytes_avail, "US-ASCII");

						// we cannot run this on UI thread, because UI thread is
						// supposed to be waiting...
						if (msgReceived.charAt(0) == 'A') {
							synchronized (acknowledgementWaiter) {
								acknowledgementWaiter.notify();
							}
						} else {
							// GUI can not be updated in an asyncrhonous task.
							// So, update the GUI using the UI thread.
							runOnUiThread(new Runnable() {
								public void run() {
									Toast.makeText(app, msgReceived,
											Toast.LENGTH_SHORT).show();
									if (msgReceived.charAt(0) == 'T') {
										myTurn = true;
										changeStatusBar(status_bar, 0, "Your Turn! Pick a tile.");
										// acknowledge
										send_message("A");
									} else if (msgReceived.charAt(0) == 'Y') {
										ImageView iv = (ImageView) fogBoardLayout
												.getChildAt(fire_coordinates[0]
														* GameBoard
																.getBoardHeight()
														+ fire_coordinates[1]);
										drawHitCell(iv);
										changeStatusBar(status_bar, 0, "Hit at (" + Integer.toString(fire_coordinates[0])
												+ "," + Integer.toString(fire_coordinates[1]) + ")!");
										// acknowledge
										send_message("A");
									} else if (msgReceived.charAt(0) == 'S') {
										ImageView iv = (ImageView) fogBoardLayout
												.getChildAt(fire_coordinates[0]
														* GameBoard
																.getBoardHeight()
														+ fire_coordinates[1]);
										drawHitCell(iv);
										changeStatusBar(status_bar, 0, "You sunk a battleship!");
										// acknowledge
										send_message("A");
									} else if (msgReceived.charAt(0) == 'N') {
										ImageView iv = (ImageView) fogBoardLayout
												.getChildAt(fire_coordinates[0]
														* GameBoard
																.getBoardHeight()
														+ fire_coordinates[1]);
										drawMissCell(iv);
										changeStatusBar(status_bar, 2, "Missed at (" + Integer.toString(fire_coordinates[0])
												+ "," + Integer.toString(fire_coordinates[1]) + ")!");
										// acknowledge
										send_message("A");
									} else if (msgReceived.charAt(0) == 'H'
											&& msgReceived.length() >= 3) {
										int x = msgReceived.charAt(1) - 48;
										int y = msgReceived.charAt(2) - 48;
										ImageView iv = (ImageView) myBoardLayout
												.getChildAt(x
														* GameBoard
																.getBoardHeight()
														+ y);
										drawHitCell(iv);
										changeStatusBar(update_bar, 2, "Your opponent hit you at (" + Integer.toString(x) 
															+ ", " + Integer.toString(y) + ")!");
										
										gameBoard.killTileAt(x, y);
										hp_bar.setRating(gameBoard.getHP());
										
										// acknowledge
										send_message("A");
									} else if (msgReceived.charAt(0) == 'M'
											&& msgReceived.length() >= 3) {
										int x = msgReceived.charAt(1) - 48;
										int y = msgReceived.charAt(2) - 48;
										ImageView iv = (ImageView) myBoardLayout
												.getChildAt(x
														* GameBoard
																.getBoardHeight()
														+ y);
										drawMissCell(iv);
										changeStatusBar(update_bar, 1, "Your opponent missed at (" + Integer.toString(x) 
												+ ", " + Integer.toString(y) + ")!");
										// acknowledge
										send_message("A");
									} else if (msgReceived.charAt(0) == 'W') {
										runOnUiThread(new Runnable() {
											@Override
											public void run() {
												Intent intent = new Intent(
														BattleShipGame.this,
														WinnerScreen.class);
												intent.putExtra(EXTRA_MESSAGE,
														"WIN");
												startActivity(intent);
											}
										});
										// acknowledge
										send_message("A");
									} else if (msgReceived.charAt(0) == 'L') {
										runOnUiThread(new Runnable() {

											@Override
											public void run() {
												Intent intent = new Intent(
														BattleShipGame.this,
														WinnerScreen.class);
												intent.putExtra(EXTRA_MESSAGE,
														"LOSE");
												startActivity(intent);
											}
										});
										// acknowledge
										send_message("A");
									}
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
