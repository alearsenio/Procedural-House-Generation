// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Block.h"
#include <vector>
#include "CoreMinimal.h"

enum RoomType
{
	Public, Private
};


struct Door
{
	bool OnCorridor = false;
	Room* FirstRoom = nullptr;
	Room* SecondRoom = nullptr;
	Block* Block = nullptr;
};

class PROCEDURALHOUSE_API Room
{
public:
	Room();
	~Room();

	int Area = 0;
	int RoomId = 0;
	FString Name = "";
	std::vector<Block*> RoomBlocks;
	std::vector<Room*> ConnectedRooms;
	std::vector<Door> RoomDoors;
	RoomType RoomType;
	bool IsPositioned = false;

	bool AddConnectedRoom(Room* ConnectedRoom);
};
