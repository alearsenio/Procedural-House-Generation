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

	if (Rooms.Max() > 0) {

		House = Building(1, GridWidht, GridHeight, CorridorWidth);
		std::vector<Room*> BuildingRooms;
		for (int i = 0; i < Rooms.Max(); i++)
		{
			RoomType RoomType;
			if (Rooms[i].IsPrivate)
				RoomType = Private;
			else
				RoomType = Public;

			BuildingRooms.push_back(House.AddRoom(Rooms[i].Area, Rooms[i].Name, i, RoomType, &House));
		}
		//House.AddConnection(&BuildingRooms[0], &BuildingRooms[1]);
		for (int i = 0; i < RoomsConnections.Max(); i++)
		{
			Room* FirstRoom;
			Room* SecondRoom;

			if (RoomsConnections[i].FirstRoomId < BuildingRooms.size())
				FirstRoom = BuildingRooms[RoomsConnections[i].FirstRoomId];
			else
				FirstRoom = BuildingRooms[0];

			if (RoomsConnections[i].SecondRoomId < BuildingRooms.size())
				SecondRoom = BuildingRooms[RoomsConnections[i].SecondRoomId];
			else
				SecondRoom = BuildingRooms[1];

			House.AddConnection(FirstRoom, SecondRoom);
		}
	}
	/*
	House = Building(1, GridWidht, GridHeight, 2);
	Room LivingRoom = House.AddRoom(20, TEXT("LivingRoom"), 0, Public, &House);

	Room DiningRoom = House.AddRoom(30, TEXT("DiningRoom"), 1, Public, &House);
	House.AddConnection(&LivingRoom, &DiningRoom);

	Room Kitchen = House.AddRoom(20, TEXT("Kitchen"), 2, Public, &House);
	House.AddConnection(&Kitchen, &DiningRoom);
	House.AddConnection(&LivingRoom, &Kitchen);

	Room PrivateHall = House.AddRoom(10, TEXT("PrivateHall"), 3, Private, &House);
	House.AddConnection(&PrivateHall, &LivingRoom);


	Room BedRoom1 = House.AddRoom(25, TEXT("BedRoom1"), 4, Private, &House);
	House.AddConnection(&BedRoom1, &PrivateHall);


	Room BedRoom2 = House.AddRoom(20, TEXT("BedRoom2"), 5, Private, &House);
	House.AddConnection(&BedRoom2, &PrivateHall);


	Room BathRoom1 = House.AddRoom(10, TEXT("BathRoom1"), 6, Public, &House);
	House.AddConnection(&BathRoom1, &LivingRoom);


	Room BathRoom2 = House.AddRoom(10, TEXT("BedRoom2"), 7, Private, &House);
	House.AddConnection(&BathRoom2, &PrivateHall);*/


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
	if (PrivateRoomWallMesh && PrivateRoomFloorMesh && PublicRoomWallMesh && PublicRoomFloorMesh && CorridorFloorMesh && DoorMesh)
	{
		float WallOffset = ModulesWidth / 2 - WallsThickness / 2;

		FActorSpawnParameters SpawnParams;

		FVector Location(0, 0, 0);
		FRotator Rotation(0, 0, 0);

		FVector NewLocation;
		AActor* SpawnActorRef;

		//go on every block of the floor plan
		for (int i = 0; i < House.BuildingBlocks.size(); i++)
		{
			NewLocation = FVector(House.BuildingBlocks[i]->PosY * ModulesWidth, House.BuildingBlocks[i]->PosX * ModulesWidth, 0);

			//if the block belongs to a room
			if (House.BuildingBlocks[i]->BlockType == RoomInternalBlock || House.BuildingBlocks[i]->BlockType == RoomEdgeBlock || House.BuildingBlocks[i]->BlockType == DoorBlock)
			{
				TSubclassOf<AActor> FloorMesh;

				//position the floor tile
				if (House.BuildingBlocks[i]->OwnerRoom->RoomType == Public)
					FloorMesh = PublicRoomFloorMesh;
				else
					FloorMesh = PrivateRoomFloorMesh;

				SpawnActorRef = GetWorld()->SpawnActor<AActor>(FloorMesh, NewLocation, Rotation, SpawnParams);

				//position the wall
				if (House.BuildingBlocks[i]->BlockType == RoomEdgeBlock)
				{
					for (int j = 0; j < House.BuildingBlocks[i]->WallsDirection.size(); j++)
					{
						TSubclassOf<AActor> WallMesh;

						//position the floor tile
						if (House.BuildingBlocks[i]->OwnerRoom->RoomType == Public)
							WallMesh = PrivateRoomWallMesh;
						else
							WallMesh = PublicRoomWallMesh;

						int angle = 0;
						int Xoffset = 0;
						int Yoffset = 0;
						switch (House.BuildingBlocks[i]->WallsDirection[j])
						{
						case Left:
							angle = -90;
							Yoffset = -WallOffset;
							break;
						case Right:
							angle = 90;
							Yoffset = WallOffset;
							break;
						case Up:
							angle = 0;
							Xoffset = +WallOffset;
							break;
						case Down:
							angle = 180;
							Xoffset = -WallOffset;
							break;
						default:
							break;
						}
						NewLocation = FVector(House.BuildingBlocks[i]->PosY * ModulesWidth + Xoffset, House.BuildingBlocks[i]->PosX * ModulesWidth + Yoffset, 20);
						FRotator NewRotation = FRotator(0, angle, 0);
						SpawnActorRef = GetWorld()->SpawnActor<AActor>(WallMesh, NewLocation, NewRotation, SpawnParams);
					}
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
				//SpawnActorRef = GetWorld()->SpawnActor<AActor>(CorridorFloorMesh, NewLocation, Rotation, SpawnParams);

				if (House.BuildingBlocks[i]->isCorridorUsed)
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(CorridorUsedFloorMesh, NewLocation, Rotation, SpawnParams);
				else
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(CorridorFloorMesh, NewLocation, Rotation, SpawnParams);
			}
			//if the block belongs to an exernal wall
			else if (House.BuildingBlocks[i]->BlockType == EmptyConnectedBlock)
			{
				for (int j = 0; j < House.BuildingBlocks[i]->WallsDirection.size(); j++)
				{
					int angle = 0;
					int Xoffset = 0;
					int Yoffset = 0;
					switch (House.BuildingBlocks[i]->WallsDirection[j])
					{
					case Left:
						angle = -90;
						Yoffset = -WallOffset;
						break;
					case Right:
						angle = 90;
						Yoffset = +WallOffset;
						break;
					case Up:
						angle = 0;
						Xoffset = +WallOffset;
						break;
					case Down:
						angle = 180;
						Xoffset = -WallOffset;
						break;
					default:
						break;
					}
					NewLocation = FVector(House.BuildingBlocks[i]->PosY * ModulesWidth + Xoffset, House.BuildingBlocks[i]->PosX * ModulesWidth + Yoffset, 20);
					FRotator NewRotation = FRotator(0, angle, 0);
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(ExternalWallMesh, NewLocation, NewRotation, SpawnParams);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("One or more meshes missing, Can't generate house"));
	}
}

