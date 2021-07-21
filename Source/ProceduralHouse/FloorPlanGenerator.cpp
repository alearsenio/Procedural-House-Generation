// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorPlanGenerator.h"

FloorPlanGenerator::FloorPlanGenerator()
{
}

FloorPlanGenerator::~FloorPlanGenerator()
{
}

void FloorPlanGenerator::InitializeGrid(int width, int height)
{
    Width = width;
    Height = height;
    //initialize the grid with the specifc width and height and fill it with empty block
    for (size_t i = 0; i < Width; ++i)
    {
        std::vector<HouseBlock> Row;
        for (size_t j = 0; j < Height; ++j)
        {
            HouseBlock block;
            block.Occupied = false;
            block.RoomName = "null";
            Row.push_back(block);
        }
        Grid.push_back(Row); // push each row after you fill it
    }
}



