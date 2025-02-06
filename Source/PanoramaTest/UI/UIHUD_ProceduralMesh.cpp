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

#pragma region HeightMap
	Slider_CompNumX->OnValueChanged.AddDynamic(this, &UUIHUD_ProceduralMesh::OnChanged_CompNumX);
	Slider_CompNumY->OnValueChanged.AddDynamic(this, &UUIHUD_ProceduralMesh::OnChanged_CompNumY);
	Slider_CompNumX->SetValue(1);
	Slider_CompNumY->SetValue(1);

	TestRenderTarget = NewObject<UTextureRenderTarget2D>();
	if (IsValid(TestRenderTarget))
	{
		TestRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
		TestRenderTarget->InitAutoFormat(256, 256);
		TestRenderTarget->UpdateResourceImmediate(true);
		Img_Render->Brush.SetResourceObject(TestRenderTarget);
	}

	Btn_SetHeightMap->OnClicked.AddDynamic(this, &UUIHUD_ProceduralMesh::OnClicked_SetHeightMap);

	if (IsValid(ComboBox_SectionSize))
	{
		ComboBox_SectionSize->OnSelectionChanged.AddDynamic(this, &UUIHUD_ProceduralMesh::OnChanged_SectionSize);

		const UEnum* ptrEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EQuadBySection"), true);
		if (ptrEnum)
		{
			ComboBox_SectionSize->ClearOptions();
			int32 count = ptrEnum->NumEnums();
			FString strType = TEXT("None");
			for (int32 i = 0; i < count; i++)
			{
				strType = ptrEnum->GetNameStringByIndex(i);
				ComboBox_SectionSize->AddOption(strType);
			}
		}
	}

	if (IsValid(ComboBox_ComponentsSection))
	{
		ComboBox_ComponentsSection->OnSelectionChanged.AddDynamic(this, &UUIHUD_ProceduralMesh::OnChanged_ComponentsSection);
		const UEnum* ptrEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESectionByComponent"), true);
		if (ptrEnum)
		{
			ComboBox_ComponentsSection->ClearOptions();
			int32 count = ptrEnum->NumEnums();
			FString strType = TEXT("None");
			for (int32 i = 0; i < count; i++)
			{
				strType = ptrEnum->GetNameStringByIndex(i);
				ComboBox_ComponentsSection->AddOption(strType);
			}
		}
	}

#pragma endregion HeightMap

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

	//UUIHexagonTileDataBase* selectTile = HexagonView->SelectTileData;
	//if (false == IsValid(selectTile))
	//	return;

	//Txt_TileIndex->SetText(FText::AsNumber(selectTile->Index));
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

void UUIHUD_ProceduralMesh::SetDetail()
{
	if (QuadBySection == EQuadBySection::None)
		return;

	if (SectionByComponent == ESectionByComponent::None)
		return;

	Scale = GetScale();
	Components = GetComponentsCount();
	SetResolution();
	SetTotalComponents();
}

FVector UUIHUD_ProceduralMesh::GetScale()
{
	FVector scale = FVector::Zero();
	Scale.X = FCString::Atof(*Editor_ScaleX->GetText().ToString());
	Scale.Y = FCString::Atof(*Editor_ScaleY->GetText().ToString());
	Scale.Z = FCString::Atof(*Editor_ScaleZ->GetText().ToString());
	return Scale;
}

void UUIHUD_ProceduralMesh::SetComponentsCount(FVector2D inComponentsCount)
{
	Components = inComponentsCount;
	Txt_CompNumX->SetText(FText::AsNumber((int32)Components.X));
	Txt_CompNumY->SetText(FText::AsNumber((int32)Components.Y));
}

FVector2D UUIHUD_ProceduralMesh::GetComponentsCount()
{
	FVector2D componentsCount = FVector2D::Zero();
	componentsCount.X = FCString::Atof(*Txt_CompNumX->GetText().ToString());
	componentsCount.Y = FCString::Atof(*Txt_CompNumY->GetText().ToString());
	return componentsCount;
}

