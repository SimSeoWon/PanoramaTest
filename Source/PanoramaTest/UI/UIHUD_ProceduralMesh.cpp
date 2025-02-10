// Fill out your copyright notice in the Description page of Project Settings.

#include "UIHUD_ProceduralMesh.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Engine/LevelScriptActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/KismetRenderingLibrary.h" // for ReleaseRenderTarget2D()

#include "Widget/UIWidget_HeightMap.h"
#include "Hexagon/UIHexagonTileView.h"
#include "Hexagon/UIHexagonTileDataBase.h"
#include "../MyGameInstance.h"
#include "../MyPlayerController.h"
#include "../MyLevelScriptActor.h"
#include "../Tables/tabledata/tabledata_enum.h"
#include "../Tables/tabledata/tabledata_resource.h"

void UUIHUD_ProceduralMesh::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_CreateTile->OnClicked.AddDynamic(this, &UUIHUD_ProceduralMesh::OnClicked_CreateTile);
	Btn_Export->OnClicked.AddDynamic(this, &UUIHUD_ProceduralMesh::OnClicked_Export);
	Btn_SetMesh->OnClicked.AddDynamic(this, &UUIHUD_ProceduralMesh::OnClicked_SetMesh);
	Btn_Plus->OnClicked.AddDynamic(this, &UUIHUD_ProceduralMesh::OnClicked_Plus);
	Btn_Minus->OnClicked.AddDynamic(this, &UUIHUD_ProceduralMesh::OnClicked_Minus);
	Btn_LoadFile->OnClicked.AddDynamic(this, &UUIHUD_ProceduralMesh::OnClicked_LoadFile);
	HexagonView->OnSelectHexagonTile.BindUObject(this, &UUIHUD_ProceduralMesh::OnSelected_HexagonTile);

	OnClicked_LoadFile();

	OnClicked_CreateTile();
	//입력 모드 세팅
	UMyGameInstance* gameInstance = UMyGameInstance::Get();
	if (false == IsValid(gameInstance))
		return;

	AMyPlayerController* playerController = Cast<AMyPlayerController>(gameInstance->GetFirstLocalPlayerController());
	if (false == IsValid(playerController))
		return;

	playerController->SetInputMode(InputModeUIOnly);
}

void UUIHUD_ProceduralMesh::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUIHUD_ProceduralMesh::OnSelected_HexagonTile(const FVector2D inPos/**= FVector2D::Zero()*/)
{
	if (false == IsValid(HexagonView))
		return;

	UUIHexagonTileDataBase* selectTile = HexagonView->SelectTileData;
	if (false == IsValid(selectTile))
		return;

}

void UUIHUD_ProceduralMesh::CreateTile()
{
	// 절차적 생성 메시에 버텍스와 트라이앵글 세팅
	if (false == IsValid(HexagonView))
		return;

	float tileSize = 0.0f;
	int32 column = 0;
	int32 count = 0;
	if (IsValid(Editor_TileSize))
	{
		tileSize = FCString::Atof(*Editor_TileSize->GetText().ToString());
	}

	if (IsValid(Editor_Columnt))
	{
		column = FCString::Atoi(*Editor_Columnt->GetText().ToString());
	}

	if (IsValid(Editor_TotalCount))
	{
		count = FCString::Atoi(*Editor_TotalCount->GetText().ToString());
	}

	HexagonView->SetTileListItems(tileSize, column, count);
}

void UUIHUD_ProceduralMesh::OnClicked_CreateTile()
{
	CreateTile();

}

void UUIHUD_ProceduralMesh::OnClicked_SetMesh()
{
	// 월드에 있는 액터에 접근해야 한다.
	// 레벨 스크립트 액터를 만들자.

	float tileSize = 0.0f;
	int32 column = 0;
	int32 count = 0;
	if (IsValid(Editor_TileSize))
	{
		tileSize = FCString::Atof(*Editor_TileSize->GetText().ToString());
	}

	if (IsValid(Editor_Columnt))
	{
		column = FCString::Atoi(*Editor_Columnt->GetText().ToString());
	}

	if (IsValid(Editor_TotalCount))
	{
		count = FCString::Atoi(*Editor_TotalCount->GetText().ToString());
	}

	UWorld* world = GetWorld();
	if (false == IsValid(world))
		return;

	ALevelScriptActor* levelScript = world->GetLevelScriptActor();
	if (false == IsValid(levelScript))
		return;

	AMyLevelScriptActor* level = Cast<AMyLevelScriptActor>(levelScript);
	if (false == IsValid(level))
		return;

	level->SetHexagonData(tileSize, column, count, HexagonView);
}

