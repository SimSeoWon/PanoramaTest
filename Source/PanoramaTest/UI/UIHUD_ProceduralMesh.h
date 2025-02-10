// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidgetBase.h"
#include "UIHUD_ProceduralMesh.generated.h"
class UUIHexagonTileView;
class UEditableTextBox;
class UComboBoxString;
class UButton;
class UImage;
class USlider;
class UTextBlock;
class UTextureRenderTarget2D;

class UUIWidget_HeightMap;

enum class EQuadBySection : uint8;
enum class ESectionByComponent : uint8;

/**
 *
 */
UCLASS()
class PANORAMATEST_API UUIHUD_ProceduralMesh : public UMyUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnSelected_HexagonTile(const FVector2D inPos = FVector2D::Zero());

	UFUNCTION()
	void OnClicked_CreateTile();

	UFUNCTION()
	void OnClicked_SetMesh();

	UFUNCTION()
	void OnClicked_Export();

	UFUNCTION()
	void OnClicked_LoadFile();

	UFUNCTION()
	void OnClicked_Plus();

	UFUNCTION()
	void OnClicked_Minus();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void CreateTile();

protected:
	UPROPERTY(meta = (BindWidget))
	UUIWidget_HeightMap* Widget_HeightMap = nullptr;

	UPROPERTY(meta = (BindWidget))
	UUIHexagonTileView* HexagonView = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_CreateTile = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_TileSize = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_OffsetX = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_OffsetY = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_OffsetZ = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_Columnt = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_TotalCount = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_FilePath = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Plus = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Minus = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SetMesh = nullptr;



	UPROPERTY(meta = (BindWidget))
	UButton* Btn_LoadFile = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Export = nullptr;

	FInputModeUIOnly InputModeUIOnly;

	UPROPERTY(EditAnywhere)
	FString FilePath;

	UPROPERTY(EditAnywhere)
	FString CsvFilePath;


};
