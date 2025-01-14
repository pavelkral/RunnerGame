// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "LevelGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "GameHudWidget.generated.h"

/**
 *
 */
UCLASS()
class RUNNERGAME_API UGameHudWidget : public UUserWidget
{
	GENERATED_BODY()

protected:


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* CoinsCount;


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* LivesCount;


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* PauseBtn;

	UFUNCTION()
	void OnPauseClick();

	virtual void NativeConstruct() override;


public:

	UFUNCTION(BlueprintCallable)
	void InitializeHUD(ALevelGameModeBase* RunGameMode);

	UFUNCTION(BlueprintCallable)
	void SetCoinsCount(int32 Count);


	UFUNCTION(BlueprintCallable)
	void SetLivesCount(int32 Count);
};
