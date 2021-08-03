// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class Room;

enum BlockType
{
	Empty, RoomInternalBlock, RoomEdgeBlock, CorridorBlock, EmptyConnectedBlock, DoorBlock
};

enum NormalDirection
{
	Left, Right, Up, Down
};

class PROCEDURALHOUSE_API Block
{
public:
	Block();
	~Block();

	int PosX;
	int PosY;
	bool isUsed = false;
	BlockType BlockType;
	NormalDirection NormalDirection;
	Room* OwnerRoom;
};
