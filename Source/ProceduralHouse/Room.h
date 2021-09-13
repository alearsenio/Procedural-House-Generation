// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Block.h"
#include <vector>
#include "CoreMinimal.h"

class Building;

enum RoomType
{
	Public, Private
};

enum MoveDirection 
{
	MoveLeft, MoveRight, MoveUp, MoveDown
};


class PROCEDURALHOUSE_API Room
{
public:
	Room();
	~Room();

	void MoveRoom(MoveDirection Direction);
	void FreeCorridorPath();


	int Area = 0;
	int RoomId = 0;
	FString Name = "";
	std::vector<Block*> RoomBlocks;
	std::vector<Block*> PathBlocks;
	std::vector<RoomConnection*> RoomConnections;
	Building* OwnerBuilding;
	RoomType RoomType;
	bool IsPositioned = false;

};
