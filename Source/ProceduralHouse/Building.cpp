// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

Building::Building()
{
}

Building::Building(int newBlockSize, int newWidth, int newHeight, int newCorridorWidth)
{
	BlockSize = newBlockSize;
	TerrainWidth = newWidth;
	TerrainHeight = newHeight;
	CorridorWidth = newCorridorWidth;
}

Building::~Building()
{
}

//position all the rooms and generate the floor plan of the building
void Building::GenerateFloorPlan()
{
	if (Rooms.size() > 0)
	{
		//!!!!!!!!!!!change this part to real front based on the calculated width of the main room
		//define front door untouchable space 
		FrontSpaceLeftEdge = 0;
		FrontSpaceRightEdge = 4;
		FrontSpaceTopEdge = 0;
		//!!!!!!!!!!!change this part to real front based on the calculated width of the main room

		//position the first room
		BuildCoordinates BuildCoordinates;
		BuildCoordinates.StartingPointX = 0;
		BuildCoordinates.StartingPointY = 0;
		BuildCoordinates.RoomWidth = 5;
		BuildCoordinates.RoomHeight = 6;
		BuildCoordinates.NormalBuildDirection = Up;
		BuildCoordinates.TangentBuildDirection = Normal;
		PositionRoom(true, Rooms[0], BuildCoordinates);

		//UE_LOG(LogTemp, Warning, TEXT("bottom level: %d top level: %d, left level: %d right level: %d  "), MinYValue, MaxYValue, MinXValue, MaxXValue);
		for (int RoomId = 1; RoomId < Rooms.size(); RoomId++)
		{
			BuildCoordinates = EvaluatesBuildPosition(Rooms[RoomId]);
			PositionRoom(true, Rooms[RoomId], BuildCoordinates);
		}
		


		/*bool positionFound = false;
		Block randomBlock;
		for (int i = 0; i < 500; i++)
		{
			randomBlock = *EmptyConnectectBlocks[rand() % EmptyConnectectBlocks.size()];
			positionFound = CheckIfSpaceAvaiable(randomBlock.PosX, randomBlock.PosY, 4, 5, randomBlock.NormalDirection, Normal);
			if (positionFound)
				break;
		}
		positionFound = false;
		PositionRoom(true,Rooms[1], randomBlock.PosX, randomBlock.PosY, 4, 5, randomBlock.NormalDirection, Normal);

		for (int i = 0; i < 500; i++)
		{
			randomBlock = *EmptyConnectectBlocks[rand() % EmptyConnectectBlocks.size()];
			positionFound = CheckIfSpaceAvaiable(randomBlock.PosX, randomBlock.PosY, 6, 4, randomBlock.NormalDirection, Normal);
			if (positionFound)
				break;
		}
		positionFound = false;
		PositionRoom(true, Rooms[2], randomBlock.PosX, randomBlock.PosY, 6, 4, randomBlock.NormalDirection, Normal);

		for (int i = 0; i < 500; i++)
		{
			randomBlock = *EmptyConnectectBlocks[rand() % EmptyConnectectBlocks.size()];
			positionFound = CheckIfSpaceAvaiable(randomBlock.PosX, randomBlock.PosY, 4, 4, randomBlock.NormalDirection, Normal);
			if (positionFound)
				break;
		}
		positionFound = false;
		PositionRoom(true, Rooms[3], randomBlock.PosX, randomBlock.PosY, 4, 4, randomBlock.NormalDirection, Normal);*/

	}


}

