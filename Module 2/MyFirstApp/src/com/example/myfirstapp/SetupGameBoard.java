package com.example.myfirstapp;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import java.util.Timer;
import java.util.TimerTask;

import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Activity;
import android.content.Intent;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.TransitionDrawable;
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
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewFlipper;

import com.example.models.*;
import com.example.models.Battleship.ShipOrientation;

public class SetupGameBoard extends Activity {

	private View setupPanel;
	private View playPanel;
	private GridLayout myBoardLayout;
	private GridLayout fogBoardLayout;

	private final Handler longPressHandler = new Handler();
	private LongPressRunnable gridLongPress = new LongPressRunnable();
	private boolean isLongPress = false;
	private View longPressView;

	private int crossfadeAnimationDuration;

	private ViewFlipper viewflipper;
	private GameMapOnTouchListener mapTouchListener;
	private float previous_x_position;
	private float current_x_position;

	private Battleship shipOnHand;
	private GameBoard gameBoard;

	private ShipOrientation setupOrientation = ShipOrientation.HORIZONTAL;

	private boolean gameIsPlaying = false;
	private Button fire_button;
	private TextView status_bar;
	private int[] fire_coordinates;
	private AnimationDrawable selected_fog_cell;

	private boolean single_player_mode = false;
	private boolean myTurn = false;
	
	// Set up a timer task. We will use the timer to check the
	// input queue every 500 ms
	ServerMessageHandler smh = new ServerMessageHandler();
	Timer tcp_timer = new Timer();

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
		fire_button = (Button) findViewById(R.id.fire_button);
		status_bar = (TextView) findViewById(R.id.status_bar);

		// reference the map layouts
		fogBoardLayout = (GridLayout) findViewById(R.id.fogBoardLayout);
		myBoardLayout = (GridLayout) findViewById(R.id.myBoardLayout);

		// initially hide this play panel
		playPanel.setVisibility(View.GONE);

		// set the animation time for crossfade
		crossfadeAnimationDuration = getResources().getInteger(
				android.R.integer.config_mediumAnimTime);

		// after all initialization, we draw the map
		drawGameBoard();
		drawFogBoard();
		
		Intent intent = getIntent();
		String message = intent.getStringExtra(MainActivity.EXTRA_MESSAGE);
		if (message != null && message.equals("SINGLE"))
		{
			single_player_mode = true;
		}
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
					imageView.setBackgroundResource(R.drawable.ship_cell);
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

