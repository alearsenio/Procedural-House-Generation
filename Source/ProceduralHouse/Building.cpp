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

    //initialize the grid with the specifc width and height and fill it with empty blocks
    for (size_t Y = 0; Y < TerrainHeight; ++Y)
    {
        std::vector<Block> Row;
        for (size_t X = 0; X < TerrainWidth; ++X)
        {
            Block block;
            block.BlockType = Empty;
            block.PosX = X;
            block.PosY = Y;
            block.OwnerRoom = nullptr;
            Row.push_back(block);
        }
        HouseGrid.push_back(Row); // push each row after you fill it
    }
}

Building::~Building()
{
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
bool Building::PositionRoom(Room currentRoom, int StartingPointX, int StartingPointY, int RoomWidth, int RoomHeight, BuildDirection BuildDirectionX, BuildDirection BuildDirectionY)
{
    if (BuildDirectionX == Left)
    {
        StartingPointX -= RoomWidth - 1;
    }
    else if (BuildDirectionX == Down)
    {
        StartingPointY -= RoomHeight - 1;
    }
    ////check if there is space to position the room
    //for (int Y = StartingPointY; Y < StartingPointY + RoomHeight; Y++)
    //{
    //    for (int X = StartingPointX; X < StartingPointX + RoomWidth; X++)
    //    {
    //        if (X < 0 || Y < 0 || X >= RoomWidth || Y >= RoomHeight || (HouseGrid[Y][X].BlockType != Empty && HouseGrid[Y][X].BlockType != EmptyConnectedBlock))
    //        {
    //            return false;
    //        }
    //    }
    //}

    //if there is space, position the room
    for (int Y = StartingPointY; Y < StartingPointY + RoomHeight; Y++)
    {
        for (int X = StartingPointX; X < StartingPointX + RoomWidth; X++)
        {
            if (X >= 0 && Y >= 0) {
                //if the block we are placing is on the edge
                if (X == StartingPointX || X == StartingPointX + RoomWidth - 1 || Y == StartingPointY || Y == StartingPointY + RoomHeight - 1)
                {
                    HouseGrid[Y][X].BlockType = RoomEdgeBlock;
                    //add corridor blocks around the room for connections
                    CreateCorridorBlocks(X, Y, StartingPointX, StartingPointY, RoomWidth, RoomHeight);
                }
                else
                {
                    HouseGrid[Y][X].BlockType = RoomInternalBlock;
                }
                HouseGrid[Y][X].OwnerRoom = &currentRoom;
                currentRoom.RoomBlocks.push_back(HouseGrid[Y][X]);
            }
        }
    }
    return true;
}

//check if at that starting point it is possible to create a room with that specifications
bool Building::checkIfSpaceAvaiable(int StartingPointX, int StartingPointY, int width, int height, BuildDirection BuildDirection)
{
    
    return false;
}

//position all the rooms and generate the floor plan of the building
void Building::GenerateFloorPlan()  
{
    if (Rooms.size() > 0)
    {
        //position the first room
        PositionRoom(*Rooms[0], 4, 4, 2, 2, Up, Left);
        Block randomBlock = *EmptyConnectectBlocks[rand() % EmptyConnectectBlocks.size()];
        PositionRoom(*Rooms[1],7, 5, 2, 2, Up, Right);
        randomBlock = *EmptyConnectectBlocks[rand() % EmptyConnectectBlocks.size()];
         \zPositionRoom(*Rooms[2], randomBlock.PosX, randomBlock.PosY, 3, 2, randomBlock.BuildDirection, Left);


    }
}

//called for every edge block of a room, add a corridors block on the empty block around a specific block
void Building::CreateCorridorBlocks(int PosX, int PosY, int StartingPointX, int StartingPointY, int RoomWidth, int RoomHeight)
{

    //check every possible position of the block around the room
    //sides
    //check left side of the block
    for (int i = 1; i <= CorridorWidth + 1; i++) {
        if (PosX == StartingPointX && PosX - i >= 0 && (HouseGrid[PosY][PosX - i].BlockType == Empty || HouseGrid[PosY][PosX - i].BlockType == EmptyConnectedBlock))
        {
            //add corridor blocks
            if (i <= CorridorWidth) {
                HouseGrid[PosY][PosX - i].BlockType = CorridorBlock;
                CorridorBlocks.push_back(&HouseGrid[PosY][PosX - i]);
            }
            else //after adding the corridor blocks, add the adiacent empty block to the empty connected blocks vector
            {
                EmptyConnectectBlocks.push_back(&HouseGrid[PosY][PosX - i]);
                HouseGrid[PosY][PosX - i].BuildDirection = Left;
                HouseGrid[PosY][PosX - i].BlockType = EmptyConnectedBlock;

            }
        }
    }

    //check right side of the block
    for (int i = 1; i <= CorridorWidth + 1; i++) {
        if (PosX == StartingPointX + RoomWidth - 1 && PosX + i <= TerrainWidth - 1 && (HouseGrid[PosY][PosX + i].BlockType == Empty || HouseGrid[PosY][PosX + i].BlockType == EmptyConnectedBlock))
        {
            //add corridor blocks
            if (i <= CorridorWidth) {
                HouseGrid[PosY][PosX + i].BlockType = CorridorBlock;
                CorridorBlocks.push_back(&HouseGrid[PosY][PosX + i]);
            }
            else //after adding the corridor blocks, add the adiacent empty block to the empty connected blocks vector
            {
                EmptyConnectectBlocks.push_back(&HouseGrid[PosY][PosX + i]);
                HouseGrid[PosY][PosX + i].BuildDirection = Right;
                HouseGrid[PosY][PosX + i].BlockType = EmptyConnectedBlock;

            }
        }
    }

    //check down side of the block
    for (int i = 1; i <= CorridorWidth + 1; i++) {
        if (PosY == StartingPointY && PosY - i >= 0 && (HouseGrid[PosY - i][PosX].BlockType == Empty || HouseGrid[PosY - i][PosX].BlockType == EmptyConnectedBlock))
        {
            //add corridor blocks
            if (i <= CorridorWidth) {
                HouseGrid[PosY - i][PosX].BlockType = CorridorBlock;
                CorridorBlocks.push_back(&HouseGrid[PosY - i][PosX]);
            }
            else //after adding the corridor blocks, add the adiacent empty block to the empty connected blocks vector
            {
                EmptyConnectectBlocks.push_back(&HouseGrid[PosY - i][PosX]);
                HouseGrid[PosY - i][PosX].BuildDirection = Down;
                HouseGrid[PosY - i][PosX].BlockType = EmptyConnectedBlock;

            }
        }
    }

    //check up side of the block
    for (int i = 1; i <= CorridorWidth + 1; i++) {
        if (PosY == StartingPointY + RoomHeight - 1 && PosY + i <= TerrainHeight - 1 && (HouseGrid[PosY + i][PosX].BlockType == Empty || HouseGrid[PosY + i][PosX].BlockType == EmptyConnectedBlock))
        {
            //add corridor blocks
            if (i <= CorridorWidth) {
                HouseGrid[PosY + i][PosX].BlockType = CorridorBlock;
                CorridorBlocks.push_back(&HouseGrid[PosY + i][PosX]);
            }
            else //after adding the corridor blocks, add the adiacent empty block to the empty connected blocks vector
            {
                EmptyConnectectBlocks.push_back(&HouseGrid[PosY + i][PosX]);
                HouseGrid[PosY + i][PosX].BuildDirection = Up;
                HouseGrid[PosY + i][PosX].BlockType = EmptyConnectedBlock;

            }
        }
    }

    //corners 
    //add bottom left corners blocks
    for (int i = 1; i <= CorridorWidth + 1; i++) {
        for (int j = 1; j <= CorridorWidth + 1; j++) {
            if (PosX == StartingPointX && PosY == StartingPointY && PosY - i >= 0 && PosX - j >= 0 && (HouseGrid[PosY - i][PosX - j].BlockType == Empty || HouseGrid[PosY - i][PosX - j].BlockType == EmptyConnectedBlock))
            {
                //add corridor blocks
                if (i <= CorridorWidth && j <= CorridorWidth) 
                {
                    HouseGrid[PosY - i][PosX - j].BlockType = CorridorBlock;
                    CorridorBlocks.push_back(&HouseGrid[PosY - i][PosX - j]);
                }
                else if(!(i > CorridorWidth && j > CorridorWidth))
                {
                    EmptyConnectectBlocks.push_back(&HouseGrid[PosY - i][PosX - j]);
                    HouseGrid[PosY - i][PosX - j].BlockType = EmptyConnectedBlock;

                    if(i > j)
                        HouseGrid[PosY - i][PosX - j].BuildDirection = Down;
                    else
                        HouseGrid[PosY - i][PosX - j].BuildDirection = Left;

                }
            }
        }
    }

    //add top left corners blocks
    for (int i = 1; i <= CorridorWidth + 1; i++) {
        for (int j = 1; j <= CorridorWidth + 1; j++) {
            if (PosX == StartingPointX && PosY == StartingPointY + RoomHeight - 1 && PosY + i <= TerrainHeight - 1 && PosX - j >= 0 && (HouseGrid[PosY + i][PosX - j].BlockType == Empty || HouseGrid[PosY + i][PosX - j].BlockType == EmptyConnectedBlock))
            {
                //add corridor blocks
                if (i <= CorridorWidth && j <= CorridorWidth) 
                {
                    HouseGrid[PosY + i][PosX - j].BlockType = CorridorBlock;
                    CorridorBlocks.push_back(&HouseGrid[PosY + i][PosX - j]);
                }
                else if (!(i > CorridorWidth && j > CorridorWidth)) //after adding the corridor blocks, add the adiacent empty block to the empty connected blocks vector
                {
                    EmptyConnectectBlocks.push_back(&HouseGrid[PosY + i][PosX - j]);
                    HouseGrid[PosY + i][PosX - j].BlockType = EmptyConnectedBlock;

                    if (i > j)
                        HouseGrid[PosY + i][PosX - j].BuildDirection = Up;
                    else
                        HouseGrid[PosY + i][PosX - j].BuildDirection = Left;

                }
            }
        }
    }

    //add bottom right corners blocks
    for (int i = 1; i <= CorridorWidth + 1; i++) {
        for (int j = 1; j <= CorridorWidth + 1; j++) {
            if (PosX == StartingPointX + RoomWidth - 1 && PosY == StartingPointY && PosY - i >=0 && PosX + j <= TerrainWidth - 1 && (HouseGrid[PosY - i][PosX + j].BlockType == Empty || HouseGrid[PosY - i][PosX + j].BlockType == EmptyConnectedBlock))
            {
                //add corridor blocks
                if (i <= CorridorWidth && j <= CorridorWidth) 
                {
                    HouseGrid[PosY - i][PosX + j].BlockType = CorridorBlock;
                    CorridorBlocks.push_back(&HouseGrid[PosY - i][PosX + j]);
                }
                else if (!(i > CorridorWidth && j > CorridorWidth)) //after adding the corridor blocks, add the adiacent empty block to the empty connected blocks vector
                {
                    EmptyConnectectBlocks.push_back(&HouseGrid[PosY - i][PosX + j]);
                    HouseGrid[PosY - i][PosX + j].BlockType = EmptyConnectedBlock;

                    if (i > j)
                        HouseGrid[PosY - i][PosX + j].BuildDirection = Down;
                    else
                        HouseGrid[PosY - i][PosX + j].BuildDirection = Right;

                }
            }
        }
    }

    //add top right corners blocks
    for (int i = 1; i <= CorridorWidth + 1; i++) {
        for (int j = 1; j <= CorridorWidth + 1; j++) {
            if (PosX == StartingPointX + RoomWidth - 1 && PosY == StartingPointY + RoomHeight - 1 && PosY + i <= TerrainHeight - 1 && PosX - j <= TerrainWidth - 1 && (HouseGrid[PosY + i][PosX + j].BlockType == Empty || HouseGrid[PosY + i][PosX + j].BlockType == EmptyConnectedBlock))
            {
                //add corridor blocks
                if (i <= CorridorWidth && j <= CorridorWidth) 
                {
                    HouseGrid[PosY + i][PosX + j].BlockType = CorridorBlock;
                    CorridorBlocks.push_back(&HouseGrid[PosY + i][PosX + j]);
                }
                else if (!(i > CorridorWidth && j > CorridorWidth)) //after adding the corridor blocks, add the adiacent empty block to the empty connected blocks vector
                {
                    EmptyConnectectBlocks.push_back(&HouseGrid[PosY + i][PosX + j]);
                    HouseGrid[PosY + i][PosX + j].BlockType = EmptyConnectedBlock;

                    if (i > j)
                        HouseGrid[PosY + i][PosX + j].BuildDirection = Up;
                    else
                        HouseGrid[PosY + i][PosX + j].BuildDirection = Right;
                }
            }
        }
    }

}

