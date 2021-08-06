// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class Room;

enum BlockType
{
	Empty, RoomInternalBlock, RoomEdgeBlock, CorridorBlock, EmptyConnectedBlock, DoorBlock, ExternalWall
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

class PROCEDURALHOUSE_API Block
{
public:
	Block();
	~Block();

	int PosX;
	int PosY;
	bool isCorridorUsed = false;
	BlockType BlockType;
	NormalDirection NormalDirection;
	Room* OwnerRoom;
	//variables for path finding
	bool isVisited = false;
	Block* ParentBlockInPath = nullptr;
};

