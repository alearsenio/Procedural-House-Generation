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

	//if the user provided at leat one room
	if (Rooms.Max() > 0) {

		House = Building(1, GridWidht, GridHeight, CorridorWidth);
		std::vector<Room*> BuildingRooms;
		//add the rooms to the house
		for (int i = 0; i < Rooms.Max(); i++)
		{
			RoomType RoomType;
			if (Rooms[i].IsPrivate)
				RoomType = Private;
			else
				RoomType = Public;

			BuildingRooms.push_back(House.AddRoom(Rooms[i].Area, Rooms[i].Name, i, RoomType, &House));
		}
		//add the connections to the house
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

		steady_clock::time_point t1 = steady_clock::now();
		//generate the floor plan
		House.GenerateFloorPlan();
		steady_clock::time_point t2 = steady_clock::now();

		//measure and print the time to generate the floor plan and ratio of space used
		duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
		double TimeElapsed = time_span.count();
		UE_LOG(LogTemp, Warning, TEXT("Time to generate floor plan: %f seconds"), TimeElapsed);
		UE_LOG(LogTemp, Warning, TEXT("Ratio of space used: %f"), House.CalcutateRatioSpaceUsed());

		t1 = steady_clock::now();
		//position the meshes on top of the floor plan
		PositionMesh();
		t2 = steady_clock::now();

		//measure and print the time to position the meshes
		time_span = duration_cast<duration<double>>(t2 - t1);
		TimeElapsed = time_span.count();
		UE_LOG(LogTemp, Warning, TEXT("Time to position the meshes: %f seconds"), TimeElapsed);
	}
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

				/*if (House.BuildingBlocks[i]->isCorridorUsed)
					SpawnActorRef = GetWorld()->SpawnActor<AActor>(CorridorUsedFloorMesh, NewLocation, Rotation, SpawnParams);
				else*/
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

