// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Room.h"
#include <vector>
#include "CoreMinimal.h"


class PROCEDURALHOUSE_API Building
{
public:
	Building();
	Building(int BlockSize, int Width, int Height, int newCorridorWidth);
	~Building();
	
	Room AddRoom(int Area, FString Name, int RoomId);
	bool PositionRoom(Room currentRoom, int startingPointX, int StartingPointY, int RoomWidth, int RoomHeight, BuildDirection BuildDirectionX, BuildDirection BuildDirectionY);
	bool checkIfSpaceAvaiable(int startingPointX, int StartingPointY, int width, int height, BuildDirection BuildDirection);
	void GenerateFloorPlan();
	void CreateCorridorBlocks(int PosX, int PosY, int StartingPointX, int StartingPointY, int RoomWidth, int RoomHeight);
	//dimensions of the building
	int BlockSize = 0;
	int TerrainWidth = 0;
	int TerrainHeight = 0;
	int CorridorWidth = 0;

	//vector of rooms in the building
	std::vector<Room*> Rooms;
	//vector of blocks used to connect the rooms
	std::vector<Block*> CorridorBlocks;
	//vector of empty connected blocks
	std::vector<Block*> EmptyConnectectBlocks;
	//matrix of blocks for the building
	std::vector<std::vector<Block>> HouseGrid;
};
