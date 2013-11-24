package com.ece381.controller;

import java.util.ArrayList;
import java.util.Arrays;

import com.ece381.application.BattleShipApp;
import com.ece381.models.Battleship;
import com.ece381.models.GameBoard;
import com.ece381.models.Tile;
import com.ece381.models.Battleship.ShipOrientation;

public class ComputerPlayer {

	private GameBoard aiGameBoard;
	private int[][] hitMap; 
	private int[] lastHitLocation;
	private ArrayList<int[]> previousFireCoors;
	private ArrayList<Integer> listOfDirTried;
	
	public ComputerPlayer()
	{
		generateGameBoard();
		generateHitMap();
		lastHitLocation = null;
		previousFireCoors = new ArrayList<int[]>();
		listOfDirTried = new ArrayList<Integer>();
	}

	private void generateGameBoard() {
		// create a new empty game board
		aiGameBoard = new GameBoard();
		// populate this game board with ships
		generateRandomShip(4);
		generateRandomShip(3);
		generateRandomShip(2);
		generateRandomShip(1);
	}
	
	private void generateHitMap()
	{
		hitMap = new int[GameBoard.getBoardWidth()][GameBoard.getBoardHeight()];
	}

	private void generateRandomShip(int size) {
		ShipOrientation orientation;
		int[] coordinates = new int[2];
		
		//keep trying until a ship is inserted
		while (true)
		{
			//randomize orientation
			orientation =(Math.random() < 0.5) ? 
				ShipOrientation.HORIZONTAL : ShipOrientation.VERTICAL;
			//randomize coorindates
			coordinates[0] = (int) (Math.random() * GameBoard.getBoardWidth());
			coordinates[1] = (int) (Math.random() * GameBoard.getBoardHeight());
		
			//create the ship and insert it
			Battleship shipToInsert = new Battleship(size, orientation);	
			if (aiGameBoard.insertIntoGameBoard(shipToInsert, coordinates))
			{
				break;
			}
		}
	}
	
	public int[] computerFireCoordinates()
	{
		int[] fireCoor = new int[2];
		//generate completely random coordinates when computer has no clue
		if (lastHitLocation == null)
		{
			do {
				fireCoor[0] = (int) (Math.random() * GameBoard.getBoardWidth());
				fireCoor[1] = (int) (Math.random() * GameBoard.getBoardHeight());
			}while (checkFireCoor(fireCoor));
		}
		
		//if it previously hit a ship, try firing at the neighbouring locations
		else {
			int dir;		
			
			// if all 4 directions have been tried, fire at a random location 
			if (listOfDirTried.size() == 4) {
				do {
					fireCoor[0] = (int) (Math.random() * GameBoard.getBoardWidth());
					fireCoor[1] = (int) (Math.random() * GameBoard.getBoardHeight());
				} while (checkFireCoor(fireCoor));
				
				listOfDirTried.clear();
				lastHitLocation = null;
			}
			
			else {
				do {
					//generate a random direction
					do {
						dir = (int) (Math.random() * 3);								
					}
					while (listOfDirTried.contains(dir));
							
					switch (dir) {
						case 0: 
							fireCoor[0] =  lastHitLocation[0];
							fireCoor[1] =  lastHitLocation[1] + 1;
							break;
						case 1:
							fireCoor[0] =  lastHitLocation[0];
							fireCoor[1] =  lastHitLocation[1] - 1;
							break;
						case 2:
							fireCoor[0] =  lastHitLocation[0] + 1;
							fireCoor[1] =  lastHitLocation[1];
							break;
						case 3:
							fireCoor[0] =  lastHitLocation[0] - 1;
							fireCoor[1] =  lastHitLocation[1];
							break;				
					}	
					listOfDirTried.add(dir); //keep track of the directions that have been tried
					
				} while (checkFireCoor(fireCoor) && listOfDirTried.size() < 4);		
			}
			
		}
		
		
		previousFireCoors.add(fireCoor); //keep track of the locations computer player has fired at
		return fireCoor;
	}


	public GameBoard getAIGameBoard()
	{
		return aiGameBoard;
	}

	public void setHitLocation(int[] hitLocation) {
		lastHitLocation = hitLocation.clone();
		listOfDirTried.clear();
	}
	
	public boolean checkFireCoor(int[] fireCoor) {
		boolean isContained = false;
		
		//check if it's one of the locations that has been fired at
		for(int[] coor : previousFireCoors) {
			if(Arrays.equals(fireCoor, coor))
				isContained = true;
		}
		
		return (hitMap[fireCoor[0]][fireCoor[1]] == 1 || isContained);
	}
	
}