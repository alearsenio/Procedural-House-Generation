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
	//srand(static_cast <unsigned> (time(0)));
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
		PositionFirstRoom(Rooms[0]);

		//position all the public rooms
		BuildCoordinates BuildCoordinates;
		for (int RoomId = 1; RoomId < Rooms.size(); RoomId++)
		{
			if (Rooms[RoomId]->RoomType == Public)
			{
				BuildCoordinates = EvaluatesBuildPosition(Rooms[RoomId]);
				PositionRoom(true, Rooms[RoomId], BuildCoordinates);
			}
		}
		//position all the private rooms
		for (int RoomId = 1; RoomId < Rooms.size(); RoomId++)
		{
			if (Rooms[RoomId]->RoomType == Private)
			{
				BuildCoordinates = EvaluatesBuildPosition(Rooms[RoomId]);
				PositionRoom(true, Rooms[RoomId], BuildCoordinates);
			}
		}

		//remove all the empty connected cubes in the middle of corridors and transform the other ones into external walls
		//RemoveAllEmptyConnectedCubes();
		
		//find the shortest path to connect the rooms in the connections
		for (int i = 0; i < Connections.size(); i++)
			BFSShortestPathLength(Connections[i]);

		////expand every room if possible
		for (int i = 0; i < Rooms.size(); i++)
		{
			ExpandRoom(Rooms[i]);
		}

		//add missing edge blocks
		for (int i = 0; i < BuildingBlocks.size(); i++)
		{
			if (BuildingBlocks[i]->BlockType == RoomInternalBlock && CheckIfIsOnEdge(BuildingBlocks[i]))
			{
				BuildingBlocks[i]->BlockType = RoomEdgeBlock;
			}
		}
		
		//determine how many walls every the edge block need to positioned
		for (int i = 0; i < BuildingBlocks.size(); i++)
		{
			if (BuildingBlocks[i]->BlockType == RoomEdgeBlock)
			{
				CheckWallsOnEdgeBlock(BuildingBlocks[i]);
			}
		}

		//determine how many walls every the external block need to positioned
		for (int i = 0; i < BuildingBlocks.size(); i++)
		{
			if (BuildingBlocks[i]->BlockType == EmptyConnectedBlock)
			{
				CheckWallsOnExternalBlock(BuildingBlocks[i]);
			}
		}

	}
}