BuildCoordinates Building::EvaluatesBuildPosition(Room* CurrentRoom)
{
	int Area = CurrentRoom->Area;

	//find possible room's width and height combinations
	std::vector<RoomWidthHeight> PossibleAspectRatios = *FindPossibleAspectRatios(Area);

	UE_LOG(LogTemp, Warning, TEXT("%d  %d  %d"), PossibleAspectRatios[0].Width, PossibleAspectRatios[0].Height, Area);

	std::vector<BuildCoordinates> PossibleBuildConfigurations;

	//for every empty connected block avaiable
	for (int blockPos = 0; blockPos < EmptyConnectectBlocks.size(); blockPos++)
	{
		//for every configuration of aspect ratio for that room's area
		for (int AspectRatio = 0; AspectRatio < PossibleAspectRatios.size(); AspectRatio++)
		{
			//for both direction of building, normal and inverted
			TangentDirection TangentDirection = Normal;
			for (int TangentBuild = 0; TangentBuild <= 1; TangentBuild++)
			{
				if (TangentBuild == 1)
					TangentDirection = Inverted;

				int PosX = EmptyConnectectBlocks[blockPos]->PosX;
				int PosY = EmptyConnectectBlocks[blockPos]->PosY;
				int Width = PossibleAspectRatios[AspectRatio].Width;
				int Height = PossibleAspectRatios[AspectRatio].Height;

				//check if its possible to build a room with these specifications
				if (CheckIfSpaceAvaiable(PosX, PosY, Width, Height, EmptyConnectectBlocks[blockPos]->NormalDirection, TangentDirection))
				{
					BuildCoordinates BuildCoordinates;
					BuildCoordinates.Room = CurrentRoom;
					BuildCoordinates.StartingPointX = EmptyConnectectBlocks[blockPos]->PosX;
					BuildCoordinates.StartingPointY = EmptyConnectectBlocks[blockPos]->PosY;
					BuildCoordinates.RoomWidth = PossibleAspectRatios[AspectRatio].Width;
					BuildCoordinates.RoomHeight = PossibleAspectRatios[AspectRatio].Height;
					BuildCoordinates.NormalBuildDirection = EmptyConnectectBlocks[blockPos]->NormalDirection;
					BuildCoordinates.TangentBuildDirection = TangentDirection;
					BuildCoordinates.score = EvaluateBuildCoordinatesScore(BuildCoordinates);
					PossibleBuildConfigurations.push_back(BuildCoordinates);
				}
			}
		}
	}
	int randomId = rand() % PossibleBuildConfigurations.size();
	return PossibleBuildConfigurations[randomId];
}

//evaluate the score of positiong the room with that speficic configuration
float Building::EvaluateBuildCoordinatesScore(BuildCoordinates BuildCoordinates)
{
	//save current building size values
	int OldMinXValue = MinXValue;
	int OldMaxXValue = MaxXValue;
	int OldMinYValue = MinYValue;
	int OldMaxYValue = MaxYValue;

	//temporary position the room without corridors, updating the building size values
	PositionGhostRoom(BuildCoordinates);

	//calculate the space used by the building including the empty blocks inside,
	int AreaUsed = (MaxXValue + 1 - MinXValue) * (MaxYValue + 1 - MinYValue);
	int BlocksUsed = 0;
	//calculate the ratio of actual block used in that area
	for (int i = 0; i < BuildingBlocks.size(); i++)
	{
		if (BuildingBlocks[i]->BlockType != EmptyConnectedBlock)
			BlocksUsed++;
	}
	BlocksUsed += BuildCoordinates.Room->Area;
	float RatioOfSpaceUsed = (float)BlocksUsed / AreaUsed;

	//UE_LOG(LogTemp, Warning, TEXT("blocks used:%d, area blocks: %d, score = %f"), BlocksUsed, AreaUsed, RatioOfSpaceUsed);

	//restore the old room size values
	MinXValue = OldMinXValue;
	MaxXValue = OldMaxXValue;
	MinYValue = OldMinYValue;
	MaxYValue = OldMaxYValue;

	return RatioOfSpaceUsed;
}

//position the room only to check if how it changes the edge values of the building
void Building::PositionGhostRoom(BuildCoordinates BuildCoordinates)
{

	int StartingPointX = BuildCoordinates.StartingPointX;
	int StartingPointY = BuildCoordinates.StartingPointY;
	int RoomWidth = BuildCoordinates.RoomWidth;
	int RoomHeight = BuildCoordinates.RoomHeight;
	NormalDirection NormalBuildDirection = BuildCoordinates.NormalBuildDirection;
	TangentDirection TangentBuildDirection = BuildCoordinates.TangentBuildDirection;

	//check normal build direction
	if (NormalBuildDirection == Left)
	{
		StartingPointX -= RoomWidth - 1;
	}
	else if (NormalBuildDirection == Down)
	{
		StartingPointY -= RoomHeight - 1;
	}

	//check tangent build direction
	if (TangentBuildDirection == Inverted)
	{
		if (NormalBuildDirection == Left || NormalBuildDirection == Right)
		{
			StartingPointY -= RoomHeight - 1;
		}
		else if (NormalBuildDirection == Down || NormalBuildDirection == Up)
		{
			StartingPointX -= RoomWidth - 1;
		}
	}

	//if there is space, position the room
	for (int X = StartingPointX; X < StartingPointX + RoomWidth; X++)
	{
		for (int Y = StartingPointY; Y < StartingPointY + RoomHeight; Y++)
		{
			UpdateBuildingCornerBlocks(X, Y);

		}
	}
}

