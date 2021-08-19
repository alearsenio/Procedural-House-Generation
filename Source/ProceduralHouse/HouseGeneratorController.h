// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HouseGeneratorController.generated.h"


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
	TSubclassOf<AActor> DoorMesh;
	UPROPERTY(EditAnywhere, Category = "House Componenets")
	TSubclassOf<AActor> ExternalWallMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Components")
	int GridWidht = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House Components")
	int GridHeight = 0;

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
