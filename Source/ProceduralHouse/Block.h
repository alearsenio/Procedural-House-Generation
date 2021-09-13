// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include "CoreMinimal.h"

class Room;
class Block;

enum BlockType
{
	Empty, RoomInternalBlock, RoomEdgeBlock, CorridorBlock, EmptyConnectedBlock
};

enum NormalDirection
{
	Left, Right, Up, Down
};

struct Position
{
	int X;
	int Y;
};

enum SideType
{
	Wall, Door, Window, EmptySide
};

struct RoomConnection
{
	Room* Room1 = nullptr;
	Room* Room2 = nullptr;
	bool AreConnected = false;
	std::vector<Block*> ConnectionPath;
	bool HasDoor = false;
	bool HasFirstCorridorDoor = false;
	bool HasSecondCorridorDoor = false;
};

class PROCEDURALHOUSE_API Block
{
public:
	Block();
	~Block();

	int PosX;
	int PosY;
	bool isCorridorUsed = false;
	int CorridorId = 0;
	BlockType BlockType;
	NormalDirection NormalDirection;
	std::vector<int> WallsDirection;
	Room* OwnerRoom;
	//variables for path finding
	bool isVisited = false;
	Block* ParentBlockInPath = nullptr;
	std::vector<RoomConnection*> ConnectionsUsingBlock;
	SideType Sides[4] = { EmptySide, EmptySide, EmptySide, EmptySide };
};
