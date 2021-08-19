// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Room.h"
#include <vector>
#include "CoreMinimal.h"
#include <ctime>
#include <queue>

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
	Room* Room = nullptr;
	int StartingPointX;
	int StartingPointY;
	int RoomWidth;
	int RoomHeight;
	NormalDirection NormalBuildDirection;
	TangentDirection TangentBuildDirection;
	float score;
};


class PROCEDURALHOUSE_API Building
{
public:
	Building();
	Building(int BlockSize, int Width, int Height, int newCorridorWidth);
	~Building();
	
	Room AddRoom(int Area, FString Name, int RoomId, RoomType RoomType, Building* Building);
	void PositionRoom(bool WithCorridors, Room* currentRoom, BuildCoordinates BuildCoordinates);
	bool AddConnection(Room* Room1, Room* Room2);
	bool CheckIfSpaceAvailable(int StartingPointX, int StartingPointY, int RoomWidth, int RoomHeight, NormalDirection NormalBuildDirection, TangentDirection TangentBuildDirection);
	void GenerateFloorPlan();
	void CreateCorridorBlocks(int PosX, int PosY, int StartingPointX, int StartingPointY, int RoomWidth, int RoomHeight);
	Block* GetBlock(int X, int Y);
	Block* AddBlock(int X, int Y, BlockType BlockType, Room* OwnerRoom);
	BuildCoordinates EvaluatesBuildPosition(Room* CurrentRoom);
	bool CheckIfPrime(int number);
	std::vector<RoomWidthHeight>* FindPossibleAspectRatios(int Area);
	bool IsInFrontOfFrontDoor(int X, int Y);
	void UpdateBuildingCornerBlocks(int PosX, int PosY);
	float EvaluateBuildCoordinatesScore(BuildCoordinates BuildCoordinates);
	void PositionGhostRoom(BuildCoordinates BuildCoordinates);
	int ChooseRandomBestPosition(std::vector<BuildCoordinates> &PossibleBuildConfigurations, float ScoreSum, int NumberOfCombinations);
	int EvaluateDistance(int PosX, int PosY, Room* Room2);
	void RemoveAllEmptyConnectedCubes();
	void FindPathToConnections(Room* Room);
	void BFSShortestPathLength(RoomConnection* Connection);
	void MarkNeighboursCorridorBlocks(Block* CurrentBlock, std::vector<Block*>* PossiblePath);
	bool ExpandRoom(Room* Room);
	bool ExpandInDirection(Room* CurrentRoom, Block* CurrentBlock, NormalDirection Direction, int ExploredBlocks);
	bool IsCorridorUsedByThisRoom(Block* CorridorBlock, Room* CurrentRoom);
	bool CheckRoomsConnection(Room* Room1, Room* Room2);
	bool CheckIfIsOnEdge(Block* Block);

	//input dimensions of the building
	int BlockSize = 0;
	int TerrainWidth = 0;
	int TerrainHeight = 0;
	int CorridorWidth = 0;

	int CorridorLinesCount = 0;
	//edge block of the building
	int MinXValue = 0;
	int MaxXValue = 0;
	int MinYValue = 0;
	int MaxYValue = 0;

	//vector of rooms in the building
	std::vector<Room*> Rooms;
	//matrix of blocks for the building
	std::vector<Block*> BuildingBlocks;
	//vector of connections among room in the building
	std::vector<RoomConnection*> Connections;

	//front door space boundaries
	int FrontSpaceLeftEdge = 0;
	int FrontSpaceRightEdge = 0;
	int FrontSpaceTopEdge = 0;
};
