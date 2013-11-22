package com.ece381.controller;

import com.ece381.application.BattleShipApp;
import com.ece381.models.Battleship;
import com.ece381.models.GameBoard;
import com.ece381.models.Battleship.ShipOrientation;

public class ComputerPlayer {

	private GameBoard aiGameBoard;
	private int[][] hitMap; 
	private int[] lastHitLocation = null;
	
	public ComputerPlayer()
	{
		generateGameBoard();
		generateHitMap();
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
		int[] randomCoor = new int[2];
		//generate completely random coordinates when computer have no clue
		if (lastHitLocation == null)
		{
			do {
				randomCoor[0] = (int) (Math.random() * GameBoard.getBoardWidth());
				randomCoor[1] = (int) (Math.random() * GameBoard.getBoardHeight());
			}while (hitMap[randomCoor[0]][randomCoor[1]] == 1);
		}
		return randomCoor;
	}
}
