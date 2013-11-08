package com.example.models;

public class Tile {
	
	private WorldObject worldObject;

	public WorldObject getTileObject()
	{
		return this.worldObject;
	}
	
	public void fillTile(WorldObject worldObject)
	{
		this.worldObject = worldObject;
	}
}
