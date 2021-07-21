// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class Room;

enum BlockType
{
	Empty, RoomInternalBlock, RoomEdgeBlock, CorridorBlock, EmptyConnectedBlock
};

enum BuildDirection
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
	BlockType BlockType;
	BuildDirection BuildDirection;
	Room* OwnerRoom;
};