BuildCoordinates Building::EvaluatesBuildPosition(Room* CurrentRoom)
{
	int Area = CurrentRoom->Area;

	//find possible room's width and height combinations
	std::vector<RoomWidthHeight> PossibleAspectRatios = *FindPossibleAspectRatios(Area);
	std::vector<BuildCoordinates> PossibleBuildConfigurations;

	float ScoreSum = 0;
	int NumberOfCombinations = 0;
	//for every empty connected block avaiable
	for (int blockPos = 0; blockPos < BuildingBlocks.size(); blockPos++)
	{
		if (BuildingBlocks[blockPos]->BlockType == EmptyConnectedBlock)
		{
			Block* EmptyConnectedBlock = BuildingBlocks[blockPos];
			//for every configuration of aspect ratio for that room's area
			for (int AspectRatio = 0; AspectRatio < PossibleAspectRatios.size(); AspectRatio++)
			{
				//for both direction of building, normal and inverted
				TangentDirection TangentDirection = Normal;

				for (int TangentBuild = 0; TangentBuild <= 1; TangentBuild++)
				{
					if (TangentBuild == 1)
						TangentDirection = Inverted;

					int PosX = EmptyConnectedBlock->PosX;
					int PosY = EmptyConnectedBlock->PosY;
					int Width = PossibleAspectRatios[AspectRatio].Width;
					int Height = PossibleAspectRatios[AspectRatio].Height;

					//check if its possible to build a room with these specifications
					if (CheckIfSpaceAvailable(PosX, PosY, Width, Height, EmptyConnectedBlock->NormalDirection, TangentDirection))
					{
						BuildCoordinates BuildCoordinates;
						BuildCoordinates.Room = CurrentRoom;
						BuildCoordinates.StartingPointX = EmptyConnectedBlock->PosX;
						BuildCoordinates.StartingPointY = EmptyConnectedBlock->PosY;
						BuildCoordinates.RoomWidth = PossibleAspectRatios[AspectRatio].Width;
						BuildCoordinates.RoomHeight = PossibleAspectRatios[AspectRatio].Height;
						BuildCoordinates.NormalBuildDirection = EmptyConnectedBlock->NormalDirection;
						BuildCoordinates.TangentBuildDirection = TangentDirection;
						BuildCoordinates.score = EvaluateBuildCoordinatesScore(BuildCoordinates);
						ScoreSum += BuildCoordinates.score;
						//UE_LOG(LogTemp, Warning, TEXT("score: %f "), BuildCoordinates.score);
						NumberOfCombinations++;
						PossibleBuildConfigurations.push_back(BuildCoordinates);
					}
				}
			}
		}
	}
	if (!PossibleBuildConfigurations.empty())
	{
		int randomId = ChooseRandomBestPosition(PossibleBuildConfigurations, ScoreSum, NumberOfCombinations);
		return PossibleBuildConfigurations[randomId];
	}

	float bestScore = 0;
	int index = 0;

	for (int i = 0; i < PossibleBuildConfigurations.size(); i++)
	{
		if (PossibleBuildConfigurations[i].score > bestScore)
		{
			bestScore = PossibleBuildConfigurations[i].score;
			index = i;
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Chosen Score: %f"), bestScore);
	return PossibleBuildConfigurations[index];
	return BuildCoordinates();
}

int Building::ChooseRandomBestPosition(std::vector<BuildCoordinates>& PossibleBuildConfigurations, float ScoreSum, int NumberOfCombinations)
{
	//generate a random value between 0 and ScoreSum
	float RandomNumber = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / ScoreSum));

	//UE_LOG(LogTemp, Warning, TEXT("ScoreSum: %f NumberOfCombinations: %d, random number: %f"), ScoreSum, NumberOfCombinations, RandomNumber);

	float Score = 0;
	for (int i = 0; i < NumberOfCombinations; i++)
	{
		Score = PossibleBuildConfigurations[i].score;
		if (RandomNumber < Score)
		{
			//UE_LOG(LogTemp, Warning, TEXT("chosen score: %f "), Score);
			return i;
		}
		RandomNumber -= Score;
	}

	int randomIndex = rand() % PossibleBuildConfigurations.size();
	Score = PossibleBuildConfigurations[randomIndex].score;
	//UE_LOG(LogTemp, Warning, TEXT("chosen score: %f "), Score);
	return randomIndex;
}

//evaluate the score of positiong the room with that speficic configuration
float Building::EvaluateBuildCoordinatesScore(BuildCoordinates BuildCoordinates)
{

	float Score = 0;

	//EVALUATE SPACE USED

	//save current building size values
	int OldMinXValue = MinXValue;
	int OldMaxXValue = MaxXValue;
	int OldMinYValue = MinYValue;
	int OldMaxYValue = MaxYValue;

	//temporary position the room without corridors, updating the building size values
	PositionGhostRoom(BuildCoordinates);

	//calculate the space used by the building including the empty blocks inside
	float BuildingWidth = (MaxXValue + 1 - MinXValue);
	float BuildinghHeight = (MaxYValue + 1 - MinYValue);
	float AreaUsed = BuildingWidth * BuildinghHeight;
	float BlocksUsed = 0;

	//count actual blocks used in that area
	for (int i = 0; i < BuildingBlocks.size(); i++)
	{
		if (BuildingBlocks[i]->BlockType != EmptyConnectedBlock)
			BlocksUsed++;
	}

	//add blocks used by the room
	BlocksUsed += BuildCoordinates.Room->Area;

	//calculate the ratio blocks in area/blocks really used
	float RatioOfSpaceUsed = BlocksUsed / AreaUsed;

	//penalise ratio undere 0.5
	if (RatioOfSpaceUsed < 0.5f)
		RatioOfSpaceUsed = 0;

	//add the value to the final score
	//UE_LOG(LogTemp, Warning, TEXT("Score = %f"), Score);

	//penalise the score the more the building's height and witdth are different, to prioritise squared floor plans
	float HeightWidthRatios = 1;
	if (BuildingWidth <= BuildinghHeight)
		HeightWidthRatios = BuildingWidth / BuildinghHeight;
	else
		HeightWidthRatios = BuildinghHeight / BuildingWidth;

	Score += HeightWidthRatios;
	Score *= RatioOfSpaceUsed;

	//UE_LOG(LogTemp, Warning, TEXT("HeightWidthRatio: %f, Score = %f"), HeightWidthRatios, Score);

	//penalise the score the more the house width and height get closer to the terrain boundaries
	float widthDifference = 1;
	if (BuildingWidth <= TerrainWidth)
		widthDifference = (float)TerrainWidth - BuildingWidth;
	else
		widthDifference = TerrainWidth / BuildingWidth;

	float heightDifference = 1;
	if (BuildinghHeight <= TerrainHeight)
		heightDifference = (float)TerrainHeight - BuildinghHeight;
	else
		heightDifference = (float)TerrainHeight / BuildinghHeight;

	Score *= widthDifference * heightDifference;
	//UE_LOG(LogTemp, Warning, TEXT("widthDifference:%f, heightDifference: %f, Score = %f"), widthDifference, heightDifference, Score);

	//EVALUATE DISTANCE BETWEEN CONNECTED ROOMS
	float SumOfDistances = 0;
	for (int i = 0; i < BuildCoordinates.Room->RoomConnections.size(); i++)
	{
		Room* ConnectedRoom;
		if (BuildCoordinates.Room->RoomConnections[i]->Room1 != BuildCoordinates.Room)
			ConnectedRoom = BuildCoordinates.Room->RoomConnections[i]->Room1;
		else
			ConnectedRoom = BuildCoordinates.Room->RoomConnections[i]->Room2;

		if (ConnectedRoom->IsPositioned)
		{
			SumOfDistances += (float)EvaluateDistance(BuildCoordinates.StartingPointX, BuildCoordinates.StartingPointY, ConnectedRoom);
		}
	}
	Score /= SumOfDistances * SumOfDistances;
	//restore the old room size values
	MinXValue = OldMinXValue;
	MaxXValue = OldMaxXValue;
	MinYValue = OldMinYValue;
	MaxYValue = OldMaxYValue;

	return Score;
}

