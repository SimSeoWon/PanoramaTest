// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralMeshPlayground.h"
#include "Engine/EngineTypes.h"
#include "EditorDirectories.h"
#include "AssetToolsModule.h"
#include "Dialogs/DlgPickAssetPath.h"
#include "PhysicsEngine/BodySetup.h"
#include "MaterialDomain.h"
#include "Materials/Material.h"
#include "StaticMeshAttributes.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "../UI/Hexagon/UIHexagonTileView.h"
#include "../UI/Hexagon/UIHexagonTileDataBase.h"
#include "../Tables/tabledata/tabledata_resource.h"

// Sets default values
AProceduralMeshPlayground::AProceduralMeshPlayground(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
}

void AProceduralMeshPlayground::BuildMeshHexagon(float inScale, int32 inColumn, int32 inCount, UUIHexagonTileView* inTileView)
{
	Scale = inScale;
	Column = inColumn;
	Width = FMath::Sqrt(3.0f) * Scale;
	Height = 2 * Scale;
	HalfWidth = Width / 2;
	HalfHeight = Scale;
	QuarterHeight = HalfHeight / 2;
	ArrVertices.Reset();
	ArrTriangles.Reset();
	ArrNormals.Reset();// Calculate normals and UVs
	ArrUVs.Reset();


	if (false == IsValid(inTileView))
		return;


	TArray<UUIHexagonTileDataBase*>& tileItems = inTileView->GetTileListItems();

	TileList.Reset();
	TileList.Reserve(tileItems.Num());


	for (int32 i = 0; i < tileItems.Num(); i++)
	{
		auto iter = tileItems[i];
		if (false == IsValid(iter))
			continue;

		TSharedPtr<FHexagonData> tile = MakeShared<FHexagonData>();
		tile->SetData(iter);
		TileList.Add(tile);
	}

	ArrVertices.Reserve(inCount * 7);
	for (int32 i = 0; i < inCount; i++)
	{
		AddHexagon(i);
	}

	ArrNormals.Reserve(ArrVertices.Num());
	for (int32 i = 0; i < ArrVertices.Num(); ++i)
	{
		ArrNormals.Emplace(FVector(0, 0, 1));
	}

	ArrUVs.Reserve(ArrVertices.Num());
	for (int32 i = 0; i < ArrVertices.Num(); ++i)
	{
		ArrUVs.Emplace(FVector2D(0, 1));
	}

	// Assign the geometry data to the mesh component
	if (ProceduralMesh)
	{
		ProceduralMesh->ClearAllMeshSections();
		ProceduralMesh->CreateMeshSection_LinearColor(0, ArrVertices, ArrTriangles, ArrNormals, ArrUVs, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
	}
}

void  AProceduralMeshPlayground::AddHexagon(int32 index)
{
	if (false == TileList.IsValidIndex(index))
		return;

	TSharedPtr<FHexagonData> tileInfo = TileList[index];
	if (false == tileInfo.IsValid())
		return;

	

	float testHeight = ((FMath::Rand32() + index) % 3) * 10;

	for (int32 i = 0; i < 7; i++)
	{
		if (false == tileInfo->ArrVertices.IsValidIndex(i))
			continue;

		ArrVertices.Emplace(FVector(tileInfo->ArrVertices[i].X, tileInfo->ArrVertices[i].Y, tileInfo->Height));
	}

	AddTriangle(index);
}

void AProceduralMeshPlayground::AddTriangle(int32 index)
{
	if (false == TileList.IsValidIndex(index))
		return;

	TSharedPtr<FHexagonData> tileInfo = TileList[index];
	if (false == tileInfo.IsValid())
		return;


	int32 curCount = ArrVertices.Num();
	ArrTriangles.Reserve(curCount + 18);


	//좌측 상단 삼각형
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[0]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[1]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[2]);
	// 이전 타일의 우측 하단 삼각형과 조인트

	//좌측 삼각형
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[0]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[2]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[3]);
	// 이전 타일의 우측 삼각형과 조인트

	//좌측 하단 삼각형
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[0]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[3]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[4]);


	// 우측 하단 삼각형
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[0]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[4]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[5]);

	// 우측 삼각형
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[0]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[5]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[6]);

	// 우측 상단 삼각형
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[0]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[6]);
	ArrTriangles.Emplace(tileInfo->ArrVerticeID[1]);
	//이전 타일의 좌측 하산 삼각형과 조인트

	for (int32 i = 0; i < tileInfo->Neighbors.Num(); i++)
	{
		JoinNearestTile(index, i);
	}
}

