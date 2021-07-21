// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>

struct HouseBlock {
	bool Occupied;
	FString RoomName;
};

struct Room {
	FString Name;
	int minArea;
};
class PROCEDURALHOUSE_API FloorPlanGenerator
{
public:
	FloorPlanGenerator();
	~FloorPlanGenerator();

	void InitializeGrid(int width, int height);
	void AddRoom(FString Name, int Area);

	std::vector<std::vector<HouseBlock>> Grid;
	std::vector<Room> RoomList;

	int Width;
	int Height;
};
