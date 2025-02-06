// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h"

#include "Tables/tabledata/tabledata_resource.h"
#include "UI/Hexagon/UIHexagonTileDataBase.h"
#include "UI/Hexagon/UIHexagonTileView.h"
#include "ProceduralMesh/ProceduralMeshPlayground.h"

AMyLevelScriptActor::AMyLevelScriptActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void AMyLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
		if (IsValid(playerController))
		{
			playerController->SetViewTarget(Camera);
		}
	}

}

void AMyLevelScriptActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMyLevelScriptActor::EndPlay(const EEndPlayReason::Type inEndPlayReson)
{
	Super::EndPlay(inEndPlayReson);
}

void AMyLevelScriptActor::SetHexagonData(float inTileSize, int32 inColumn, int32 inCount, UUIHexagonTileView* inTileView)
{

	if (false == IsValid(inTileView))
		return;

	Viewer->BuildMeshHexagon(inTileSize, inColumn, inCount, inTileView);
}

void AMyLevelScriptActor::SetHexagonData(TSharedPtr<FRES_HEXAGONTILEMAP> inTableData)
{
	//데이터가 유효한가?
	if (false == inTableData.IsValid())
		return;

	if (false == IsValid(Viewer))
		return;

	Viewer->SetHexagonData(inTableData);
}

void AMyLevelScriptActor::ConvertToStaticMesh()
{
	if (false == IsValid(Viewer))
		return;

	Viewer->ConvertToStaticMesh();
}