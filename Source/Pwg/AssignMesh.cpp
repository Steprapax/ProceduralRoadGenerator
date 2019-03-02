#include "Pwg.h"
#include "AssignMesh.h"
#include "polypartition.h"
#include "RuntimeMeshAsync.h"

int workersWorking = 0;
bool wantsToWork = false;
bool isWorking = false;
int currentlyWorkingArray = 0;

URuntimeMeshComponent * mesh;
TArray<URuntimeMeshComponent*> components;
TArray<UMaterialInterface*> materials;
TArray<TArray<FRoad>> roads;

AAssignMesh::AAssignMesh()
{
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("mesh"));
}

// Called when the game starts or when spawned
void AAssignMesh::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAssignMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (wantsToWork) {
		workersWorking++;
		wantsToWork = false;
		isWorking = true;
	}

	if (isWorking) {
		materialize(roads[currentlyWorkingArray], materials[currentlyWorkingArray], components[currentlyWorkingArray]);
		currentlyWorkingArray++;
		if (currentlyWorkingArray >= roads.Num()) {
			isWorking = false;
			workersWorking--;
		}
	}
}

bool AAssignMesh::assignMaterial(TArray<FRoad> pols) {
	roads.Empty();
	roads.Add(pols);

	components.Empty();
	components.Add(mesh);

	materials.Empty();
	materials.Add(roadMaterial);

	currentlyWorkingArray = 0;
	wantsToWork = true;

	return true;
}

bool AAssignMesh::materialize(TArray<FRoad> &roads, UMaterialInterface *material, URuntimeMeshComponent* mesh) {

	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector2D> UV;
	TArray<FVector> normals;

	TArray<FColor> vertexColors;
	TArray<FRuntimeMeshTangent> tangents;

	int current = 0;
	for (FRoad &road : roads) {
		// local coordinates are found by getting the coordinates of points on the plane which they span up
		FVector e1 = road.verteces[1] - road.verteces[0];
		e1.Normalize();
		FVector n =  FVector::CrossProduct(e1, road.verteces[road.verteces.Num() - 1] - road.verteces[0]);
		FVector e2 = FVector::CrossProduct(e1, n);
		e2.Normalize();

		FVector origin = road.verteces[0];
		std::list<TPPLPoly> inTriangles;

		TPPLPoly poly;
		poly.Init(road.verteces.Num());
		for (int i = 0; i < road.verteces.Num(); i++) {
			FVector verteces = road.verteces[i];
			float y = FVector::DotProduct(e1, verteces - origin);
			float x = FVector::DotProduct(e2, verteces - origin);
			UV.Add(FVector2D(x, y));
			TPPLPoint newP{ x, y, current + i };
			poly[i] = newP;
			vertices.Add(verteces);
			normals.Add(-n);
		}

		TPPLPartition part;
		poly.SetOrientation(TPPL_CCW);
		int res = part.Triangulate_EC(&poly, &inTriangles);

		for (auto i : inTriangles) {
			triangles.Add(i[0].id);
			triangles.Add(i[1].id);
			triangles.Add(i[2].id);
		}
		current += road.verteces.Num();
	}

	mesh->SetMaterial(0, material);
	mesh->CreateMeshSection(0, vertices, triangles, normals, UV, vertexColors, tangents, true, EUpdateFrequency::Infrequent);

	return true;
}