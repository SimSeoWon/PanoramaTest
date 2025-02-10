// Fill out your copyright notice in the Description page of Project Settings.


#include "UIWidget_HeightMap.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Engine/LevelScriptActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/KismetRenderingLibrary.h" // for ReleaseRenderTarget2D()
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorDirectories.h"
#include "AssetToolsModule.h"
#include "Dialogs/DlgPickAssetPath.h"

#include "../Hexagon/UIHexagonTileView.h"
#include "../Hexagon/UIHexagonTileDataBase.h"
#include "../../MyGameInstance.h"
#include "../../MyPlayerController.h"
#include "../../MyLevelScriptActor.h"
#include "../../Tables/tabledata/tabledata_enum.h"
#include "../../Tables/tabledata/tabledata_resource.h"

void UUIWidget_HeightMap::NativeConstruct()
{
	Super::NativeConstruct();
	Slider_CompNumX->OnValueChanged.AddDynamic(this, &UUIWidget_HeightMap::OnChanged_CompNumX);
	Slider_CompNumY->OnValueChanged.AddDynamic(this, &UUIWidget_HeightMap::OnChanged_CompNumY);
	Slider_CompNumX->SetValue(1);
	Slider_CompNumY->SetValue(1);

	CanvasRenderTarget = NewObject<UCanvasRenderTarget2D>();
	if (IsValid(CanvasRenderTarget))
	{
		//CanvasRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
		CanvasRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
		CanvasRenderTarget->InitAutoFormat(256, 256);
		CanvasRenderTarget->Filter = TF_Nearest;
		CanvasRenderTarget->AddressX = TextureAddress::TA_Clamp;
		CanvasRenderTarget->AddressY = TextureAddress::TA_Clamp;
		CanvasRenderTarget->UpdateResourceImmediate(true);

		Img_Render->Brush.SetResourceObject(CanvasRenderTarget);
	}

	Btn_SetHeightMap->OnClicked.AddDynamic(this, &UUIWidget_HeightMap::OnClicked_SetHeightMap);
	Btn_SaveHeightMap->OnClicked.AddDynamic(this, &UUIWidget_HeightMap::OnClicked_SaveHeightMap);

	if (IsValid(ComboBox_SectionSize))
	{
		ComboBox_SectionSize->OnSelectionChanged.AddDynamic(this, &UUIWidget_HeightMap::OnChanged_SectionSize);

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

			ComboBox_SectionSize->SetSelectedOption(TEXT("None"));
		}
	}

	if (IsValid(ComboBox_ComponentsSection))
	{
		ComboBox_ComponentsSection->OnSelectionChanged.AddDynamic(this, &UUIWidget_HeightMap::OnChanged_ComponentsSection);
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

			ComboBox_ComponentsSection->SetSelectedOption(TEXT("None"));
		}
	}
}

void UUIWidget_HeightMap::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUIWidget_HeightMap::SetData(UUIHexagonTileView* inTileView)
{
	EntryDataList.Reset();

	if (false == IsValid(inTileView))
		return;

	TArray<UUIHexagonTileDataBase*>& tileItems = inTileView->GetTileListItems();
	EntryDataList.Reserve(tileItems.Num());
	for (auto iter : tileItems)
	{
		EntryDataList.Add(iter);
	}

	if (EntryDataList.IsEmpty())
		return;

	float halfHeight = EntryDataList[0]->Size;
	Tile_QuarterHeight = halfHeight / 2;

	SetScale(FVector(Tile_QuarterHeight, Tile_QuarterHeight, Tile_QuarterHeight));
	SetDetail();
}

void UUIWidget_HeightMap::SetDetail()
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

FVector UUIWidget_HeightMap::GetScale()
{
	FVector scale = FVector::Zero();
	scale.X = FCString::Atof(*Editor_ScaleX->GetText().ToString());
	scale.Y = FCString::Atof(*Editor_ScaleY->GetText().ToString());
	scale.Z = FCString::Atof(*Editor_ScaleZ->GetText().ToString());
	return scale;
}

