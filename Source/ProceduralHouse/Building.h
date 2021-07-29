// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Room.h"
#include <vector>
#include "CoreMinimal.h"

enum TangentDirection
{
	Normal, Inverted
};


struct RoomWidthHeight
{
	int Width;
	int Height;
};

struct BuildCoordinates
{
	int StartingPointX;
	int StartingPointY;
	int RoomWidth;
	int RoomHeight;
	NormalDirection NormalBuildDirection;
	TangentDirection TangentBuildDirection;
};


class PROCEDURALHOUSE_API Building
{
public:
	Building();
	Building(int BlockSize, int Width, int Height, int newCorridorWidth);
	~Building();
	
	Room AddRoom(int Area, FString Name, int RoomId);
	bool PositionRoom(Room* currentRoom, int startingPointX, int StartingPointY, int RoomWidth, int RoomHeight, NormalDirection NormalBuildDirection, TangentDirection TangentBuildDirection);
	bool CheckIfSpaceAvaiable(int StartingPointX, int StartingPointY, int RoomWidth, int RoomHeight, NormalDirection NormalBuildDirection, TangentDirection TangentBuildDirection);
	void GenerateFloorPlan();
	void CreateCorridorBlocks(int PosX, int PosY, int StartingPointX, int StartingPointY, int RoomWidth, int RoomHeight);
	Block* GetBlock(int X, int Y);
	Block* AddBlock(int X, int Y, BlockType BlockType, Room* OwnerRoom);
	BuildCoordinates EvaluatesBuildPosition(Room* CurrentRoom);
	bool CheckIfPrime(int number);
	std::vector<RoomWidthHeight>* FindPossibleAspectRatios(int Area);
	bool IsInFrontOfFrontDoor(int X, int Y);

	//dimensions of the building
	int BlockSize = 0;
	int TerrainWidth = 0;
	int TerrainHeight = 0;
	int CorridorWidth = 0;
	Block* TopRightBlock = nullptr;
	Block* BottomLeftBlock = nullptr;
	//vector of rooms in the building
	std::vector<Room*> Rooms;
	//vector of blocks used to connect the rooms
	std::vector<Block*> CorridorBlocks;
	//vector of empty connected blocks
	std::vector<Block*> EmptyConnectectBlocks;
	//matrix of blocks for the building
	std::vector<Block*> HouseBlocks;

	//front door space boundaries
	int FrontSpaceLeftEdge = 0;
	int FrontSpaceRightEdge = 0;
	int FrontSpaceTopEdge = 0;
};
