#include "GameFramework/Actor.h"
#include "Functions.h"
#include "Generation.generated.h"

UCLASS()
class PWG_API AGeneration : public AActor
{
	GENERATED_BODY()
	//numero massimo di segmenti di strada che possono essere visualizzati
	UPROPERTY(config=Game, EditAnywhere, BlueprintReadWrite, Category = road, meta = (AllowPrivateAccess = "true"))
		int length;
	//funziona come indicazione per la rando-mizzazione dell’algoritmo: al variare di esso la mappa sarà sempre diversa
	UPROPERTY(config = Game, EditAnywhere, BlueprintReadWrite, Category = road, meta = (AllowPrivateAccess = "true"))
		int newSeed;
	//pone un limite sulla generazione consecutiva delle strade principali
	UPROPERTY(config = Game, EditAnywhere, BlueprintReadWrite, Category = road, meta = (AllowPrivateAccess = "true"))
		int maxMainPath;
	//pone un limite sulla generazione consecutiva delle strade secondarie
	UPROPERTY(config = Game, EditAnywhere, BlueprintReadWrite, Category = road, meta = (AllowPrivateAccess = "true"))
		int maxSecondaryPath;
	//permette di indicare un range massimo(uguale al minimo) di angolo di rotazione tra le strade 
	UPROPERTY(config = Game, EditAnywhere, BlueprintReadWrite, Category = road, meta = (AllowPrivateAccess = "true"))
		float degrees;

	UPROPERTY(config = Game, EditAnywhere, BlueprintReadWrite, Category = road, meta = (AllowPrivateAccess = "true"))
		int testLenghtControl;
	UPROPERTY(config = Game, EditAnywhere, BlueprintReadWrite, Category = road, meta = (AllowPrivateAccess = "true"))
		int testType;

public:	
	//usata nel Blueprint
	UFUNCTION(BlueprintCallable, Category = "Generation")
	TArray<FRoad> roadGeneration();

	void addRoad(std::priority_queue<FRoad*, std::deque<FRoad*>, priorityComparator> &queue, FRoad* currentRoad);
	void addForward(std::priority_queue<FRoad*, std::deque<FRoad*>, priorityComparator> &queue, RoadType type, FRoad* previousRoad, float width);
	void addSide(std::priority_queue<FRoad*, std::deque<FRoad*>, priorityComparator> &queue, RoadType type, FRoad* previous, float width, bool left);
	bool distanceCheck(TArray<FRoad*> &segments, FRoad* currentRoad);
	void vertices(FRoad* road);
	FRotator randomRotation(FRotator originalRotation, float degrees);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};