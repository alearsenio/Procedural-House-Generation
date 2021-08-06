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

	House = Building(1, GridWidht, GridHeight, 2);
	Room LivingRoom = House.AddRoom(60, TEXT("LivingRoom"), 0, Public);

	Room DiningRoom = House.AddRoom(60, TEXT("DiningRoom"), 1, Public);
	DiningRoom.AddConnectedRoom(&LivingRoom);

	Room Kitchen = House.AddRoom(60, TEXT("Kitchen"), 2, Public);
	Kitchen.AddConnectedRoom(&DiningRoom);

	Room PrivateHall = House.AddRoom(40, TEXT("PrivateHall"), 3, Private);
	PrivateHall.AddConnectedRoom(&LivingRoom);

	Room BedRoom1 = House.AddRoom(80, TEXT("BedRoom1"), 4, Private);
	BedRoom1.AddConnectedRoom(&PrivateHall);

	Room BedRoom2 = House.AddRoom(80, TEXT("BedRoom2"), 5, Private);
	BedRoom2.AddConnectedRoom(&PrivateHall);

	Room BathRoom1 = House.AddRoom(45, TEXT("BathRoom1"), 6, Public);
	BathRoom1.AddConnectedRoom(&LivingRoom);

	Room BathRoom2 = House.AddRoom(45, TEXT("BedRoom2"), 7, Private);
	BathRoom2.AddConnectedRoom(&PrivateHall);

	Room BathRoom = House.AddRoom(30, TEXT("BathRoom"), 6, Private);
	BathRoom.AddConnectedRoom(&PrivateHall);


	House.GenerateFloorPlan();
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
	if (PublicRoomCube && CorridorCube && ConnectedCube && PrivateRoomCube)
	{
		FVector NewLocation;
		AActor* SpawnActorRef;
		for (int i = 0; i < House.BuildingBlocks.size(); i++)
		{
			NewLocation = FVector(House.BuildingBlocks[i]->PosY * 100, House.BuildingBlocks[i]->PosX * 100, 0);
			if (House.BuildingBlocks[i]->BlockType == RoomInternalBlock || House.BuildingBlocks[i]->BlockType == RoomEdgeBlock)
			{
				TSubclassOf<AActor> RoomCube;
				if (House.BuildingBlocks[i]->OwnerRoom->RoomType == Public)
					RoomCube = PublicRoomCube;
				else
					RoomCube = PrivateRoomCube;

				SpawnActorRef = GetWorld()->SpawnActor<AActor>(RoomCube, NewLocation, Rotation, SpawnParams);
				if (House.BuildingBlocks[i]->BlockType == RoomEdgeBlock) 
				{
					for (int j = 1; j <= 4; j++)
					{
						NewLocation = FVector(House.BuildingBlocks[i]->PosY * 100, House.BuildingBlocks[i]->PosX * 100, 100 * j);
						SpawnActorRef = GetWorld()->SpawnActor<AActor>(RoomCube, NewLocation, Rotation, SpawnParams);
					}
				}
			}
			else if (House.BuildingBlocks[i]->BlockType == CorridorBlock)
			{
				if(House.BuildingBlocks[i]->isCorridorUsed)
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(ConnectedCube, NewLocation, Rotation, SpawnParams);
				else
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(CorridorCube, NewLocation, Rotation, SpawnParams);
			}
			else if (House.BuildingBlocks[i]->BlockType == ExternalWall)
			{
				SpawnActorRef = GetWorld()->SpawnActor<AActor>(ConnectedCube, NewLocation, Rotation, SpawnParams);
				for (int j = 1; j <= 4; j++)
				{
					NewLocation = FVector(House.BuildingBlocks[i]->PosY * 100, House.BuildingBlocks[i]->PosX * 100, 100 * j);
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(ConnectedCube, NewLocation, Rotation, SpawnParams);
				}
			}
		}
	}
}

