// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

class UUserWidget;
class UMyUserWidgetBase;
class UUIHUD_ProceduralMesh;
struct FStreamableHandle;
/**
 * 
 */
UCLASS()
class PANORAMATEST_API AMyHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	UUIHUD_ProceduralMesh* Widget = nullptr;

	//TSharedPtr<FStreamableHandle> Handle;

	
};