void AProceduralMeshPlayground::JoinNearestTile(int32 inIndex, int32 inNeighborsIndex)
{

	int32 neighborsIndexTest = inNeighborsIndex;

	TSharedPtr<FHexagonData> tileInfo = TileList[inIndex];
	if (false == tileInfo.IsValid())
		return;

	if (false == tileInfo->Neighbors.IsValidIndex(inNeighborsIndex))
		return;

	FVector2D neighbors = tileInfo->Neighbors[inNeighborsIndex];
	if (neighbors.X >= Column)
		return;

	if (neighbors.X < 0 || neighbors.Y < 0)
		return;

	int32 selectIndex = neighbors.Y * Column + neighbors.X;
	if (TileList.IsValidIndex(selectIndex) == false)
		return;

	TSharedPtr<FHexagonData> neighborsTile = TileList[selectIndex];
	if (false == neighborsTile.IsValid())
		return;

	UE_LOG(LogCore, Warning, TEXT("AProceduralMeshPlayground::JoinNearestTile() inIndex : %d - inNeighborsIndex : %d"), inIndex, inNeighborsIndex);
	switch (inNeighborsIndex)
	{
	case 0:
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[1]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[5]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[4]);

		ArrTriangles.Emplace(tileInfo->ArrVerticeID[1]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[4]);
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[2]);
		break;
		
	case 1:
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[6]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[5]);
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[2]);

		ArrTriangles.Emplace(tileInfo->ArrVerticeID[2]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[5]);
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[3]);
		break;

	case 2:
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[1]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[6]);
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[3]);

		ArrTriangles.Emplace(tileInfo->ArrVerticeID[3]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[6]);
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[4]);
		break;

	case 3:
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[1]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[2]);
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[4]);

		ArrTriangles.Emplace(tileInfo->ArrVerticeID[5]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[1]);
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[4]);
		break;

	case 4:
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[2]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[3]);
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[5]);

		ArrTriangles.Emplace(tileInfo->ArrVerticeID[6]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[2]);
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[5]);
		break;

	case 5:
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[3]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[4]);
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[6]);

		ArrTriangles.Emplace(tileInfo->ArrVerticeID[1]);
		ArrTriangles.Emplace(neighborsTile->ArrVerticeID[3]);
		ArrTriangles.Emplace(tileInfo->ArrVerticeID[6]);
		break;

	default:
		break;
	}

	// 두 타일의 메시를 연결해주자
}

// Called when the game starts or when spawned
void AProceduralMeshPlayground::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AProceduralMeshPlayground::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 여기선 이미 90도 회전시킨 상태...
void AProceduralMeshPlayground::SetHexagonData(TSharedPtr<FRES_HEXAGONTILEMAP> inTableData)
{
	if (false == inTableData.IsValid())
		return;
}


