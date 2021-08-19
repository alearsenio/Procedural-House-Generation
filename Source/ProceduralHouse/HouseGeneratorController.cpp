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

	House = Building(1, GridWidht, GridHeight, 2);
	Room LivingRoom = House.AddRoom(30, TEXT("LivingRoom"), 0, Public, &House);

	Room DiningRoom = House.AddRoom(40, TEXT("DiningRoom"), 1, Public, &House);
	House.AddConnection(&LivingRoom, &DiningRoom);

	Room Kitchen = House.AddRoom(30, TEXT("Kitchen"), 2, Public, &House);
	House.AddConnection(&Kitchen, &DiningRoom);
	House.AddConnection(&LivingRoom, &Kitchen);

	Room PrivateHall = House.AddRoom(30, TEXT("PrivateHall"), 3, Private, &House);
	House.AddConnection(&PrivateHall, &LivingRoom);


	Room BedRoom1 = House.AddRoom(25, TEXT("BedRoom1"), 4, Private, &House);
	House.AddConnection(&BedRoom1, &LivingRoom);


	/*Room BedRoom2 = House.AddRoom(80, TEXT("BedRoom2"), 5, Private, &House);
	House.AddConnection(&BedRoom2, &PrivateHall);


	Room BathRoom1 = House.AddRoom(20, TEXT("BathRoom1"), 6, Public, &House);
	House.AddConnection(&BathRoom1, &LivingRoom);


	Room BathRoom2 = House.AddRoom(20, TEXT("BedRoom2"), 7, Private, &House);
	House.AddConnection(&BathRoom2, &BedRoom2);*/


	/*Room BathRoom = House.AddRoom(30, TEXT("BathRoom"), 6, Private);
	BathRoom.AddConnectedRoom(&PrivateHall);*/


	House.GenerateFloorPlan();
	PositionMesh();
}


// Called every frame
void UHouseGeneratorController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHouseGeneratorController::PositionMesh()
{
	//check if the mesh are avaiable
	if (PrivateRoomWallMesh && PrivateRoomFloorMesh && PublicRoomWallMesh && PublicRoomFloorMesh  && CorridorFloorMesh && DoorMesh)
	{
		FActorSpawnParameters SpawnParams;

		FVector Location(0, 0, 0);
		FRotator Rotation(0, 0, 0);

		FVector NewLocation;
		AActor* SpawnActorRef;

		//go on every block of the floor plan
		for (int i = 0; i < House.BuildingBlocks.size(); i++)
		{
			NewLocation = FVector(House.BuildingBlocks[i]->PosY * 100, House.BuildingBlocks[i]->PosX * 100, 0);

			//if the block belongs to a room
			if (House.BuildingBlocks[i]->BlockType == RoomInternalBlock || House.BuildingBlocks[i]->BlockType == RoomEdgeBlock || House.BuildingBlocks[i]->BlockType == DoorBlock)
			{
				TSubclassOf<AActor> FloorMesh;

				//position the floor tile
				if (House.BuildingBlocks[i]->OwnerRoom->RoomType == Public)
					FloorMesh = PrivateRoomFloorMesh;
				else
					FloorMesh = PublicRoomFloorMesh;

				SpawnActorRef = GetWorld()->SpawnActor<AActor>(FloorMesh, NewLocation, Rotation, SpawnParams);

				//position the wall
				if (House.BuildingBlocks[i]->BlockType == RoomEdgeBlock) 
				{
					TSubclassOf<AActor> WallMesh;

					//position the floor tile
					if (House.BuildingBlocks[i]->OwnerRoom->RoomType == Public)
						WallMesh = PrivateRoomWallMesh;
					else
						WallMesh = PublicRoomWallMesh;
					NewLocation = FVector(House.BuildingBlocks[i]->PosY * 100, House.BuildingBlocks[i]->PosX * 100, 20);
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(WallMesh, NewLocation, Rotation, SpawnParams);
				}
				else if (House.BuildingBlocks[i]->BlockType == DoorBlock)
				{
					TSubclassOf<AActor> Door = DoorMesh;
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(Door, NewLocation, Rotation, SpawnParams);
				}
			}
			//if the block belongs to a corridor
			else if (House.BuildingBlocks[i]->BlockType == CorridorBlock)
			{
				SpawnActorRef = GetWorld()->SpawnActor<AActor>(CorridorFloorMesh, NewLocation, Rotation, SpawnParams);

				/*if(House.BuildingBlocks[i]->isCorridorUsed)
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(ConnectedCube, NewLocation, Rotation, SpawnParams);
				else
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(CorridorCube, NewLocation, Rotation, SpawnParams);*/
			}
			//if the block belongs to an exernal wall
			else if (House.BuildingBlocks[i]->BlockType == EmptyConnectedBlock)
			{
				NewLocation = FVector(House.BuildingBlocks[i]->PosY * 100, House.BuildingBlocks[i]->PosX * 100, 20);
				SpawnActorRef = GetWorld()->SpawnActor<AActor>(ExternalWallMesh, NewLocation, Rotation, SpawnParams);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("One or more meshes missing, Can't generate house"));
	}
}

