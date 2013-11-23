package com.ece381.models;

import java.util.ArrayList;

import com.ece381.models.Battleship.ShipOrientation;

public class GameBoard {

	private Tile[][] gameBoard; 
	private static final int width = 6;
	private static final int height = 6;
	
	private ArrayList<Battleship> shipOnBoard;
	private ArrayList<int[]> shipOnBoardCoor;
	
	public GameBoard()
	{
		initializeGameBoard();
	}
	
	private void initializeGameBoard()
	{
		shipOnBoard = new ArrayList<Battleship>();
		shipOnBoardCoor = new ArrayList<int[]>();
		gameBoard = new Tile[width][height];
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				//initialize the game board with empty tiles
				gameBoard[i][j] = new Tile();
			}
		}
	}
	
	public boolean insertIntoGameBoard(Battleship ship, int[] coordinates)
	{
		// requires: X between 0 and 5, Y between 0 and 6
		ShipOrientation orientation = ship.getOrientation();
		int size = ship.getSize();
		switch (orientation)
		{
			case VERTICAL:
				//check if in range
				if (coordinates[1] + size - 1 < GameBoard.height)
				{
					//check if empty
					for (int i = coordinates[1]; i < coordinates[1] + size; i++)
					{
						if (gameBoard[coordinates[0]][i].getTileObject() != null)
						{
							return false;
						}
					}
					//insert into board
					shipOnBoard.add(ship);
					shipOnBoardCoor.add(coordinates);
					for (int i = coordinates[1]; i < coordinates[1] + size; i++)
					{
						gameBoard[coordinates[0]][i].fillTile(ship.getShipComponentAt(i - coordinates[1]));
					}
					return true;
				}
				else
				{
					return false;
				}
			case HORIZONTAL:
				// check if in range
				if (coordinates[0] + size - 1 < GameBoard.width)
				{
					//check if empty
					for (int i = coordinates[0]; i < coordinates[0] + size; i++)
					{
						if (gameBoard[i][coordinates[1]].getTileObject() != null)
						{
							return false;
						}
					}
					//insert into board
					shipOnBoard.add(ship);
					shipOnBoardCoor.add(coordinates);
					for (int i = coordinates[0]; i < coordinates[0] + size; i++)
					{
						gameBoard[i][coordinates[1]].fillTile(ship.getShipComponentAt(i - coordinates[0]));
					}
					return true;
				}
				else
				{
					return false;
				}
			default:
				return false;
		}	
	}

	public Tile[][] getGameBoard()
	{
		return this.gameBoard;
	}
	
	public Tile getTileAt(int x, int y)
	{
		return gameBoard[x][y];
	}
	
	public static int getBoardWidth()
	{
		return width;
	}
	
	public static int getBoardHeight()
	{
		return height;
	}
	
	public ArrayList<Battleship> getShipOnBoard()
	{
		return this.shipOnBoard;
	}
	
	public ArrayList<int[]> getShipOnBoardCoor()
	{
		return this.shipOnBoardCoor;
	}
	
	public void removeShipFromBoard(int shipIndex)
	{
		shipOnBoard.remove(shipIndex);
		shipOnBoardCoor.remove(shipIndex);
	}
	
	
	
}
