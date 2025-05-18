// Fill out your copyright notice in the Description page of Project Settings.


#include "RunCharacter.h"
#include "LevelGameModeBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "PauseMenu.h"

ARunCharacter::ARunCharacter()
{

	PrimaryActorTick.bCanEverTick = true;

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArmComp"));
	CameraArm->TargetArmLength = 450.f;
	CameraArm->SocketOffset = FVector(0.f, 0.f, 100.f);
	CameraArm->bUsePawnControlRotation = true;
	CameraArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
}

void ARunCharacter::BeginPlay()
{
	Super::BeginPlay();

	RunGameMode = Cast<ALevelGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	PauseMenu = Cast<UPauseMenu>(CreateWidget(GetWorld(), PauseMenuWidgetClass));

	check(RunGameMode);
	check(PauseMenu);

	RunGameMode->OnLevelReset.AddDynamic(this, &ARunCharacter::ResetLevel);

	PlayerStart = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()));

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		//PlayerController1 = PlayerController;

		PlayerController->SetInputMode(FInputModeGameOnly());

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


void ARunCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator ControlRot = GetControlRotation();
	ControlRot.Roll = 0.f;
	ControlRot.Pitch = 0.f;

	AddMovementInput(ControlRot.Vector());

}

void ARunCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARunCharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARunCharacter::StopJumping);
	//PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ARunCharacter::MoveLeft);
	//PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ARunCharacter::MoveRight);
	//PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ARunCharacter::MoveDown);
	//PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ARunCharacter::AddControllerYawInput);

	//PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ARunCharacter::MoveHorizontal);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Completed ,this, &ARunCharacter::MoveLeft);

		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Completed, this, &ARunCharacter::MoveRight);

		EnhancedInputComponent->BindAction(MoveHorizontalAction, ETriggerEvent::Triggered, this, &ARunCharacter::MoveHorizontal);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Completed, this, &ARunCharacter::PauseGame);

	}
}
void ARunCharacter::MoveHorizontal(const FInputActionValue& Value)
{
	//if (Value.get > 0)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("left : %f"), AxisValue);
	//	NextLane = FMath::Clamp(CurrentLane + 1, 0, 2);
	//	ChangeLane();
	//}
	//if (Value < 0){
		//UE_LOG(LogTemp, Warning, TEXT("right : %f"), AxisValue);
	//	NextLane = FMath::Clamp(CurrentLane - 1, 0, 2);
	//	ChangeLane();
	//}
	//AddMovementInput(FVector(0.0f, AxisValue * 10, 0.0f), 1.0f, false);
	//UE_LOG(LogTemp, Warning, TEXT("AXIS : %f"), AxisValue);

	FVector2D MovementVector = Value.Get<FVector2D>();

	UE_LOG(LogTemp, Warning, TEXT("AXIS : %f"), MovementVector.X);

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
	//	AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

}



void ARunCharacter::AddCoin()
{
	RunGameMode->AddCoin();
}

void ARunCharacter::ChangeLaneUpdate(const float Value)
{
	FVector Location = GetCapsuleComponent()->GetComponentLocation();
	Location.Y = FMath::Lerp(RunGameMode->LaneSwitchValues[CurrentLane], RunGameMode->LaneSwitchValues[NextLane], Value);
	SetActorLocation(Location);
	//UE_LOG(LogTemp, Warning, TEXT("C LANE : %s"), *Location.ToString());
}

void ARunCharacter::ChangeLaneFinished()
{
	CurrentLane = NextLane;
	//UE_LOG(LogTemp, Warning, TEXT("LANE : %f"), CurrentLane);
}
void ARunCharacter::MoveLeft()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("left: %s"), bIsPaused ? TEXT("true") : TEXT("false")));
	}

	NextLane = FMath::Clamp(CurrentLane - 1, 0, 2);
	ChangeLane();
}

void ARunCharacter::MoveRight()
{
	NextLane = FMath::Clamp(CurrentLane + 1, 0, 2);
	ChangeLane();
}

void ARunCharacter::MoveDown()
{
	static FVector Impulse = FVector(0, 0, MoveDownImpulse);
	GetCharacterMovement()->AddImpulse(Impulse, true);
}

void ARunCharacter::PauseGame()
{
	if (bIsPaused == false)

	{
		bIsPaused = true;
		UGameplayStatics::SetGamePaused(GetWorld(), true);		
		Widget = CreateWidget(GetWorld(), PauseMenuWidgetClass);

		if (Widget)
		{
			UE_LOG(LogTemp, Warning, TEXT("widget on"));
			Widget->AddToViewport();
			APlayerController* PlayerController = Cast<APlayerController>(Controller);
			PlayerController->SetInputMode(FInputModeGameAndUI());
		}

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Bool: %s"), bIsPaused ? TEXT("true") : TEXT("false")));
		}
		//UE_LOG(LogTemp, Warning, TEXT("Pause : %s"), bIsPaused);
	}
	else
	{
		bIsPaused = false;
		UGameplayStatics::SetGamePaused(GetWorld(), false);		
		//UE_LOG(LogTemp, Warning, TEXT("Pause : %s"), bIsPaused);
		if (Widget)
		{
			UE_LOG(LogTemp, Warning, TEXT("widget off"));
			Widget->RemoveFromParent();
			APlayerController* PlayerController = Cast<APlayerController>(Controller);
			PlayerController->SetInputMode(FInputModeGameOnly());
		}
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Bool: %s"), bIsPaused ? TEXT("true") : TEXT("false")));
		}
	}
}

void ARunCharacter::Death()
{
	

	if (!bIsDead)
	{
		const FVector Location = GetActorLocation();

		UWorld* World = GetWorld();


		if (World)
		{
			bIsDead = true;
			DisableInput(nullptr);

			if (DeathParticleSystem)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, DeathParticleSystem, Location);
			}

			if (DeathSound)
			{
				UGameplayStatics::PlaySoundAtLocation(World, DeathSound, Location);
			}

			GetMesh()->SetVisibility(false);
			UE_LOG(LogTemp, Warning, TEXT("DEATH"));
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("left: %s"), bIsPaused ? TEXT("true") : TEXT("false")));
			}
			World->GetTimerManager().SetTimer(RestartTimerHandle, this, &ARunCharacter::OnDeath, 1.f);
		}
	}
}
void ARunCharacter::OnDeath()
{
	//bIsDead = false;

	if (RestartTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(RestartTimerHandle);
	}
	//UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("RestartLevel"));
	RunGameMode->PlayerDied();
}

void ARunCharacter::ResetLevel()
{
	bIsDead = false;
	EnableInput(nullptr);
	GetMesh()->SetVisibility(true);

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	
	PlayerController->SetInputMode(FInputModeGameOnly());


	if (PlayerStart)
	{
		SetActorLocation(PlayerStart->GetActorLocation());
		SetActorRotation(PlayerStart->GetActorRotation());
	}
}