void UUIWidget_HeightMap::SetScale(FVector inScale) 
{
	Scale = inScale;

	Editor_ScaleX->SetText(FText::AsNumber(Scale.X));
	Editor_ScaleY->SetText(FText::AsNumber(Scale.Y));
	Editor_ScaleZ->SetText(FText::AsNumber(Scale.Z));
}

void UUIWidget_HeightMap::SetComponentsCount(FVector2D inComponentsCount)
{
	Components = inComponentsCount;
	Txt_CompNumX->SetText(FText::AsNumber((int32)Components.X));
	Txt_CompNumY->SetText(FText::AsNumber((int32)Components.Y));
}

FVector2D UUIWidget_HeightMap::GetComponentsCount()
{
	FVector2D componentsCount = FVector2D::Zero();
	componentsCount.X = FCString::Atof(*Txt_CompNumX->GetText().ToString());
	componentsCount.Y = FCString::Atof(*Txt_CompNumY->GetText().ToString());
	return componentsCount;
}

void UUIWidget_HeightMap::SetResolution() // 해상도...
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

	if (IsValid(CanvasRenderTarget))
	{
		//CanvasRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
		CanvasRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
		CanvasRenderTarget->InitAutoFormat(Resolution.X, Resolution.Y);
		CanvasRenderTarget->Filter = TF_Nearest;
		CanvasRenderTarget->AddressX = TextureAddress::TA_Clamp;
		CanvasRenderTarget->AddressY = TextureAddress::TA_Clamp;
		CanvasRenderTarget->UpdateResourceImmediate(true);
	}
}

void UUIWidget_HeightMap::SetTotalComponents()
{
	TotalComponents = (int32)Components.X * (int32)Components.Y;
}

void UUIWidget_HeightMap::OnChanged_SectionSize(FString inSelectedItem, ESelectInfo::Type inSelectType)
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

void UUIWidget_HeightMap::OnChanged_ComponentsSection(FString inSelectedItem, ESelectInfo::Type inSelectType)
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

void UUIWidget_HeightMap::OnChanged_CompNumX(float inValue)
{
	Components.X = inValue;
	SetComponentsCount(Components);
	SetDetail();
}

void UUIWidget_HeightMap::OnChanged_CompNumY(float inValue)
{
	Components.Y = inValue;
	SetComponentsCount(Components);
	SetDetail();
}

void UUIWidget_HeightMap::OnClicked_SaveHeightMap()
{
	if (false == IsValid(CanvasRenderTarget))
		return;

	FString NewNameSuggestion = FString(TEXT("ProcRenderTarget"));
	FString DefaultPath;
	const FString DefaultDirectory = FEditorDirectories::Get().GetLastDirectory(ELastDirectory::NEW_ASSET);
	FPackageName::TryConvertFilenameToLongPackageName(DefaultDirectory, DefaultPath);
	if (DefaultPath.IsEmpty())
	{
		DefaultPath = TEXT("/Game");
	}
	// 디렉토리 선택해서 랜더타겟 저장하기.

	FString PackageName = DefaultPath / NewNameSuggestion;
	FString Name;
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(PackageName, TEXT(""), PackageName, Name);

	TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
		SNew(SDlgPickAssetPath)
		.Title(FText::FromString(TEXT("ProcRanderTargetName")))
		.DefaultAssetPath(FText::FromString(PackageName));

	if (PickAssetPathWidget->ShowModal() == EAppReturnType::Ok)
	{
		FString UserPackageName = PickAssetPathWidget->GetFullAssetPath().ToString();
		FName assetName(*FPackageName::GetLongPackageAssetName(UserPackageName));

		// Check if the user inputed a valid asset name, if they did not, give it the generated default name
		if (assetName == NAME_None)
		{
			// Use the defaults that were already generated.
			UserPackageName = PackageName;
			assetName = *Name;
		}

		UPackage* Package = CreatePackage(*UserPackageName);
		if (false == IsValid(Package))
			return;

		UCanvasRenderTarget2D* renderTarget = NewObject<UCanvasRenderTarget2D>(Package, assetName, RF_Public | RF_Standalone);
		if (false == IsValid(renderTarget))
			return;

		//renderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
		renderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
		renderTarget->InitAutoFormat(Resolution.X, Resolution.Y);
		renderTarget->Filter = TF_Nearest;
		renderTarget->AddressX = TextureAddress::TA_Clamp;
		renderTarget->AddressY = TextureAddress::TA_Clamp;
		renderTarget->UpdateResourceImmediate(true);
		DrawHeightMap(renderTarget);
		FAssetRegistryModule::AssetCreated(renderTarget);
	}
}