//find the all the possible couples width and height of a specific area
std::vector<RoomWidthHeight>* Building::FindPossibleAspectRatios(int Area)
{
	std::vector<RoomWidthHeight>* PossibleAspectRatios = new std::vector<RoomWidthHeight>();

	//check if the number its prime, if so increase it by one unit
	while (CheckIfPrime(Area))
	{
		Area++;
	}

	//check the divisors of the area and take list of all the two numbers which multipled give the area as result, but they are not one greater than double the other
	//use this list of int couples as possible width and height of the room

	//if there are no good couples as divors, increase the number of one unit and try again
	while (PossibleAspectRatios->empty())
	{
		PossibleAspectRatios->clear();
		for (int divisor = 2; divisor <= Area / 2; divisor++)
		{

			if (Area % divisor == 0)
			{
				int Width = divisor;
				int Height = Area / divisor;

				//take the two values only if they are not one greater than double the other
				if ((Width >= Height && Width < Height * 2) || (Height >= Width && Height < Width * 2))
				{
					RoomWidthHeight RoomWidthHeight;
					RoomWidthHeight.Width = Width;
					RoomWidthHeight.Height = Height;
					PossibleAspectRatios->push_back(RoomWidthHeight);
				}
			}
		}
		Area++;
	}
	return PossibleAspectRatios;
}

//check if the block at those coordintates is in front of the front door facade
bool Building::IsInFrontOfFrontDoor(int X, int Y)
{
	if (X >= FrontSpaceLeftEdge && X <= FrontSpaceRightEdge && Y <= FrontSpaceTopEdge)
		return true;
	else
		return false;
}
//check if the number is prime
bool Building::CheckIfPrime(int number)
{
	if (number == 0 || number == 1) {
		return true;
	}
	else {
		for (int i = 2; i <= number / 2; ++i) {
			if (number % i == 0) {
				return false;
			}
		}
	}
	return true;
}

//add a room to the rooms list
Room Building::AddRoom(int Area, FString Name, int RoomId)
{
	Room newRoom = Room();
	newRoom.Area = Area;
	newRoom.RoomId = RoomId;
	newRoom.Name = Name;
	Rooms.push_back(&newRoom);
	return newRoom;
}

//position the room inside the grid
void Building::PositionRoom(bool WithCorridors, Room* currentRoom, BuildCoordinates BuildCoordinates)
{
	int StartingPointX = BuildCoordinates.StartingPointX;
	int StartingPointY = BuildCoordinates.StartingPointY;
	int RoomWidth = BuildCoordinates.RoomWidth;
	int RoomHeight = BuildCoordinates.RoomHeight;
	NormalDirection NormalBuildDirection = BuildCoordinates.NormalBuildDirection;
	TangentDirection TangentBuildDirection = BuildCoordinates.TangentBuildDirection;

	//check normal build direction
	if (NormalBuildDirection == Left)
	{
		StartingPointX -= RoomWidth - 1;
	}
	else if (NormalBuildDirection == Down)
	{
		StartingPointY -= RoomHeight - 1;
	}

	//check tangent build direction
	if (TangentBuildDirection == Inverted)
	{
		if (NormalBuildDirection == Left || NormalBuildDirection == Right)
		{
			StartingPointY -= RoomHeight - 1;
		}
		else if (NormalBuildDirection == Down || NormalBuildDirection == Up)
		{
			StartingPointX -= RoomWidth - 1;
		}
	}

	//if there is space, position the room
	for (int X = StartingPointX; X < StartingPointX + RoomWidth; X++)
	{
		for (int Y = StartingPointY; Y < StartingPointY + RoomHeight; Y++)
		{
			Block* CurrentBlock = GetBlock(X, Y);
			if (CurrentBlock == nullptr || (CurrentBlock != nullptr && CurrentBlock->BlockType == EmptyConnectedBlock))
			{
				BlockType BlockType;
				//if the block we are placing is on the edge
				if (X == StartingPointX || X == StartingPointX + RoomWidth - 1 || Y == StartingPointY || Y == StartingPointY + RoomHeight - 1)
				{
					BlockType = RoomEdgeBlock;
					//add corridor blocks around the room for connections
					if (WithCorridors)
					{
						CreateCorridorBlocks(X, Y, StartingPointX, StartingPointY, RoomWidth, RoomHeight);
					}
				}
				else
				{
					BlockType = RoomInternalBlock;
				}

				//add a new block if it doesn't exist or change is blocktype
				if (CurrentBlock == nullptr)
				{
					CurrentBlock = AddBlock(X, Y, BlockType, currentRoom);
				}
				else
				{
					CurrentBlock->BlockType = BlockType;
					CurrentBlock->OwnerRoom = currentRoom;
				}
				UpdateBuildingCornerBlocks(X, Y);
			}
		}
	}
}