void AProceduralMeshPlayground::CreateMesh()
{
	// Create a simple triangle mesh
	ArrVertices.Reset();
	ArrVertices.Add(FVector(0, 0, 0)); // Vertex 0
	ArrVertices.Add(FVector(0, 1000, 0)); // Vertex 1
	ArrVertices.Add(FVector(1000, 0, 0)); // Vertex 2

	ArrTriangles.Reset();
	ArrTriangles.Add(0);
	ArrTriangles.Add(1);
	ArrTriangles.Add(2);

	// Calculate normals and UVs
	ArrNormals.Reset();
	for (int32 i = 0; i < ArrVertices.Num(); ++i)
	{
		ArrNormals.Add(FVector(0, 0, 1));
	}

	ArrUVs.Reset();;
	ArrUVs.Add(FVector2D(0, 0));
	ArrUVs.Add(FVector2D(1, 0));
	ArrUVs.Add(FVector2D(0, 1));

	// Assign the geometry data to the mesh component
	if (ProceduralMesh)
	{
		ProceduralMesh->ClearAllMeshSections();
		ProceduralMesh->CreateMeshSection_LinearColor(0, ArrVertices, ArrTriangles, ArrNormals, ArrUVs, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
	}
}

FMeshDescription AProceduralMeshPlayground::BuildMeshDescription(UProceduralMeshComponent* ProcMeshComp)
{
	FMeshDescription MeshDescription;

	FStaticMeshAttributes AttributeGetter(MeshDescription);
	AttributeGetter.Register();

	TPolygonGroupAttributesRef<FName> PolygonGroupNames = AttributeGetter.GetPolygonGroupMaterialSlotNames();
	TVertexAttributesRef<FVector3f> VertexPositions = AttributeGetter.GetVertexPositions();
	TVertexInstanceAttributesRef<FVector3f> Tangents = AttributeGetter.GetVertexInstanceTangents();
	TVertexInstanceAttributesRef<float> BinormalSigns = AttributeGetter.GetVertexInstanceBinormalSigns();
	TVertexInstanceAttributesRef<FVector3f> Normals = AttributeGetter.GetVertexInstanceNormals();
	TVertexInstanceAttributesRef<FVector4f> Colors = AttributeGetter.GetVertexInstanceColors();
	TVertexInstanceAttributesRef<FVector2f> UVs = AttributeGetter.GetVertexInstanceUVs();

	// Materials to apply to new mesh
	const int32 NumSections = ProcMeshComp->GetNumSections();
	int32 VertexCount = 0;
	int32 VertexInstanceCount = 0;
	int32 PolygonCount = 0;

	TMap<UMaterialInterface*, FPolygonGroupID> UniqueMaterials = BuildMaterialMap(ProcMeshComp, MeshDescription);
	TArray<FPolygonGroupID> PolygonGroupForSection;
	PolygonGroupForSection.Reserve(NumSections);

	// Calculate the totals for each ProcMesh element type
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FProcMeshSection* ProcSection =
			ProcMeshComp->GetProcMeshSection(SectionIdx);
		VertexCount += ProcSection->ProcVertexBuffer.Num();
		VertexInstanceCount += ProcSection->ProcIndexBuffer.Num();
		PolygonCount += ProcSection->ProcIndexBuffer.Num() / 3;
	}
	MeshDescription.ReserveNewVertices(VertexCount);
	MeshDescription.ReserveNewVertexInstances(VertexInstanceCount);
	MeshDescription.ReserveNewPolygons(PolygonCount);
	MeshDescription.ReserveNewEdges(PolygonCount * 2);
	UVs.SetNumChannels(4);

	// Create the Polygon Groups
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FProcMeshSection* ProcSection =
			ProcMeshComp->GetProcMeshSection(SectionIdx);
		UMaterialInterface* Material = ProcMeshComp->GetMaterial(SectionIdx);
		if (Material == nullptr)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}

		FPolygonGroupID* PolygonGroupID = UniqueMaterials.Find(Material);
		check(PolygonGroupID != nullptr);
		PolygonGroupForSection.Add(*PolygonGroupID);
	}


	// Add Vertex and VertexInstance and polygon for each section
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FProcMeshSection* ProcSection =
			ProcMeshComp->GetProcMeshSection(SectionIdx);
		FPolygonGroupID PolygonGroupID = PolygonGroupForSection[SectionIdx];
		// Create the vertex
		int32 NumVertex = ProcSection->ProcVertexBuffer.Num();
		TMap<int32, FVertexID> VertexIndexToVertexID;
		VertexIndexToVertexID.Reserve(NumVertex);
		for (int32 VertexIndex = 0; VertexIndex < NumVertex; ++VertexIndex)
		{
			FProcMeshVertex& Vert = ProcSection->ProcVertexBuffer[VertexIndex];
			const FVertexID VertexID = MeshDescription.CreateVertex();
			VertexPositions[VertexID] = (FVector3f)Vert.Position;
			VertexIndexToVertexID.Add(VertexIndex, VertexID);
		}
		// Create the VertexInstance
		int32 NumIndices = ProcSection->ProcIndexBuffer.Num();
		int32 NumTri = NumIndices / 3;
		TMap<int32, FVertexInstanceID> IndiceIndexToVertexInstanceID;
		IndiceIndexToVertexInstanceID.Reserve(NumVertex);
		for (int32 IndiceIndex = 0; IndiceIndex < NumIndices; IndiceIndex++)
		{
			const int32 VertexIndex = ProcSection->ProcIndexBuffer[IndiceIndex];
			const FVertexID VertexID = VertexIndexToVertexID[VertexIndex];
			const FVertexInstanceID VertexInstanceID =
				MeshDescription.CreateVertexInstance(VertexID);
			IndiceIndexToVertexInstanceID.Add(IndiceIndex, VertexInstanceID);

			FProcMeshVertex& ProcVertex = ProcSection->ProcVertexBuffer[VertexIndex];

			Tangents[VertexInstanceID] = (FVector3f)ProcVertex.Tangent.TangentX;
			Normals[VertexInstanceID] = (FVector3f)ProcVertex.Normal;
			BinormalSigns[VertexInstanceID] =
				ProcVertex.Tangent.bFlipTangentY ? -1.f : 1.f;

			Colors[VertexInstanceID] = FLinearColor(ProcVertex.Color);

			UVs.Set(VertexInstanceID, 0, FVector2f(ProcVertex.UV0));	// LWC_TODO: Precision loss
			UVs.Set(VertexInstanceID, 1, FVector2f(ProcVertex.UV1));	// LWC_TODO: Precision loss
			UVs.Set(VertexInstanceID, 2, FVector2f(ProcVertex.UV2));	// LWC_TODO: Precision loss
			UVs.Set(VertexInstanceID, 3, FVector2f(ProcVertex.UV3));	// LWC_TODO: Precision loss
		}

		// Create the polygons for this section
		for (int32 TriIdx = 0; TriIdx < NumTri; TriIdx++)
		{
			FVertexID VertexIndexes[3];
			TArray<FVertexInstanceID> VertexInstanceIDs;
			VertexInstanceIDs.SetNum(3);

			for (int32 CornerIndex = 0; CornerIndex < 3; ++CornerIndex)
			{
				const int32 IndiceIndex = (TriIdx * 3) + CornerIndex;
				const int32 VertexIndex = ProcSection->ProcIndexBuffer[IndiceIndex];
				VertexIndexes[CornerIndex] = VertexIndexToVertexID[VertexIndex];
				VertexInstanceIDs[CornerIndex] =
					IndiceIndexToVertexInstanceID[IndiceIndex];
			}

			// Insert a polygon into the mesh
			MeshDescription.CreatePolygon(PolygonGroupID, VertexInstanceIDs);
		}
	}
	return MeshDescription;
}