int Building::EvaluateDistance(int PosX, int PosY, Room* Room2)
{
	int Xdistance = abs(PosX - Room2->RoomBlocks[0]->PosX);
	int Ydistance = abs(PosY - Room2->RoomBlocks[0]->PosY);
	int totalDistance = Xdistance + Ydistance;
	int ShortestDistance = totalDistance;

	for (int i = 1; i < Room2->RoomBlocks.size(); i++)
	{
		if (Room2->RoomBlocks[i]->BlockType == RoomEdgeBlock)
		{
			Xdistance = abs(PosX - Room2->RoomBlocks[i]->PosX);
			Ydistance = abs(PosY - Room2->RoomBlocks[i]->PosY);
			totalDistance = Xdistance + Ydistance;
			if (totalDistance < ShortestDistance)
			{
				ShortestDistance = totalDistance;
			}
		}
	}
	return ShortestDistance;
}

void Building::RemoveAllEmptyConnectedCubes()
{
	BlockType BlockType;
	int BlockPosX = 0;
	int BlockPosY = 0;
	for (int i = 0; i < BuildingBlocks.size(); i++)
	{
		if (BuildingBlocks[i]->BlockType == EmptyConnectedBlock)
		{
			BlockPosX = BuildingBlocks[i]->PosX;
			BlockPosY = BuildingBlocks[i]->PosY;
			int OccupiedNeighboors = 0;
			if (GetBlock(BlockPosX, BlockPosY - 1))
				OccupiedNeighboors++;
			if (GetBlock(BlockPosX, BlockPosY + 1))
				OccupiedNeighboors++;
			if (GetBlock(BlockPosX - 1, BlockPosY))
				OccupiedNeighboors++;
			if (GetBlock(BlockPosX + 1, BlockPosY))
				OccupiedNeighboors++;
			if (OccupiedNeighboors >= 4)
			{
				BlockType = CorridorBlock;
			}
			else
			{
				BlockType = ExternalWall;
			}
			BuildingBlocks[i]->BlockType = BlockType;
		}
	}
}

void Building::FindPathToConnections(Room* Room)
{

}

