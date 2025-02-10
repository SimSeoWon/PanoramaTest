// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MyUserWidgetBase.h"
#include "UIWidget_HeightMap.generated.h"
class UUIHexagonTileView;
class UEditableTextBox;
class UComboBoxString;
class UButton;
class UImage;
class USlider;
class UTextBlock;
class UTextureRenderTarget2D;
class UCanvasRenderTarget2D;
class UUIHexagonTileDataBase;

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
class PANORAMATEST_API UUIWidget_HeightMap : public UMyUserWidgetBase
{
	GENERATED_BODY()

public:
	void SetData(UUIHexagonTileView* inTileView);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	float GroundCheck(FVector2D inPos);
	
public:
	UFUNCTION()
	void OnClicked_SetHeightMap();

	UFUNCTION()
	void OnClicked_SaveHeightMap();

	UFUNCTION()
	void OnChanged_SectionSize(FString inSelectedItem, ESelectInfo::Type inSelectType);

	UFUNCTION()
	void OnChanged_ComponentsSection(FString inSelectedItem, ESelectInfo::Type inSelectType);

	UFUNCTION()
	void OnChanged_CompNumX(float inValue);

	UFUNCTION()
	void OnChanged_CompNumY(float inValue);

protected:
	void DrawHeightMap(UCanvasRenderTarget2D* inRenderTarget);
	void SetDetail();
	FVector GetScale();
	FVector2D GetComponentsCount();

	void SetScale(FVector inScale);
	void SetComponentsCount(FVector2D inComponentsCount);
	void SetResolution();
	void SetTotalComponents();

protected:
	//UPROPERTY(EditAnywhere)
	//UTextureRenderTarget2D* TestRenderTarget = nullptr;

	UCanvasRenderTarget2D* CanvasRenderTarget = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SetHeightMap = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SaveHeightMap = nullptr;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_Render = nullptr;

	/**스케일*/
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_ScaleX = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_ScaleY = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_ScaleZ = nullptr;

	/**섹션 크기*/
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ComboBox_SectionSize = nullptr;

	/**컴포넌트별 섹션*/
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ComboBox_ComponentsSection = nullptr;

	/**컴포넌트 수 (1 ~ 256)*/
	UPROPERTY(meta = (BindWidget))
	USlider* Slider_CompNumX = nullptr;

	UPROPERTY(meta = (BindWidget))
	USlider* Slider_CompNumY = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_CompNumX = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_CompNumY = nullptr;

	/**전체 해상도  ([섹션 수 * 컴포넌트별 섹션) + 1] ~ [8191] */
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_Width = nullptr;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_Height = nullptr;

	/**총 컴포넌트  */
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Editor_Total = nullptr;

	UPROPERTY(EditAnywhere)
	FVector Scale;	// 픽셀 정보에 곱해지는 배수.

	UPROPERTY(EditAnywhere)
	EQuadBySection  QuadBySection = EQuadBySection::None; // 섹션당 쿼드 7(8Bit), 15(16Bit), 31(32Bit), 63(64Bit)

	UPROPERTY(EditAnywhere)
	ESectionByComponent SectionByComponent = ESectionByComponent::None; //컴포넌트별 섹션의 숫자. 1x1, 2x2

	UPROPERTY(EditAnywhere)
	FVector2D Components; // 컴포넌트 숫자. 컴포넌트는 섹션을 들고있고, 섹션은 쿼드를 구성함.

	UPROPERTY(EditAnywhere)
	FVector2D Resolution;  // 전체 해상도.

	UPROPERTY(EditAnywhere)
	int32 TotalComponents = 1; //최종 컴포넌트 숫자.

	UPROPERTY(EditAnywhere)
	TArray<UUIHexagonTileDataBase*> EntryDataList;

	UPROPERTY(EditAnywhere)
	float Tile_QuarterHeight = 0.0f;

};
