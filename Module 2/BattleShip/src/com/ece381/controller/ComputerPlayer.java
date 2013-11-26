package com.ece381.controller;

import java.util.Stack;

import com.ece381.application.BattleShipApp;
import com.ece381.models.Battleship;
import com.ece381.models.GameBoard;
import com.ece381.models.ShipComponent;
import com.ece381.models.WorldObject;
import com.ece381.models.Battleship.ShipOrientation;

public class ComputerPlayer {

	private GameBoard aiGameBoard;
	private int[][] hitMap; 
	private int[] lastHitLocation = null;
	//keep track potential hit coordinates
	private Stack<int[]> targets = new Stack<int[]>();
	
	//ai status
	private int aiHP = 10;
	
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
		int[] fireCoor = new int[2];
		if (targets.isEmpty())
		{
			//generate completely random coordinates when computer have no clue
			if (lastHitLocation == null)
			{
				do {
					fireCoor[0] = (int) (Math.random() * GameBoard.getBoardWidth());
					fireCoor[1] = (int) (Math.random() * GameBoard.getBoardHeight());
				}while (hitMap[fireCoor[0]][fireCoor[1]] == 1);
			}
		}
		else 
		{
			fireCoor = targets.pop();
		}
		
		return fireCoor;
	}
	
	public boolean receiveFireCommand(int[] fire_coordinates)
	{
		boolean gotHit = false;
		WorldObject wo = aiGameBoard.getTileAt(
				fire_coordinates[0], fire_coordinates[1]).getTileObject();
		if (wo != null && wo instanceof ShipComponent)
		{
			//got hit, minus 1 hp
			aiHP--;
			gotHit = true;
		}
		return gotHit;
	}
	
	public void updateHitMap(int[] coordinates)
	{
		this.hitMap[coordinates[0]][coordinates[1]] = 1;
	}
	
	public void updateTargetList(int[] coordinates)
	{
		int x = coordinates[0];
		int y = coordinates[1];
		
		//north
		if (y + 1 < GameBoard.getBoardHeight())
		{
			if (hitMap[x][y+1] == 0)
			{
				this.targets.push(new int[] {x, y+1});
			}
		}
		//south
		if (y - 1 >= 0)
		{
			if (hitMap[x][y-1] == 0)
			{
				this.targets.push(new int[] {x, y-1});
			}
		}
		//east
		if (x + 1 < GameBoard.getBoardWidth())
		{
			if (hitMap[x+1][y] == 0)
			{
				this.targets.push(new int[] {x+1, y});
			}
		}
		//west
		if (x - 1 >= 0)
		{
			if (hitMap[x-1][y] == 0)
			{
				this.targets.push(new int[] {x-1, y});
			}
		}
	}
	
	public int getHP()
	{
		return this.aiHP;
	}
	
}