TMap<UMaterialInterface*, FPolygonGroupID> AProceduralMeshPlayground::BuildMaterialMap(UProceduralMeshComponent* ProcMeshComp, FMeshDescription& MeshDescription)
{
	TMap<UMaterialInterface*, FPolygonGroupID> UniqueMaterials;
	const int32 NumSections = ProcMeshComp->GetNumSections();
	UniqueMaterials.Reserve(NumSections);

	FStaticMeshAttributes AttributeGetter(MeshDescription);
	TPolygonGroupAttributesRef<FName> PolygonGroupNames = AttributeGetter.GetPolygonGroupMaterialSlotNames();
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FProcMeshSection* ProcSection =
			ProcMeshComp->GetProcMeshSection(SectionIdx);
		UMaterialInterface* Material = ProcMeshComp->GetMaterial(SectionIdx);
		if (Material == nullptr)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}

		if (!UniqueMaterials.Contains(Material))
		{
			FPolygonGroupID NewPolygonGroup = MeshDescription.CreatePolygonGroup();
			UniqueMaterials.Add(Material, NewPolygonGroup);
			PolygonGroupNames[NewPolygonGroup] = Material->GetFName();
		}
	}
	return UniqueMaterials;
}

void AProceduralMeshPlayground::ConvertToStaticMesh()
{
	if (false == IsValid(ProceduralMesh))
		return;

	FString NewNameSuggestion = FString(TEXT("ProcMesh"));
	FString DefaultPath;
	const FString DefaultDirectory = FEditorDirectories::Get().GetLastDirectory(ELastDirectory::NEW_ASSET);
	FPackageName::TryConvertFilenameToLongPackageName(DefaultDirectory, DefaultPath);

	if (DefaultPath.IsEmpty())
	{
		DefaultPath = TEXT("/Game/Meshes");
	}

	FString PackageName = DefaultPath / NewNameSuggestion;
	FString Name;
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(PackageName, TEXT(""), PackageName, Name);

	TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
		SNew(SDlgPickAssetPath)
		.Title(FText::FromString(TEXT("ConvertToStaticMeshPickName")))
		.DefaultAssetPath(FText::FromString(PackageName));

	if (PickAssetPathWidget->ShowModal() == EAppReturnType::Ok)
	{
		// Get the full name of where we want to create the physics asset.
		FString UserPackageName = PickAssetPathWidget->GetFullAssetPath().ToString();
		FName MeshName(*FPackageName::GetLongPackageAssetName(UserPackageName));

		// Check if the user inputed a valid asset name, if they did not, give it the generated default name
		if (MeshName == NAME_None)
		{
			// Use the defaults that were already generated.
			UserPackageName = PackageName;
			MeshName = *Name;
		}


		FMeshDescription MeshDescription = BuildMeshDescription(ProceduralMesh);

		// If we got some valid data.
		if (MeshDescription.Polygons().Num() > 0)
		{
			// Then find/create it.
			UPackage* Package = CreatePackage(*UserPackageName);
			check(Package);

			// Create StaticMesh object
			UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, MeshName, RF_Public | RF_Standalone);
			StaticMesh->InitResources();

			StaticMesh->SetLightingGuid();

			// Add source to new StaticMesh
			FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
			SrcModel.BuildSettings.bRecomputeNormals = false;
			SrcModel.BuildSettings.bRecomputeTangents = false;
			SrcModel.BuildSettings.bRemoveDegenerates = false;
			SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
			SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
			SrcModel.BuildSettings.bGenerateLightmapUVs = true;
			SrcModel.BuildSettings.SrcLightmapIndex = 0;
			SrcModel.BuildSettings.DstLightmapIndex = 1;
			StaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
			StaticMesh->CommitMeshDescription(0);

			//// SIMPLE COLLISION
			if (!ProceduralMesh->bUseComplexAsSimpleCollision)
			{
				StaticMesh->CreateBodySetup();
				UBodySetup* NewBodySetup = StaticMesh->GetBodySetup();
				NewBodySetup->BodySetupGuid = FGuid::NewGuid();
				NewBodySetup->AggGeom.ConvexElems = ProceduralMesh->ProcMeshBodySetup->AggGeom.ConvexElems;
				NewBodySetup->bGenerateMirroredCollision = false;
				NewBodySetup->bDoubleSidedGeometry = true;
				NewBodySetup->CollisionTraceFlag = CTF_UseDefault;
				NewBodySetup->CreatePhysicsMeshes();
			}

			//// MATERIALS
			TSet<UMaterialInterface*> UniqueMaterials;
			const int32 NumSections = ProceduralMesh->GetNumSections();
			for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
			{
				FProcMeshSection* ProcSection =
					ProceduralMesh->GetProcMeshSection(SectionIdx);
				UMaterialInterface* Material = ProceduralMesh->GetMaterial(SectionIdx);
				UniqueMaterials.Add(Material);
			}
			// Copy materials to new mesh
			for (auto* Material : UniqueMaterials)
			{
				StaticMesh->GetStaticMaterials().Add(FStaticMaterial(Material));
			}

			//Set the Imported version before calling the build
			StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

			// Build mesh from source
			StaticMesh->Build(false);
			StaticMesh->PostEditChange();

			// Notify asset registry of new asset
			FAssetRegistryModule::AssetCreated(StaticMesh);
		}
	}
}