	private void drawShipOnMap(Battleship ship, int index) {
		switch (ship.getOrientation()) {
		case HORIZONTAL:
			for (int i = 0; i < ship.getSize(); i++) {
				ImageView iv = (ImageView) myBoardLayout.getChildAt(index + i
						* GameBoard.getBoardHeight());
				iv.setBackgroundResource(R.drawable.ship_cell);
				TransitionDrawable td = (TransitionDrawable) iv.getBackground();
				td.startTransition(crossfadeAnimationDuration);
			}
			break;
		case VERTICAL:
			for (int i = 0; i < ship.getSize(); i++) {
				ImageView iv = (ImageView) myBoardLayout.getChildAt(index + i);
				iv.setBackgroundResource(R.drawable.ship_cell);
				TransitionDrawable td = (TransitionDrawable) iv.getBackground();
				td.startTransition(crossfadeAnimationDuration);
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
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		if (requestCode == 1) {
			//we need to schedule to read from middleman in multiplayer
			if (single_player_mode == false)
			{
				tcp_timer.schedule(smh, 0, 500);
			}
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

	// ================Button OnClick Handlers==========//

	// TODO: Send fire coordinates to Middleman
	public void send_fire_command(View view) {
		// disable fire button
		myTurn = false;
		disableFireButton();
		// unhighlight the tile
		stopAnimateSelectedGridCell();
		// update status abr
		status_bar.setText("Fired at (" + Integer.toString(fire_coordinates[0]) 
				+ "," + Integer.toString(fire_coordinates[1]) + ")");
		// Multiplayer mode: send to DE2 board 
		if (single_player_mode == false)
		{
			send_message("F" + Integer.toString(fire_coordinates[0])
					+ Integer.toString(fire_coordinates[1]));
		}
		// Singleplayer mode: process command locally
		else 
		{
			//TODO: Single player mode implementation
		}
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
		// Build ready signal 
		String setupMsg = "R";
		// Append ship positions to signal
		for (int i = 0; i < 4; i++)
		{
			String size = Integer.toString(gameBoard.
					getShipOnBoard().get(i).getSize());
			String orientation = gameBoard.
					getShipOnBoard().get(i).getOrientation().toString();
			String x_pos = Integer.toString(gameBoard.
					getShipOnBoardCoor().get(i)[0]);
			String y_pos = Integer.toString(gameBoard. 
					getShipOnBoardCoor().get(i)[1]);
			setupMsg = setupMsg + size + orientation + x_pos + y_pos;
		}
		//Multiplayer: send the signal to middleman
		if (single_player_mode == false)
		{
			send_message(setupMsg);
	
			// start Loading activity to wait for response
			Intent intent = new Intent(this, LoadingScreen.class);
			// we want to handle its return
			startActivityForResult(intent, 1);
		}
		//Singleplayer: setup AI locally
		else 
		{
			//TODO: Single player setup
		}
	}

	public void send_message(String msg) {
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
			if (myTurn == true) {
				animateSelectedGridCell(iv);
				int index = fogBoardLayout.indexOfChild(iv);
				fire_coordinates = indexToCoordinates(index);
				enableFireButton();
			}

		}
	}

	private void animateSelectedGridCell(ImageView iv) {
		if (selected_fog_cell != null) {
			selected_fog_cell.stop();
			selected_fog_cell.selectDrawable(0);
		}

		selected_fog_cell = (AnimationDrawable) iv.getDrawable();
		selected_fog_cell.setExitFadeDuration(crossfadeAnimationDuration);
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
				.setDuration(crossfadeAnimationDuration).setListener(null);
		viewflipper.showNext();
		viewflipper.getCurrentView().animate().alpha(1f)
				.setDuration(crossfadeAnimationDuration).setListener(null);
	}

	private void swipeRightLeft() {
		// in from right
		setViewFlipperAnimation(R.anim.in_right, R.anim.out_left);
		viewflipper.getCurrentView().animate().alpha(0f)
				.setDuration(crossfadeAnimationDuration).setListener(null);
		viewflipper.showNext();
		viewflipper.getCurrentView().animate().alpha(1f)
				.setDuration(crossfadeAnimationDuration).setListener(null);
	}

	private void setViewFlipperAnimation(int resource_in, int resource_out) {
		viewflipper.setInAnimation(this, resource_in);
		viewflipper.setOutAnimation(this, resource_out);
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
					longPressHandler.postDelayed(gridLongPress, 1000);
				}
			}
			if (event.getAction() == MotionEvent.ACTION_MOVE) {
				longPressHandler.removeCallbacks(gridLongPress);
			}
			if (event.getAction() == MotionEvent.ACTION_UP) {
				// click
				longPressHandler.removeCallbacks(gridLongPress);
				current_x_position = event.getX();
				// If this click was a long press, then it was already handled
				// by the Runnable
				// Therefore, we can skip this as we don't want to handle it
				// again.
				if (isLongPress == true) {
					// we reset the flag because the long press has now been
					// handled.
					isLongPress = false;
				}
				// If this wasn't a long press, then we want to handle it here.
				else {
					if (current_x_position == previous_x_position) {
						clickGrid(v);
					} else if (current_x_position > previous_x_position) {
						if (gameIsPlaying)
							swipeLeftRight();
					} else if (current_x_position < previous_x_position) {
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
					TransitionDrawable td = (TransitionDrawable) iv
							.getBackground();
					td.reverseTransition(crossfadeAnimationDuration);
				}
				break;
			case VERTICAL:
				for (int i = 0; i < size; i++) {
					ImageView iv = (ImageView) myBoardLayout.getChildAt(index
							+ i);
					TransitionDrawable td = (TransitionDrawable) iv
							.getBackground();
					td.reverseTransition(crossfadeAnimationDuration);
				}
				break;
			}
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

						final String msgReceived = new String(buf, 0, bytes_avail, "US-ASCII");
	
						// GUI can not be updated in an asyncrhonous task.  
						// So, update the GUI using the UI thread.
						runOnUiThread(new Runnable() {
							public void run() {
								Toast.makeText(app, msgReceived, Toast.LENGTH_SHORT).show();
								if (msgReceived.equals("T"))
								{
									myTurn = true;
									if (viewflipper.getDisplayedChild() == 0)
									{
										//go to the fog board when it is my turn
										swipeRightLeft();
										status_bar.setText("Your Turn! Pick a tile.");
									}
								}
								// acknowledge 
								send_message("A");
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
