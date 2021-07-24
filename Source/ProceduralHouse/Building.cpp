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
		//position the first room
		PositionRoom(*Rooms[0], 0, 0, 8, 7, Up, Normal);
		bool positionFound = false;
		Block randomBlock;
		for (int i = 0; i<500; i++)
		{
			randomBlock = *EmptyConnectectBlocks[rand() % EmptyConnectectBlocks.size()];
			positionFound = CheckIfSpaceAvaiable(randomBlock.PosX, randomBlock.PosY, 4, 5, randomBlock.NormalDirection, Normal);
			if (positionFound)
				break;
		}
		positionFound = false;
		PositionRoom(*Rooms[1], randomBlock.PosX, randomBlock.PosY, 4, 5, randomBlock.NormalDirection, Normal);

		for (int i = 0; i < 500; i++)
		{
			randomBlock = *EmptyConnectectBlocks[rand() % EmptyConnectectBlocks.size()];
			positionFound = CheckIfSpaceAvaiable(randomBlock.PosX, randomBlock.PosY, 6, 4, randomBlock.NormalDirection, Normal);
			if (positionFound)
				break;
		}
		positionFound = false;
		PositionRoom(*Rooms[2], randomBlock.PosX, randomBlock.PosY, 6, 4, randomBlock.NormalDirection, Normal);

		for (int i = 0; i < 500; i++)
		{
			randomBlock = *EmptyConnectectBlocks[rand() % EmptyConnectectBlocks.size()];
			positionFound = CheckIfSpaceAvaiable(randomBlock.PosX, randomBlock.PosY, 4, 4, randomBlock.NormalDirection, Normal);
			if (positionFound)
				break;
		}
		positionFound = false;
		PositionRoom(*Rooms[3], randomBlock.PosX, randomBlock.PosY, 4, 4, randomBlock.NormalDirection, Normal);
	}
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
bool Building::PositionRoom(Room currentRoom, int StartingPointX, int StartingPointY, int RoomWidth, int RoomHeight, NormalDirection NormalBuildDirection, TangentDirection TangentBuildDirection)
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
					CreateCorridorBlocks(X, Y, StartingPointX, StartingPointY, RoomWidth, RoomHeight);
				}
				else
				{
					BlockType = RoomInternalBlock;
				}

				//add a new block if it doesn't exist or change is blocktype
				if (CurrentBlock == nullptr)
				{
					CurrentBlock = AddBlock(X, Y, BlockType, &currentRoom);
				}
				else
				{
					CurrentBlock->BlockType = BlockType;
					CurrentBlock->OwnerRoom = &currentRoom;
				}
			}
		}
	}
	return true;
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
			if ((CurrentBlock != nullptr && CurrentBlock->BlockType != EmptyConnectedBlock))
			{
				return false;
			}
		}
	}
	return true;
}

Block* Building::GetBlock(int X, int Y)
{
	for (int i = 0; i < HouseBlocks.size(); i++)
	{
		//the block exist so return false
		if (HouseBlocks[i]->PosX == X && HouseBlocks[i]->PosY == Y)
			return HouseBlocks[i];
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
	HouseBlocks.push_back(block);

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

		if (NewX != PosX)
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

		if (NewY != PosY)
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

	//put the corridor blocks on the corner
	if ((PosX == StartingPointX && PosY == StartingPointY) ||
		(PosX == StartingPointX && PosY == StartingPointY + RoomHeight - 1) ||
		(PosX == StartingPointX + RoomWidth - 1 && PosY == StartingPointY) ||
		(PosX == StartingPointX + RoomWidth - 1 && PosY == StartingPointY + RoomHeight - 1))
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



