// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProceduralMeshComponent.h"
#include "MeshDescription.h"
#include "ProceduralMeshPlayground.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class UArrowComponent;
class UUIHexagonTileView;
class FHexagonData;
class UUIHexagonTileDataBase;
struct FRES_HEXAGONTILEMAP;

UCLASS(ClassGroup = (Rendering, Common), meta = (BlueprintSpawnableComponent))
class AProceduralMeshPlayground : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralMeshPlayground(const FObjectInitializer& ObjectInitializer);

	void BuildMeshHexagon(float inScale, int32 inColumn, int32 inCount, UUIHexagonTileView* inTileView);

	void AddHexagon(int32 index);
	void AddTriangle(int32 index);
	void JoinNearestTile(int32 inIndex, int32 inNeighborsIndex);
	void ConvertToStaticMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TMap<UMaterialInterface*, FPolygonGroupID> BuildMaterialMap(UProceduralMeshComponent* ProcMeshComp, FMeshDescription& MeshDescription);
	FMeshDescription BuildMeshDescription(UProceduralMeshComponent* ProcMeshComp);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "ProceduralMeshPlayground")
	void CreateMesh();

	void SetHexagonData(TSharedPtr<FRES_HEXAGONTILEMAP> inTableData);

	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProceduralMeshPlayground")
	UProceduralMeshComponent* ProceduralMesh;


	int32 Column = 0;
	float Scale = 0.0f;
	float Width = 0.0f;
	float Height = 0.0f;
	float HalfWidth = 0.0f;
	float HalfHeight = 0.0f;
	float QuarterHeight = 0.0f;

	TArray<TSharedPtr<FHexagonData>> TileList;

	TArray<FVector> ArrVertices;
	TArray<FVector> ArrNormals;
	TArray<int32> ArrTriangles;
	TArray<FVector2D> ArrUVs;
	
};


class FHexagonData
{
public:
	FHexagonData();
	void SetData(UUIHexagonTileDataBase* inTileData);
	void SetArrUV();

public:
	int32 Index = 0; //�ε���

	FVector2D Coordinates; //  �ڽ��� ��ǥ

	TArray<FVector2D> Neighbors; // �̿��� Ÿ���� ��ǥ.

	TArray<FVector> ArrVertices; // 3���� ���� ��ǥ ����.

	TArray<FVector2D> ArrUV; // ������ UV ����.

	TArray<int32> ArrVerticeID;

	float Height = 0.0f;

};
