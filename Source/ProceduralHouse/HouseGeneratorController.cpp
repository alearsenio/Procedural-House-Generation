// Fill out your copyright notice in the Description page of Project Settings.


#include "HouseGeneratorController.h"

// Sets default values for this component's properties
UHouseGeneratorController::UHouseGeneratorController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHouseGeneratorController::BeginPlay()
{
	Super::BeginPlay();
	// ...
	FVector Location(0, 0, 0);
	FRotator Rotation(0, 0, 0);
	
	House = Building(1, GridWidht, GridHeight, 1);
	Room LivingRoom = House.AddRoom(16, TEXT("LivingRoom"), 0);
	Room DiningRoom = House.AddRoom(20, TEXT("DiningRoom"), 1);
	DiningRoom.AddConnectedRoom(&LivingRoom);
	Room BedRoom = House.AddRoom(20, TEXT("BedRoom"), 2);
	BedRoom.AddConnectedRoom(&LivingRoom);


	House.GenerateFloorPlan();
	for (int i = 0; i < House.EmptyConnectectBlocks.size(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("%d %d"), House.EmptyConnectectBlocks[i]->PosX, House.EmptyConnectectBlocks[i]->PosY);
	}
	SpawnObject(Location, Rotation);
}


// Called every frame
void UHouseGeneratorController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHouseGeneratorController::SpawnObject(FVector Location, FRotator Rotation)
{
	FActorSpawnParameters SpawnParams;
	for (size_t i = 0; i < House.TerrainWidth; i++)
	{
		for (size_t j = 0; j < House.TerrainHeight; j++)
		{
			if (House.HouseGrid[i][j].BlockType != Empty && RoomCube && CorridorCube && ConnectedCube) 
			{
				FVector NewLocation(i * 100, j * 100, 0);
				if (House.HouseGrid[i][j].BlockType == RoomInternalBlock || House.HouseGrid[i][j].BlockType == RoomEdgeBlock)
				{
					AActor* SpawnActorRef = GetWorld()->SpawnActor<AActor>(RoomCube, NewLocation, Rotation, SpawnParams);
				}
				else if (House.HouseGrid[i][j].BlockType == CorridorBlock)
				{
					AActor* SpawnActorRef = GetWorld()->SpawnActor<AActor>(CorridorCube, NewLocation, Rotation, SpawnParams);
				}
				else if(House.HouseGrid[i][j].BlockType == EmptyConnectedBlock)
				{
					AActor* SpawnActorRef = GetWorld()->SpawnActor<AActor>(ConnectedCube, NewLocation, Rotation, SpawnParams);
				}
			}
		}
	}
}

