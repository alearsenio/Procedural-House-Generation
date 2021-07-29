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
	Room LivingRoom = House.AddRoom(124, TEXT("LivingRoom"), 0);

	Room DiningRoom = House.AddRoom(20, TEXT("DiningRoom"), 1);
	DiningRoom.AddConnectedRoom(&LivingRoom);

	Room BedRoom = House.AddRoom(20, TEXT("BedRoom"), 2);
	BedRoom.AddConnectedRoom(&LivingRoom);

	Room BathRoom = House.AddRoom(20, TEXT("BathRoom"), 3);
	BedRoom.AddConnectedRoom(&LivingRoom);


	House.GenerateFloorPlan();
	/*UE_LOG(LogTemp, Warning, TEXT(" %d"), House.HouseBlocks.size());
	for (int i = 0; i < House.Rooms.size(); i++)
	{
		for (int j = 0; j < House.Rooms[i]->ConnectedRooms.size(); j++)
		{
			UE_LOG(LogTemp, Warning, TEXT(" %d"), House.Rooms[i]->ConnectedRooms[j]->RoomId);
		}
	}
	for (int i = 0; i < House.HouseBlocks.size(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("%d  %d"), House.HouseBlocks[i]->PosX, House.HouseBlocks[i]->PosY);
	}*/
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
	if (RoomCube && CorridorCube && ConnectedCube)
	{
		for (int i = 0; i < House.HouseBlocks.size(); i++)
		{
			FVector NewLocation(House.HouseBlocks[i]->PosY * 100, House.HouseBlocks[i]->PosX * 100, 0);
			if (House.HouseBlocks[i]->BlockType == RoomInternalBlock || House.HouseBlocks[i]->BlockType == RoomEdgeBlock)
			{
				AActor* SpawnActorRef = GetWorld()->SpawnActor<AActor>(RoomCube, NewLocation, Rotation, SpawnParams);
				if (House.HouseBlocks[i]->BlockType == RoomEdgeBlock) 
				{
					FVector NewLocation2(House.HouseBlocks[i]->PosY * 100, House.HouseBlocks[i]->PosX * 100, 100);
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(RoomCube, NewLocation2, Rotation, SpawnParams);
					FVector NewLocation3(House.HouseBlocks[i]->PosY * 100, House.HouseBlocks[i]->PosX * 100, 200);
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(RoomCube, NewLocation3, Rotation, SpawnParams);


				}
			}
			else if (House.HouseBlocks[i]->BlockType == CorridorBlock)
			{
				AActor* SpawnActorRef = GetWorld()->SpawnActor<AActor>(CorridorCube, NewLocation, Rotation, SpawnParams);
			}
			else if (House.HouseBlocks[i]->BlockType == EmptyConnectedBlock)
			{
				AActor* SpawnActorRef = GetWorld()->SpawnActor<AActor>(ConnectedCube, NewLocation, Rotation, SpawnParams);
			}
		}
	}
}