void Building::UpdateBuildingCornerBlocks(int PosX, int PosY)
{
	//update the edge values of the building
	if (PosY < MinYValue)
	{
		MinYValue = PosY;
	}
	if (PosY > MaxYValue)
	{
		MaxYValue = PosY;
	}
	if (PosX < MinXValue)
	{
		MinXValue = PosX;
	}
	if (PosX > MaxXValue)
	{
		MaxXValue = PosX;
	}
}

//check if at that starting point it is possible to create a room with that specifications
bool Building::CheckIfSpaceAvaiable(int StartingPointX, int StartingPointY, int RoomWidth, int RoomHeight, NormalDirection NormalBuildDirection, TangentDirection TangentBuildDirection)
{
	//check normal build direction
	if (NormalBuildDirection == Left)
	{
		StartingPointX -= RoomWidth - 1;
	}
	else if (NormalBuildDirection == Down)
	{
		StartingPointY -= RoomHeight - 1;
	}

	//check tangent build direction
	if (TangentBuildDirection == Inverted)
	{
		if (NormalBuildDirection == Left || NormalBuildDirection == Right)
		{
			StartingPointY -= RoomHeight - 1;
		}
		else if (NormalBuildDirection == Down || NormalBuildDirection == Up)
		{
			StartingPointX -= RoomWidth - 1;
		}
	}

	//check if every block is free or if is an empty connected block
	for (int X = StartingPointX; X < StartingPointX + RoomWidth; X++)
	{
		for (int Y = StartingPointY; Y < StartingPointY + RoomHeight; Y++)
		{
			Block* CurrentBlock = GetBlock(X, Y);
			if ((CurrentBlock != nullptr && CurrentBlock->BlockType != EmptyConnectedBlock) || IsInFrontOfFrontDoor(X, Y))
			{
				return false;
			}
		}
	}
	return true;
}

Block* Building::GetBlock(int X, int Y)
{
	for (int i = 0; i < BuildingBlocks.size(); i++)
	{
		//the block exist so return false
		if (BuildingBlocks[i]->PosX == X && BuildingBlocks[i]->PosY == Y)
			return BuildingBlocks[i];
	}
	//the block do not exist so return true
	return nullptr;
}

Block* Building::AddBlock(int X, int Y, BlockType BlockType, Room* OwnerRoom)
{
	Block* block = new Block();
	block->BlockType = BlockType;
	block->PosX = X;
	block->PosY = Y;
	block->OwnerRoom = OwnerRoom;
	//add the block to the block list
	BuildingBlocks.push_back(block);

	//add the block to its room if it has one
	if (OwnerRoom != nullptr)
		OwnerRoom->RoomBlocks.push_back(block);
	return block;
}

