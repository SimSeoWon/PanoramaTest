// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "UI/MyUserWidgetBase.h"
#include "UI/UIHUD_ProceduralMesh.h"
#include "MyGameInstance.h"


void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	Widget = nullptr;
	
	FString strFileName = TEXT("bp_uiwidget_main");
	FString packagePath = FString::Printf(TEXT("/Game/UI/%s.%s_C"), *strFileName, *strFileName);
	FSoftClassPath softBPClassPath(packagePath);

	UClass* widgetClass = softBPClassPath.TryLoadClass<UUIHUD_ProceduralMesh>();
	if (false == IsValid(widgetClass))
		return;

	UMyGameInstance* gameInstance = UMyGameInstance::Get();
	if (false == IsValid(gameInstance))
		return;

	Widget = CreateWidget<UUIHUD_ProceduralMesh>(gameInstance, widgetClass);
	if (false == IsValid(Widget))
		return;

	Widget->AddToViewport();
	
}