void UUIWidget_HeightMap::OnClicked_SetHeightMap()
{
	if (QuadBySection == EQuadBySection::None)
		return;

	if (SectionByComponent == ESectionByComponent::None)
		return;

	DrawHeightMap(CanvasRenderTarget);	
}

void UUIWidget_HeightMap::DrawHeightMap(UCanvasRenderTarget2D* inRenderTarget) 
{
	if (QuadBySection == EQuadBySection::None)
		return;

	if (SectionByComponent == ESectionByComponent::None)
		return;

	//UE_LOG(LogCore, Warning, TEXT("UUIWidget_HeightMap::DrawHeightMap()"));
	if (false == IsValid(inRenderTarget))
		return;

	// 여기서 텍스처 만들어보자.
	UCanvas* canvas;
	FVector2D canvasSize;
	FDrawToRenderTargetContext context;
	FLinearColor clearColor(0, 0, 0, 0);

	UKismetRenderingLibrary::ClearRenderTarget2D(this, inRenderTarget, clearColor);
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, inRenderTarget, canvas, canvasSize, context);
	int32 total = (int32)Resolution.X * Resolution.Y;

	for (int32 i = 0; i < total; i++)
	{
		float x = i % (int32)Resolution.X;
		float y = i / (int32)Resolution.X;

		

		FVector2D vecStart = FVector2D(x, y);
		FVector2D vecEnd = FVector2D(x+1, y+1);
		FVector2D vecUVStart = FVector2D(0, 0);
		FVector2D vecUVEnd = FVector2D(1, 1);
		float height = GroundCheck(vecStart);
		FLinearColor color(height, height, height, height);

		//UE_LOG(LogCore, Warning, TEXT("---------------------------"));
		//UE_LOG(LogCore, Warning, TEXT("vecStart(%f, %f)"), vecStart.X, vecStart.Y);
		//UE_LOG(LogCore, Warning, TEXT("vecEnd(%f, %f)"), vecEnd.X, vecEnd.Y);
		//UE_LOG(LogCore, Warning, TEXT("color(%f, %f, %f, %f)"), color.R, color.G, color.B, color.A);
		//UE_LOG(LogCore, Warning, TEXT("---------------------------"));

		//canvas->K2_DrawTexture(nullptr, vecStart, vecEnd, vecUVStart, vecUVEnd, color, EBlendMode::BLEND_Translucent);
		canvas->K2_DrawTexture(nullptr, vecStart, vecEnd, vecUVStart, vecUVEnd, color, EBlendMode::BLEND_Opaque);
	}

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, context);
}

float UUIWidget_HeightMap::GroundCheck(FVector2D inPos) 
{
	//UE_LOG(LogCore, Warning, TEXT("UUIWidget_HeightMap::GroundCheck()"));


	// 배수를 곱해준다.
	inPos.X *= Scale.X;
	inPos.Y *= Scale.Y;

	EHexagonTileHeight heightType = EHexagonTileHeight::NONE;

	for (auto iter : EntryDataList)
	{
		if (false == IsValid(iter))
			continue;

		if(false == iter->CollisionCheck(inPos))
			continue;

		heightType = iter->HeightType;
	}

	//UE_LOG(LogCore, Warning, TEXT("---------------------------"));
	//UE_LOG(LogCore, Warning, TEXT("inPos(%f, %f)"), inPos.X, inPos.Y);
	//UE_LOG(LogCore, Warning, TEXT("heightType(%d)"), (uint8)heightType);
	//UE_LOG(LogCore, Warning, TEXT("---------------------------"));
	double result = ((uint8)heightType + 128) / 256.0;

	return result;
}