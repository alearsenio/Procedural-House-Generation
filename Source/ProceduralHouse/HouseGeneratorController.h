// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HouseGeneratorController.generated.h"
 
USTRUCT(BlueprintType, DisplayName = "Room Specs")
struct FRoom
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Area;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPrivate;

	FRoom()
	{
		Area = 10;
		Name = "Room";
		IsPrivate = false;
	}
};

USTRUCT(BlueprintType, DisplayName = "Connection")
struct FConnection
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int FirstRoomId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int SecondRoomId;

	FConnection()
	{
		FirstRoomId = 0;
		SecondRoomId = 0;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCEDURALHOUSE_API UHouseGeneratorController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHouseGeneratorController();

	UPROPERTY(EditAnywhere, Category = "House Componenets")
	TSubclassOf<AActor> PrivateRoomWallMesh;
	UPROPERTY(EditAnywhere, Category = "House Componenets")
	TSubclassOf<AActor> PrivateRoomFloorMesh;
	UPROPERTY(EditAnywhere, Category = "House Componenets")
	TSubclassOf<AActor> PublicRoomWallMesh;
	UPROPERTY(EditAnywhere, Category = "House Componenets")
	TSubclassOf<AActor> PublicRoomFloorMesh;
	UPROPERTY(EditAnywhere, Category = "House Componenets")
	TSubclassOf<AActor> CorridorFloorMesh;
	UPROPERTY(EditAnywhere, Category = "House Componenets")
		TSubclassOf<AActor> CorridorUsedFloorMesh;
	UPROPERTY(EditAnywhere, Category = "House Componenets")
	TSubclassOf<AActor> DoorMesh;
	UPROPERTY(EditAnywhere, Category = "House Componenets")
	TSubclassOf<AActor> ExternalWallMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Contraints")
	int GridWidht = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Contraints")
	int GridHeight = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Contraints")
	int CorridorWidth = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Contraints")
	TArray<FRoom> Rooms;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Contraints")
	TArray<FConnection> RoomsConnections;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:

	void PositionMesh();
	
	Building House;

	
};
