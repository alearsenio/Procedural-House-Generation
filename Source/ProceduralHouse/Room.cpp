// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"

Room::Room()
{
}

Room::~Room()
{
}

void Room::FreeCorridorPath()
{
	for (int i = 0; i < PathBlocks.size(); i++)
	{
		PathBlocks[i]->isCorridorUsed = false;
	}
	PathBlocks.clear();
}


void Room::MoveRoom(MoveDirection Direction)
{
	int Xmovement = 0;
	int Ymovement = 0;

	switch (Direction)
	{
	case MoveLeft:
		Xmovement--;
		break;
	case MoveRight:
		Xmovement++;
		break;
	case MoveUp:
		Ymovement++;
		break;
	case MoveDown:
		Ymovement--;
		break;
	default:
		break;
	}

	for (int i = 0; i < RoomBlocks.size(); i++)
	{
		RoomBlocks[i]->PosX += Xmovement;
		RoomBlocks[i]->PosY += Xmovement;
	}
}