//called for every edge block of a room, add a corridors block on the empty block around a specific block
void Building::CreateCorridorBlocks(int PosX, int PosY, int StartingPointX, int StartingPointY, int RoomWidth, int RoomHeight)
{
	NormalDirection BuildDirectionX;
	NormalDirection BuildDirectionY;
	for (int i = 1; i <= CorridorWidth + 1; i++)
	{
		int NewX = PosX;
		int NewY = PosY;

		//if the block in on the left side
		if (PosX == StartingPointX)
		{
			NewX = PosX - i;
			BuildDirectionX = Left;
		}//else if the block in on the right side
		else if (PosX == StartingPointX + RoomWidth - 1)
		{
			NewX = PosX + i;
			BuildDirectionX = Right;

		}
		//if the block in on the bottom side
		if (PosY == StartingPointY)
		{
			NewY = PosY - i;
			BuildDirectionY = Down;
		}//else if the block in on the top side
		else if (PosY == StartingPointY + RoomHeight - 1)
		{
			NewY = PosY + i;
			BuildDirectionY = Up;
		}

		if (NewX != PosX && !IsInFrontOfFrontDoor(NewX, PosY))
		{
			//get the free block next to the current room block
			Block* CurrentBlock = GetBlock(NewX, PosY);
			//if we are still building corridors
			if (i <= CorridorWidth)
			{
				//if it exists and it is an EmptyConnectedBlock, we can make it a corridor block
				if (CurrentBlock != nullptr && CurrentBlock->BlockType == EmptyConnectedBlock)
				{
					CurrentBlock->BlockType = CorridorBlock;
					CurrentBlock->OwnerRoom = nullptr;
				}
				else if (CurrentBlock == nullptr)
				{
					CurrentBlock = AddBlock(NewX, PosY, CorridorBlock, nullptr);
					UpdateBuildingCornerBlocks(NewX, PosY);
				}
				//add block to corridor vector
				CorridorBlocks.push_back(CurrentBlock);
			}
			else if (CurrentBlock == nullptr)//if the block is not busy, add a connected block beside the the corridor blocks
			{
				CurrentBlock = AddBlock(NewX, PosY, EmptyConnectedBlock, nullptr);
				EmptyConnectectBlocks.push_back(CurrentBlock);
				CurrentBlock->NormalDirection = BuildDirectionX;
			}
		}

		if (NewY != PosY && !IsInFrontOfFrontDoor(PosX, NewY))
		{
			//get the free block next to the current room block
			Block* CurrentBlock = GetBlock(PosX, NewY);
			//if we are still building corridors
			if (i <= CorridorWidth)
			{
				//if it exists and it is an EmptyConnectedBlock, we can make it a corridor block
				if (CurrentBlock != nullptr && CurrentBlock->BlockType == EmptyConnectedBlock)
				{
					CurrentBlock->BlockType = CorridorBlock;
					CurrentBlock->OwnerRoom = nullptr;
				}
				else if (CurrentBlock == nullptr)
				{
					CurrentBlock = AddBlock(PosX, NewY, CorridorBlock, nullptr);
					UpdateBuildingCornerBlocks(PosX, NewY);

				}
				//add block to corridor vector
				CorridorBlocks.push_back(CurrentBlock);
			}
			else if (CurrentBlock == nullptr)//if the block is not busy, add a connected block beside the the corridor blocks
			{
				CurrentBlock = AddBlock(PosX, NewY, EmptyConnectedBlock, nullptr);
				EmptyConnectectBlocks.push_back(CurrentBlock);
				CurrentBlock->NormalDirection = BuildDirectionY;
			}
		}
	}

	//put the corridor blocks on the corners
	if (((PosX == StartingPointX && PosY == StartingPointY) ||
		(PosX == StartingPointX && PosY == StartingPointY + RoomHeight - 1) ||
		(PosX == StartingPointX + RoomWidth - 1 && PosY == StartingPointY) ||
		(PosX == StartingPointX + RoomWidth - 1 && PosY == StartingPointY + RoomHeight - 1)) && !IsInFrontOfFrontDoor(PosX, PosY))
	{
		for (int i = 1; i <= CorridorWidth + 1; i++)
		{
			for (int j = 1; j <= CorridorWidth + 1; j++)
			{
				int NewX = PosX;
				int NewY = PosY;

				//if the block in on the left side
				if (PosX == StartingPointX)
				{
					NewX = PosX - i;
					BuildDirectionX = Left;
				}//else if the block in on the right side
				else if (PosX == StartingPointX + RoomWidth - 1)
				{
					NewX = PosX + i;
					BuildDirectionX = Right;

				}
				//if the block in on the bottom side
				if (PosY == StartingPointY)
				{
					NewY = PosY - j;
					BuildDirectionY = Down;
				}//else if the block in on the top side
				else if (PosY == StartingPointY + RoomHeight - 1)
				{
					NewY = PosY + j;
					BuildDirectionY = Up;
				}

				//get the free block next to the current room block
				Block* CurrentBlock = GetBlock(NewX, NewY);

				//if we are not in front of the front door facade
				if (!IsInFrontOfFrontDoor(NewX, NewY)) {
					//if we are still building corridors
					if (i <= CorridorWidth && j <= CorridorWidth)
					{
						//if it exists and it is an EmptyConnectedBlock, we can make it a corridor block
						if (CurrentBlock != nullptr && CurrentBlock->BlockType == EmptyConnectedBlock)
						{
							CurrentBlock->BlockType = CorridorBlock;
							CurrentBlock->OwnerRoom = nullptr;
						}
						else if (CurrentBlock == nullptr)
						{
							CurrentBlock = AddBlock(NewX, NewY, CorridorBlock, nullptr);
							UpdateBuildingCornerBlocks(NewX, NewY);
						}
						//add block to corridor vector
						CorridorBlocks.push_back(CurrentBlock);
					}
					else if (!(i > CorridorWidth && j > CorridorWidth) && CurrentBlock == nullptr)//if the block is not busy, add a connected block beside the the corridor blocks
					{
						CurrentBlock = AddBlock(NewX, NewY, EmptyConnectedBlock, nullptr);
						EmptyConnectectBlocks.push_back(CurrentBlock);
						if (i > j)
							CurrentBlock->NormalDirection = BuildDirectionX;
						else
							CurrentBlock->NormalDirection = BuildDirectionY;

					}
				}

			}
		}
	}
}