void Building::BFSShortestPathLength(RoomConnection* Connection)
{
	int MinBlocksUsed = BuildingBlocks.size();

	std::vector<Block*> OptimalPath;
	std::vector<Block*> AnalisedBlocks;

	Room* Room1 = Connection->Room1;
	Room* Room2 = Connection->Room2;

	//UE_LOG(LogTemp, Warning, TEXT("connecting %s with %s"), *Room1->Name, *Room2->Name);

	for (int i = 1; i < Room1->RoomBlocks.size(); i++)
	{
		if (Room1->RoomBlocks[i]->BlockType == RoomEdgeBlock)
		{


			std::vector<Block*> PossiblePath;
			//check the blocks which would be used from this starting block to arrive to all the connected rooms

			//insert the first block into the queue
			std::queue<Block*> Queue;
			Queue.push(Room1->RoomBlocks[i]);
			bool RoomFound = false;

			//analise the queue until there are elements in it and there room is not found yet
			while (!Queue.empty() && !RoomFound)
			{
				Block* VisitingBlock = Queue.front();
				Queue.pop();
				//check condition of destination found immediately 

				for (int h = 0; h < 4; h++)
				{
					int BlockPosX = VisitingBlock->PosX;
					int BlockPosY = VisitingBlock->PosY;

					if (h == 0)
						BlockPosX++;
					else if (h == 1)
						BlockPosX--;
					else if (h == 2)
						BlockPosY++;
					else
						BlockPosY--;

					Block* NeighbourBlock = GetBlock(BlockPosX, BlockPosY);

					if (NeighbourBlock)
					{
						if (NeighbourBlock->BlockType == CorridorBlock && !NeighbourBlock->isVisited)
						{
							Queue.push(NeighbourBlock);
							NeighbourBlock->isVisited = true;
							NeighbourBlock->ParentBlockInPath = VisitingBlock;
							AnalisedBlocks.push_back(NeighbourBlock);
						}
						else if (NeighbourBlock->BlockType == RoomEdgeBlock && NeighbourBlock->OwnerRoom == Room2)
						{
							RoomFound = true;
							Connection->AreConnected = true;
							Block* Block = VisitingBlock;
							while (Block && Block != Room1->RoomBlocks[i])
							{
								PossiblePath.push_back(Block);
								Block = Block->ParentBlockInPath;
							}
							break;
						}
					}
				}
			}
			//put back all analised block to unvisited status and put their path parent block to null
			for (int f = 0; f < AnalisedBlocks.size(); f++)
			{
				AnalisedBlocks[f]->isVisited = false;
				AnalisedBlocks[f]->ParentBlockInPath = nullptr;
			}
			//if the current blocks used to arrive to every destinations, are less then the previous trials, update the optimal solution
			if (PossiblePath.size() < MinBlocksUsed && PossiblePath.size() > 0)
			{


				MinBlocksUsed = PossiblePath.size();
				OptimalPath = PossiblePath;

			}
		}
	}
	//mark all the corridor blocks as used
	for (int i = 0; i < OptimalPath.size(); i++)
	{
		OptimalPath[i]->isCorridorUsed = true;
		OptimalPath[i]->ConnectionsUsingBlock.push_back(Connection);
		//MarkNeighboursCorridorBlocks(OptimalPath[i], &OptimalPath);
		Connection->ConnectionPath = OptimalPath;
	}
}

void Building::MarkNeighboursCorridorBlocks(Block* CurrentBlock, std::vector<Block*>* PossiblePath)
{
	for (int i = 0; i < 4; i++)
	{
		int BlockPosX = CurrentBlock->PosX;
		int BlockPosY = CurrentBlock->PosY;

		if (i == 0)
			BlockPosX++;
		else if (i == 1)
			BlockPosX--;
		else if (i == 2)
			BlockPosY++;
		else
			BlockPosY--;

		Block* NeighbourBlock = GetBlock(BlockPosX, BlockPosY);
		if (NeighbourBlock && NeighbourBlock->CorridorId == CurrentBlock->CorridorId && !NeighbourBlock->isCorridorUsed)
		{
			NeighbourBlock->isCorridorUsed = true;
			PossiblePath->push_back(NeighbourBlock);
			MarkNeighboursCorridorBlocks(NeighbourBlock, PossiblePath);
		}
	}
}

