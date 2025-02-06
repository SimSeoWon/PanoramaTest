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

UENUM(BlueprintType)
enum class EQuadBySection : uint8
{
	None,
	Quad_7x7,
	Quad_15x15,
	Quad_31x31,
	Quad_63x63,
	Max,
};

UENUM(BlueprintType)
enum class ESectionByComponent : uint8
{
	None,
	Section_1x1,
	Section_2x2,
	Max,
};

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

	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* TestRenderTarget = nullptr;

#pragma region HeightMap
public:
	UFUNCTION()
	void OnClicked_SetHeightMap();

	UFUNCTION()
	void OnChanged_SectionSize(FString inSelectedItem, ESelectInfo::Type inSelectType);

	UFUNCTION()
	void OnChanged_ComponentsSection(FString inSelectedItem, ESelectInfo::Type inSelectType);

	UFUNCTION()
	void OnChanged_CompNumX(float inValue);

	UFUNCTION()
	void OnChanged_CompNumY(float inValue);

protected:
	void SetDetail();
	FVector GetScale();
	FVector2D GetComponentsCount();

	void SetComponentsCount(FVector2D inComponentsCount);
	void SetResolution();
	void SetTotalComponents();

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SetHeightMap = nullptr;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_Render = nullptr;

	/**������*/
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_ScaleX = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_ScaleY = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_ScaleZ = nullptr;

	/**���� ũ��*/
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ComboBox_SectionSize = nullptr;

	/**������Ʈ�� ����*/
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ComboBox_ComponentsSection = nullptr;

	/**������Ʈ �� (1 ~ 256)*/
	UPROPERTY(meta = (BindWidget))
	USlider* Slider_CompNumX = nullptr;

	UPROPERTY(meta = (BindWidget))
	USlider* Slider_CompNumY = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_CompNumX = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_CompNumY = nullptr;

	/**��ü �ػ�  ([���� �� * ������Ʈ�� ����) + 1] ~ [8191] */
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_Width = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_Height = nullptr;

	/**�� ������Ʈ  */
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_Total = nullptr;

	UPROPERTY(EditAnywhere)
	FVector Scale;	// �ȼ� ������ �������� ���.

	UPROPERTY(EditAnywhere)
	EQuadBySection  QuadBySection = EQuadBySection::None; // ���Ǵ� ���� 7(8Bit), 15(16Bit), 31(32Bit), 63(64Bit)

	UPROPERTY(EditAnywhere)
	ESectionByComponent SectionByComponent = ESectionByComponent::None; //������Ʈ�� ������ ����. 1x1, 2x2

	UPROPERTY(EditAnywhere)
	FVector2D Components; // ������Ʈ ����. ������Ʈ�� ������ ����ְ�, ������ ���带 ������.

	UPROPERTY(EditAnywhere)
	FVector2D Resolution;  // ��ü �ػ�.

	UPROPERTY(EditAnywhere)
	int32 TotalComponents = 1; //���� ������Ʈ ����.
#pragma endregion

};
