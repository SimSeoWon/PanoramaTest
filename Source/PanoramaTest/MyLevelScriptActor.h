// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "MyLevelScriptActor.generated.h"
class ACameraActor;
class AProceduralMeshPlayground;
class UUIHexagonTileDataBase;
class UUIHexagonTileView;
struct FRES_HEXAGONTILEMAP;
struct FHexagonTile;
/**
 * 
 */
UCLASS()
class PANORAMATEST_API AMyLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	AMyLevelScriptActor(const FObjectInitializer& ObjectInitializer);
	//void SetHexagonData(float tileSize, int32 column, int32 count, TArray<UUIHexagonTileDataBase*>& inListItems);
	void SetHexagonData(float tileSize, int32 column, int32 count, UUIHexagonTileView* inTileView);
	void SetHexagonData(TSharedPtr<FRES_HEXAGONTILEMAP> inTableData);
	void ConvertToStaticMesh();
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type inEndPlayReson) override;

protected:
	UPROPERTY(EditAnywhere)
	AProceduralMeshPlayground* Viewer = nullptr;

	UPROPERTY(EditAnywhere)
	ACameraActor* Camera = nullptr;

	//TArray<FHexagonTile> TileList ;
	
};
