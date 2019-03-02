#include "ProceduralMeshComponent.h"
#include "Functions.h"
#include "RuntimeMeshComponent.h"
#include "AssignMesh.generated.h"
#pragma once
UCLASS()
class PWG_API AAssignMesh : public AActor
{
	GENERATED_BODY()
	
public:

	AAssignMesh();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* roadMaterial;

	UFUNCTION(BlueprintCallable, Category = "Generation")
	bool assignMaterial(TArray<FRoad> roads);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool materialize(TArray<FRoad> &roads, UMaterialInterface *material, URuntimeMeshComponent* mesh);
};