void UUIHUD_ProceduralMesh::SetResolution() // 해상도...
{
	if (QuadBySection == EQuadBySection::None)
		return;

	if (SectionByComponent == ESectionByComponent::None)
		return;

	int32 multiples = 1;
	if (SectionByComponent == ESectionByComponent::Section_2x2)
	{
		multiples = 2;
	}

	switch (QuadBySection)
	{
	case EQuadBySection::Quad_7x7: multiples *= 7; break; //8bit
	case EQuadBySection::Quad_15x15: multiples *= 15; break; //16bit
	case EQuadBySection::Quad_31x31: multiples *= 31; break; //32bit
	case EQuadBySection::Quad_63x63: multiples *= 63; break; //64bit
	default: return;
	}

	Resolution.X = (Components.X * multiples) + 1;
	Editor_Width->SetText(FText::AsNumber((int32)Resolution.X));

	Resolution.Y = (Components.Y * multiples) + 1;
	Editor_Height->SetText(FText::AsNumber((int32)Resolution.Y));

	if (IsValid(TestRenderTarget))
	{
		TestRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
		TestRenderTarget->InitAutoFormat(Resolution.X, Resolution.Y);
		TestRenderTarget->UpdateResourceImmediate(true);
	}
}

void UUIHUD_ProceduralMesh::SetTotalComponents()
{
	TotalComponents = (int32)Components.X * (int32)Components.Y;
}

void UUIHUD_ProceduralMesh::OnChanged_SectionSize(FString inSelectedItem, ESelectInfo::Type inSelectType)
{
	QuadBySection = EQuadBySection::None;
	const UEnum* ptrEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EQuadBySection"), true);
	if (ptrEnum)
	{
		FName typeName = FName(*inSelectedItem);
		QuadBySection = StaticCast<EQuadBySection>(ptrEnum->GetValueByName(typeName));

	}

	SetDetail();
}

void UUIHUD_ProceduralMesh::OnChanged_ComponentsSection(FString inSelectedItem, ESelectInfo::Type inSelectType)
{
	SectionByComponent = ESectionByComponent::None;
	const UEnum* ptrEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESectionByComponent"), true);
	if (ptrEnum)
	{
		FName typeName = FName(*inSelectedItem);
		SectionByComponent = StaticCast<ESectionByComponent>(ptrEnum->GetValueByName(typeName));
	}

	SetDetail();
}

void UUIHUD_ProceduralMesh::OnChanged_CompNumX(float inValue)
{
	Components.X = inValue;
	SetComponentsCount(Components);
	SetDetail();
}

void UUIHUD_ProceduralMesh::OnChanged_CompNumY(float inValue)
{
	Components.Y = inValue;
	SetComponentsCount(Components);
	SetDetail();
}

void UUIHUD_ProceduralMesh::OnClicked_SetHeightMap()
{
	if (QuadBySection == EQuadBySection::None)
		return;

	if (SectionByComponent == ESectionByComponent::None)
		return;

	// 여기서 텍스처 만들어보자.
	UCanvas* canvas;
	FVector2D canvasSize;
	FDrawToRenderTargetContext context;

	//FVector location = GetActorLocation();

	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, TestRenderTarget, canvas, canvasSize, context);
	FVector2D lineLocStart = FVector2D(0, 0);
	FVector2D lineLocEnd = FVector2D(256, 256);
	FLinearColor color(1.0f, 1.0f, 1.0f, 1.0f);
	
	//canvas->K2_DrawLine(lineLocStart, lineLocEnd, 2.0f, color);
	canvas->K2_DrawBox(FVector2D(0.0f, 0.0f), FVector2D(7.0f, 1.0f), 1.0f, color);
	
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, context);

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
}


void UUIHUD_ProceduralMesh::OnClicked_Plus()
{
	// 위젯에서만 확대 출소를 해주자
}


void UUIHUD_ProceduralMesh::OnClicked_Minus()
{
	// 위젯에서만 확대 출소를 해주자
}