FHexagonData::FHexagonData()
{
	Index = 0; //인덱스
}

void FHexagonData::SetData(UUIHexagonTileDataBase* inTileData)
{
	if (false == IsValid(inTileData))
		return;

	Height = (uint8)inTileData->HeightType;
	Height *= 100.0f;

	FRotator rotation = FRotator(0.0f, 90.0f, 0.0f); //수직축으로 90도 회전한다.
	Index = inTileData->Index;
	Coordinates = inTileData->Coordinates; // 자신의 좌표
	inTileData->GetAroundDoordinatesList(Neighbors); // 이웃된 타일 좌표

	ArrVertices.Reset();
	ArrVertices.Reserve(7);

	FVector vecCenter = FVector(inTileData->Center.X, inTileData->Center.Y, 0.0f);
	ArrVertices.Emplace(rotation.RotateVector(vecCenter));
	for (int32 i = 0; i < 6; i++)
	{
		FVector vecCorner = FVector(inTileData->CornerList[i].X, inTileData->CornerList[i].Y, 0.0f);
		ArrVertices.Emplace(rotation.RotateVector(vecCorner));
	}

	ArrVerticeID.Reset();
	ArrVerticeID.Reserve(7);
	for (int32 i = 0; i < 7; i++)
	{

		ArrVerticeID.Emplace((7 * Index) + i);
	}
}