void UUIHUD_ProceduralMesh::OnClicked_Export()
{
	//스테틱 메시로 생성
	UWorld* world = GetWorld();
	if (false == IsValid(world))
		return;

	ALevelScriptActor* levelScript = world->GetLevelScriptActor();
	if (false == IsValid(levelScript))
		return;

	AMyLevelScriptActor* level = Cast<AMyLevelScriptActor>(levelScript);
	if (false == IsValid(level))
		return;

	level->ConvertToStaticMesh();
}

void UUIHUD_ProceduralMesh::OnClicked_LoadFile()
{
	//tabledata를 로딩해서 육각형 타일 정보 수집.
	UDataTable* dtHexagonTileMap = LoadObject<UDataTable>(this, *FilePath);
	if (nullptr == dtHexagonTileMap)
		return;

	FString path = FPaths::ProjectDir() + CsvFilePath;
	path = FPaths::ConvertRelativePathToFull(path);

	if (false == FPlatformFileManager::Get().GetPlatformFile().FileExists(*path))
		return;

	UDataTableFunctionLibrary::FillDataTableFromCSVFile(dtHexagonTileMap, path);
	FRES_HEXAGONTILEMAP* rowData = dtHexagonTileMap->FindRow<FRES_HEXAGONTILEMAP>(FName(TEXT("TEST")), TEXT(""));
	if (nullptr == rowData)
		return;

	int32 count = rowData->TileList.Num();
	int32 column = rowData->Column;
	float tileSize = rowData->TileSize;
	FVector offset = rowData->Offset;

	if (IsValid(Editor_OffsetX))
	{
		FString strOffsetX = FString::Printf(TEXT("%f"), offset.X);
		Editor_OffsetX->SetText(FText::FromString(strOffsetX));
	}
	if (IsValid(Editor_OffsetY))
	{
		FString strOffsetY = FString::Printf(TEXT("%f"), offset.Y);
		Editor_OffsetY->SetText(FText::FromString(strOffsetY));
	}
	if (IsValid(Editor_OffsetZ))
	{
		FString strOffsetZ = FString::Printf(TEXT("%f"), offset.Z);
		Editor_OffsetZ->SetText(FText::FromString(strOffsetZ));
	}

	if (IsValid(Editor_TileSize))
	{
		FString strSize = FString::Printf(TEXT("%f"), tileSize);
		Editor_TileSize->SetText(FText::FromString(strSize));
	}

	if (IsValid(Editor_Columnt))
	{
		FString strColumn = FString::Printf(TEXT("%d"), column);
		Editor_Columnt->SetText(FText::FromString(strColumn));
	}

	if (IsValid(Editor_TotalCount))
	{
		FString strCount = FString::Printf(TEXT("%d"), count);
		Editor_TotalCount->SetText(FText::FromString(strCount));
	}

	CreateTile();

	for (int32 i = 0; i < rowData->TileList.Num(); i++)
	{
		EHexagonTileType type = rowData->TileList[i].TileType;
		EHexagonTileHeight height = rowData->TileList[i].HeightType;
		UUIHexagonTileDataBase* tileData = HexagonView->TileDataList[i];
		if (false == IsValid(tileData))
			continue;

		tileData->SetTileType(type);
		tileData->SetHeightType(height);
	}

	Widget_HeightMap->SetData(HexagonView);
}


void UUIHUD_ProceduralMesh::OnClicked_Plus()
{
	// 위젯에서만 확대 출소를 해주자
}


void UUIHUD_ProceduralMesh::OnClicked_Minus()
{
	// 위젯에서만 확대 출소를 해주자
}
