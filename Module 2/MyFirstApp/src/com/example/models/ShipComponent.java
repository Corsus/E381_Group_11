package com.example.models;

public class ShipComponent extends WorldObject{

	private Battleship parentShip;
	
	public ShipComponent(Battleship parent)
	{
		this.parentShip = parent;
	}
	
	public Battleship getParent()
	{
		return this.parentShip;
	}
	
}