bool Building::ExpandRoom(Room* Room)
{
	bool IsExpanded = false;
	int BlocksNumber = Room->RoomBlocks.size();
	for (int i = 0; i < BlocksNumber; i++)
	{
		if (Room->RoomBlocks[i]->BlockType == RoomEdgeBlock)
		{
			//UE_LOG(LogTemp, Warning, TEXT("expand BLOCK pos %d %d"), Room->RoomBlocks[i]->PosX, Room->RoomBlocks[i]->PosY);

			//gahter information for the corner expansion
			int FreeNeighboursCounter = 0;
			int CornerBlockPosX = 0;
			int CornerBlockPosY = 0;
			NormalDirection FirstDirection;
			NormalDirection SecondDirection;
			bool OneSideExpanded = false;
			//check every side of the block to see in which direction it is possible to expand the block
			Block* CurrentBlock = Room->RoomBlocks[i];
			bool EndReach = false;
			for (int j = 0; j < 4; j++)
			{
				int BlockPosX = CurrentBlock->PosX;
				int BlockPosY = CurrentBlock->PosY;
				NormalDirection Direction;

				switch (j)
				{
				case 0:
					BlockPosX--;
					Direction = Left;
					break;
				case 1:
					BlockPosX++;
					Direction = Right;

					break;
				case 2:
					BlockPosY++;
					Direction = Up;

					break;
				case 3:
					BlockPosY--;
					Direction = Down;
					break;
				default:
					break;
				}

				Block* NeighbourBlock = GetBlock(BlockPosX, BlockPosY);
				if (NeighbourBlock && NeighbourBlock->BlockType == CorridorBlock && (!NeighbourBlock->isCorridorUsed || IsCorridorUsedByThisRoom(NeighbourBlock, Room)))
				{
					if (j <= 1)
					{
						CornerBlockPosX = BlockPosX - CurrentBlock->PosX;
					}
					else
					{
						CornerBlockPosY = BlockPosY - CurrentBlock->PosY;
					}
					FreeNeighboursCounter++;

					//save direction for expansion of the corner
					if (FreeNeighboursCounter == 1)
						FirstDirection = Direction;
					else
						SecondDirection = Direction;

					/*NeighbourBlock->BlockType = RoomEdgeBlock;
					CurrentBlock->BlockType = RoomInternalBlock;
					NeighbourBlock->OwnerRoom = Room;
					Room->RoomBlocks.push_back(NeighbourBlock);
					IsExpanded = true;*/
					//UE_LOG(LogTemp, Warning, TEXT(" trying expanding BLOCK pos %d %d"), CurrentBlock->PosX, CurrentBlock->PosY);
					if (ExpandInDirection(Room, NeighbourBlock, Direction, 1))
					{
						OneSideExpanded = true;
						CurrentBlock->BlockType = RoomInternalBlock;
					}
				}
				else if (NeighbourBlock && NeighbourBlock->OwnerRoom != Room)
				{
					if (j <= 1)
					{
						CornerBlockPosX = BlockPosX - CurrentBlock->PosX;
					}
					else
					{
						CornerBlockPosY = BlockPosY - CurrentBlock->PosY;
					}
					FreeNeighboursCounter++;
					//save direction for expansion of the corner
					if (FreeNeighboursCounter == 1)
						FirstDirection = Direction;
					else
						SecondDirection = Direction;
				}
				//else
					//UE_LOG(LogTemp, Warning, TEXT(" can't expand BLOCK pos %d %d"), CurrentBlock->PosX, CurrentBlock->PosY);
			}
			//if you are a corner and a least one side managed to exand, try to expand diagonally
			if (FreeNeighboursCounter >= 2 && OneSideExpanded)
			{
				//UE_LOG(LogTemp, Warning, TEXT("expanding corner pos %d %d"), CurrentBlock->PosX + CornerBlockPosX, CurrentBlock->PosY + CornerBlockPosY);
				for (int k = 1; k <= CorridorWidth; k++)
				{
					Block* NeighbourBlock = GetBlock(CurrentBlock->PosX + CornerBlockPosX * k, CurrentBlock->PosY + CornerBlockPosY * k);
					if (NeighbourBlock && NeighbourBlock->BlockType == CorridorBlock && (!NeighbourBlock->isCorridorUsed || IsCorridorUsedByThisRoom(NeighbourBlock, Room)))
					{
						if (ExpandInDirection(Room, NeighbourBlock, FirstDirection, k))
						{
							CurrentBlock->BlockType = RoomInternalBlock;
						}
						if (ExpandInDirection(Room, NeighbourBlock, SecondDirection, k))
						{
							CurrentBlock->BlockType = RoomInternalBlock;
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	//if the check on the line reach a room door without any path, expand
	//if it reach a wall, delete
	return IsExpanded;
}

bool Building::ExpandInDirection(Room* CurrentRoom, Block* CurrentBlock, NormalDirection Direction, int ExploredBlocks)
{

	int BlockPosX = CurrentBlock->PosX;
	int BlockPosY = CurrentBlock->PosY;

	switch (Direction)
	{
	case Left:
		BlockPosX--;
		break;
	case Right:
		BlockPosX++;
		break;
	case Up:
		BlockPosY++;
		break;
	case Down:
		BlockPosY--;
		break;
	default:
		break;
	}

	Block* NeighbourBlock = GetBlock(BlockPosX, BlockPosY);
	if (NeighbourBlock)
	{
		//if the expansion meet a corridor block which is not used for a path, or it is used by the room exploring, move forward
		if (NeighbourBlock->BlockType == CorridorBlock && (!NeighbourBlock->isCorridorUsed || IsCorridorUsedByThisRoom(NeighbourBlock, CurrentRoom)))
		{
			//explore no deeper than the corridor width
			if (ExploredBlocks < CorridorWidth)
			{
				bool CanExpand = ExpandInDirection(CurrentRoom, NeighbourBlock, Direction, ExploredBlocks + 1);

				if (CanExpand)
				{
					CurrentBlock->BlockType = RoomInternalBlock;
					CurrentRoom->RoomBlocks.push_back(CurrentBlock);
					CurrentBlock->OwnerRoom = CurrentRoom;
				}
				else
				{
					//CurrentBlock->isCorridorUsed = true;
				}

				return CanExpand;
			}
			else
			{
				CurrentBlock->BlockType = RoomEdgeBlock;
				CurrentBlock->NormalDirection = Direction;
				CurrentRoom->RoomBlocks.push_back(CurrentBlock);
				CurrentBlock->OwnerRoom = CurrentRoom;
				return true;
			}
		}
		//if the expansion meet a corridor block which is used by a path which is not of this room do nothing
		else if (NeighbourBlock->BlockType == CorridorBlock && NeighbourBlock->isCorridorUsed)
		{
			//CurrentBlock->isCorridorUsed = true;
			return false;
		}
		//if the expansion arrive to the edge of the house without interruption, expand
		else if (NeighbourBlock->BlockType == RoomEdgeBlock || NeighbourBlock->BlockType == RoomInternalBlock || NeighbourBlock->BlockType == EmptyConnectedBlock)
		{
			/*if (NeighbourBlock->BlockType == RoomEdgeBlock && CheckRoomsConnection(CurrentRoom, NeighbourBlock->OwnerRoom))
			{
				CurrentBlock->BlockType = DoorBlock;
				NeighbourBlock->BlockType = DoorBlock;
			}
			else
			{
				CurrentBlock->BlockType = RoomEdgeBlock;
			}*/
			CurrentBlock->BlockType = RoomEdgeBlock;

			CurrentBlock->NormalDirection = Direction;
			CurrentBlock->OwnerRoom = CurrentRoom;
			CurrentRoom->RoomBlocks.push_back(CurrentBlock);
			return true;
		}

	}
	CurrentBlock->BlockType = RoomEdgeBlock;
	CurrentBlock->OwnerRoom = CurrentRoom;
	CurrentRoom->RoomBlocks.push_back(CurrentBlock);
	return true;
}

bool Building::IsCorridorUsedByThisRoom(Block* CorridorBlock, Room* CurrentRoom)
{
	//check if that corridor block is used by this room
	for (int i = 0; i < CorridorBlock->ConnectionsUsingBlock.size(); i++)
	{
		bool ConnectionFound = false;
		for (int j = 0; j <= CurrentRoom->RoomConnections.size(); j++)
		{
			if (CorridorBlock->ConnectionsUsingBlock[i] && CurrentRoom->RoomConnections[j] && CorridorBlock->ConnectionsUsingBlock[i] == CurrentRoom->RoomConnections[j])
				ConnectionFound = true;
		}

		if (!ConnectionFound)
			return false;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Block %d %d can be erased"), CorridorBlock->PosX, CorridorBlock->PosY);
	return true;
}

bool Building::CheckRoomsConnection(Room* Room1, Room* Room2)
{
	for (int i = 0; i < Room1->RoomConnections.size(); i++)
	{
		if ((Room1->RoomConnections[i]->Room1 == Room2 || Room1->RoomConnections[i]->Room2 == Room2) && !Room1->RoomConnections[i]->HasDoor)
		{
			Room1->RoomConnections[i]->HasDoor = true;
			return true;
		}
	}
	return false;
}

bool Building::CheckIfIsOnEdge(Block* CurrentBlock)
{
	//check every side block to see if one is not one of your room blocks
	for (int j = 0; j < 4; j++)
	{
		int BlockPosX = CurrentBlock->PosX;
		int BlockPosY = CurrentBlock->PosY;
		NormalDirection Direction;

		switch (j)
		{
		case 0:
			BlockPosX--;
			Direction = Left;
			break;
		case 1:
			BlockPosX++;
			Direction = Right;

			break;
		case 2:
			BlockPosY++;
			Direction = Up;

			break;
		case 3:
			BlockPosY--;
			Direction = Down;
			break;
		default:
			break;
		}

		Block* NeighbourBlock = GetBlock(BlockPosX, BlockPosY);

		if (!NeighbourBlock || (!NeighbourBlock->OwnerRoom || NeighbourBlock->OwnerRoom != CurrentBlock->OwnerRoom))
		{
			CurrentBlock->NormalDirection = Direction;
			return true;
		}
	}
	//check every corner block to see if one is not one of your room blocks
	/*for (int j = 0; j < 4; j++)
	{
		int BlockPosX = CurrentBlock->PosX;
		int BlockPosY = CurrentBlock->PosY;
		NormalDirection Direction;
		switch (j)
		{
		case 0:
			BlockPosX--;
			BlockPosY--;
			Direction = Left;
			break;
		case 1:
			BlockPosX++;
			BlockPosY++;
			Direction = Right;
			break;
		case 2:
			BlockPosX--;
			BlockPosY++;
			Direction = Up;
			break;
		case 3:
			BlockPosX++;
			BlockPosY--;
			Direction = Down;
			break;
		default:
			break;
		}
		Block* NeighbourBlock = GetBlock(BlockPosX, BlockPosY);
		if (!NeighbourBlock || (!NeighbourBlock->OwnerRoom || NeighbourBlock->OwnerRoom != CurrentBlock->OwnerRoom))
		{
			CurrentBlock->NormalDirection = Direction;
			return true;
		}
	}*/
	return false;
}

void Building::CheckWallsOnEdgeBlock(Block* CurrentBlock)
{
	//check every side block to see if one is not one of your room blocks
	for (int j = 0; j < 4; j++)
	{
		int BlockPosX = CurrentBlock->PosX;
		int BlockPosY = CurrentBlock->PosY;
		NormalDirection Direction;

		switch (j)
		{
		case 0:
			BlockPosX--;
			Direction = Left;
			break;
		case 1:
			BlockPosX++;
			Direction = Right;

			break;
		case 2:
			BlockPosY++;
			Direction = Up;

			break;
		case 3:
			BlockPosY--;
			Direction = Down;
			break;
		default:
			break;
		}

		Block* NeighbourBlock = GetBlock(BlockPosX, BlockPosY);

		if (!NeighbourBlock || (!NeighbourBlock->OwnerRoom || NeighbourBlock->OwnerRoom != CurrentBlock->OwnerRoom))
		{
			CurrentBlock->WallsDirection.push_back(Direction);
		}
	}
}

void Building::CheckWallsOnExternalBlock(Block* CurrentBlock)
{
	//check every side block to see if it belongs to a a room or a corridor
	for (int j = 0; j < 4; j++)
	{
		int BlockPosX = CurrentBlock->PosX;
		int BlockPosY = CurrentBlock->PosY;
		NormalDirection Direction;

		switch (j)
		{
		case 0:
			BlockPosX--;
			Direction = Left;
			break;
		case 1:
			BlockPosX++;
			Direction = Right;

			break;
		case 2:
			BlockPosY++;
			Direction = Up;

			break;
		case 3:
			BlockPosY--;
			Direction = Down;
			break;
		default:
			break;
		}

		Block* NeighbourBlock = GetBlock(BlockPosX, BlockPosY);

		if (NeighbourBlock && NeighbourBlock->BlockType != EmptyConnectedBlock)
		{
			CurrentBlock->WallsDirection.push_back(Direction);
		}
	}
}

void Building::InserWallsInFrontOfStoop()
{
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
Room Building::AddRoom(int Area, FString Name, int RoomId, RoomType RoomType, Building* Building)
{
	Room newRoom = Room();
	newRoom.Area = Area;
	newRoom.RoomId = RoomId;
	newRoom.Name = Name;
	newRoom.RoomType = RoomType;
	newRoom.OwnerBuilding = Building;
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

	//position the room, the space was already checked in the CheckAvailableSpace Function
	for (int X = StartingPointX; X < StartingPointX + RoomWidth; X++)
	{
		for (int Y = StartingPointY; Y < StartingPointY + RoomHeight; Y++)
		{
			Block* CurrentBlock = GetBlock(X, Y);
			BlockType BlockType;

			//if the block we are placing is on the edge
			if (X == StartingPointX || X == StartingPointX + RoomWidth - 1 || Y == StartingPointY || Y == StartingPointY + RoomHeight - 1)
			{
				//UE_LOG(LogTemp, Warning, TEXT("edge BLOCK pos %d %d"), X, Y);
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

			//add a new block if it doesn't exist or change its blocktype if already exists
			if (CurrentBlock == nullptr)
			{
				CurrentBlock = AddBlock(X, Y, BlockType, currentRoom);
			}
			else
			{
				CurrentBlock->BlockType = BlockType;
				CurrentBlock->OwnerRoom = currentRoom;
				currentRoom->RoomBlocks.push_back(CurrentBlock);
			}
			UpdateBuildingCornerBlocks(X, Y);
		}
	}

	////get the block at the beginning of the positioning and make it the corridor door
	//Block* BlockDoor = GetBlock(StartingPointX, StartingPointY);
	//BlockDoor->BlockType = DoorBlock;
	//Door CorridorDoor;
	//CorridorDoor.OnCorridor = true;
	//CorridorDoor.FirstRoom = currentRoom;
	//CorridorDoor.SecondRoom = nullptr;
	//CorridorDoor.Block = BlockDoor;
	//currentRoom->RoomDoors.push_back(CorridorDoor);

	//set the room to positioned
	currentRoom->IsPositioned = true;
}

void Building::PositionFirstRoom(Room* Room)
{
	//position the first room
	std::vector<RoomWidthHeight> PossibleAspectRatios = *FindPossibleAspectRatios(Room->Area);
	
	if (PossibleAspectRatios.size() > 0)
	{
		//prepare coordinates for the first room
		BuildCoordinates BuildCoordinates;
		BuildCoordinates.StartingPointX = 0;
		BuildCoordinates.StartingPointY = 0;
		BuildCoordinates.RoomWidth = PossibleAspectRatios[0].Width;
		BuildCoordinates.RoomHeight = PossibleAspectRatios[0].Height;
		BuildCoordinates.NormalBuildDirection = Up;
		BuildCoordinates.TangentBuildDirection = Normal;

		//define the space for the stoop
		FrontSpaceLeftEdge = 0;
		FrontSpaceRightEdge = BuildCoordinates.RoomWidth - 1;
		FrontSpaceTopEdge = 0;

		PositionRoom(true, Rooms[0], BuildCoordinates);

		
	}
}

bool Building::AddConnection(Room* Room1, Room* Room2)
{
	for (int i = 0; i < Connections.size(); i++)
	{
		RoomConnection* Connection = Connections[i];
		if ((Connection->Room1 == Room1 && Connection->Room2 == Room2) || (Connection->Room1 == Room2 && Connection->Room2 == Room1))
		{
			return false;
		}
	}

	RoomConnection* Connection = new RoomConnection();
	Connection->Room1 = Room1;
	Connection->Room2 = Room2;
	Room1->RoomConnections.push_back(Connection);
	Room2->RoomConnections.push_back(Connection);
	Connections.push_back(Connection);
	return true;
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
bool Building::CheckIfSpaceAvailable(int StartingPointX, int StartingPointY, int RoomWidth, int RoomHeight, NormalDirection NormalBuildDirection, TangentDirection TangentBuildDirection)
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
	CorridorLinesCount++;

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
				CurrentBlock->CorridorId = CorridorLinesCount;

			}
			else if (CurrentBlock == nullptr)//if the block is not busy, add a connected block beside the the corridor blocks
			{
				CurrentBlock = AddBlock(NewX, PosY, EmptyConnectedBlock, nullptr);
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
				CurrentBlock->CorridorId = CorridorLinesCount;
			}
			else if (CurrentBlock == nullptr)//if the block is not busy, add a connected block beside the the corridor blocks
			{
				CurrentBlock = AddBlock(PosX, NewY, EmptyConnectedBlock, nullptr);
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
		CorridorLinesCount++;
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
						CurrentBlock->CorridorId = CorridorLinesCount;

					}
					else if (!(i > CorridorWidth && j > CorridorWidth) && CurrentBlock == nullptr)//if the block is not busy, add a connected block beside the the corridor blocks
					{
						CurrentBlock = AddBlock(NewX, NewY, EmptyConnectedBlock, nullptr);
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



