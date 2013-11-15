package com.example.models;

import java.util.ArrayList;

public class Battleship {
	
	private int size;
	private ShipOrientation orientation;
	private ArrayList<ShipComponent> shipComponents;
	
	public enum ShipOrientation
	{
		VERTICAL,
		HORIZONTAL;
		
		public String toString()
		{
			if (this == ShipOrientation.HORIZONTAL)
				return "H";
			else
				return "V";
		}
	}
	
	public Battleship(int size, ShipOrientation orientation)
	{
		this.size = size;
		this.orientation = orientation;
		shipComponents = new ArrayList<ShipComponent>();
		initializeBattleShip();
	}
	
	private void initializeBattleShip()
	{
		for (int i = 0; i < size; i++)
		{
			shipComponents.add(new ShipComponent(this));
		}
	}
	
	public ShipComponent getShipComponentAt(int index)
	{
		return this.shipComponents.get(index);
	}
	
	public ShipOrientation getOrientation()
	{
		return this.orientation;
	}
		
	public int getSize()
	{
		return this.size;
	}

}
