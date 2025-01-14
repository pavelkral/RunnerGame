// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGameModeBase.h"
#include "FloorTile.h"
#include "GameHudWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

void ALevelGameModeBase::BeginPlay()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

	GameHud = Cast<UGameHudWidget>(CreateWidget(GetWorld(), GameHudClass));
	check(GameHud);

	GameHud->InitializeHUD(this);
	GameHud->AddToViewport();
	NumberOfLives = MaxLives;

	CreateInitialFloorTiles();
}

void ALevelGameModeBase::CreateInitialFloorTiles()
{


	AFloorTile* Tile = AddFloorTile(false);

	if (Tile)
	{
		LaneSwitchValues.Add(Tile->LeftLane->GetComponentLocation().Y);
		LaneSwitchValues.Add(Tile->CenterLane->GetComponentLocation().Y);
		LaneSwitchValues.Add(Tile->RightLane->GetComponentLocation().Y);
	}

	for (float Val : LaneSwitchValues)
	{
		UE_LOG(LogTemp, Warning, TEXT("LANE VALUE: %f"), Val);
	}

	AddFloorTile(false);
	AddFloorTile(false);

	for (int i = 0; i < NumInitialFloorTiles; i++)
	{
		AddFloorTile(true);
	}
}

AFloorTile* ALevelGameModeBase::AddFloorTile(const bool bSpawnItems)
{
	UWorld* World = GetWorld();

	if (World)
	{
		AFloorTile* Tile = World->SpawnActor<AFloorTile>(FloorTileClass, NextSpawnPoint);

		if (Tile)
		{
			FloorTiles.Add(Tile);

			if (bSpawnItems)
			{
				Tile->SpawnItems();
			}

			NextSpawnPoint = Tile->GetAttachTransform();

			if (GEngine)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("hit"));
			}

			//FVector MyVector = FVector(200.0f, 100.0f, 900.0f);
			FVector MyVector = Tile->GetActorLocation();
			if (GEngine)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Location is 1: %s"), *MyVector.ToString()));
			}

			//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Actor Name is 1: %s"), , *OtherActor->GetName()));
			//SCREEN_LOG("Name is: %s", *Tile->GetName());

		}
		return Tile;

	}

	return nullptr;
}

void ALevelGameModeBase::AddCoin()
{
	TotalCoins += 1;
	UE_LOG(LogTemp, Warning, TEXT("Coins : %i"), TotalCoins);
	OnCoinsCountChanged.Broadcast(TotalCoins);
}
void ALevelGameModeBase::PlayerDied()
{
	NumberOfLives -= 1;
	OnLivesCountChanged.Broadcast(NumberOfLives);

	if (NumberOfLives > 0)
	{
		// Iterate all FloorTiles and call DestroyFloorTile
		for (auto Tile : FloorTiles)
		{
			Tile->DestroyFloorTile();
		}

		// Empty our array
		FloorTiles.Empty();

		// set NextSpawnPoint to initial value
		NextSpawnPoint = FTransform();

		// Create out Initial floor tiles
		CreateInitialFloorTiles();

		// Broadcast level reset event
		OnLevelReset.Broadcast();
	}
	else
	{
		GameOver();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GAMEOVER"));
		}
	}
}
void ALevelGameModeBase::GameOver()
{
	if (IsValid(GameOverScreenClass))
	{
		UUserWidget* Widget = CreateWidget(GetWorld(), GameOverScreenClass);

		if (Widget)
		{
			Widget->AddToViewport();
		}
	}
}
void ALevelGameModeBase::RemoveTile(AFloorTile* Tile)
{
	if (FloorTiles.Contains(Tile))
	{
		FloorTiles.Remove(Tile);
	}
}

