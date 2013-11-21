package com.ece381.models;

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
