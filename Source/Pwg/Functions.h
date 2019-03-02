#include <queue>
#include "Pwg.h"
#include "Functions.generated.h"
#pragma once

//differenzio strade principali e secondarie
enum class RoadType : uint8{
	main,
	secondary
};

//struttura contenitore degli attributi di una strada
USTRUCT(BlueprintType)
	struct FRoad {
	GENERATED_USTRUCT_BODY();
	//vector1 e vector2 sono le coordinate delll'inizio e della fine del segmento di strada
	FVector vector1;
	FVector vector2;
	//vertex1/2/3/4 sono i vertici del rettangolo
	FVector vertex1;
	FVector vertex2;
	FVector vertex3;
	FVector vertex4;
	TArray<FVector> verteces;
	//directionVersor identifica il versore di direzione della strada
	/*rotation specifica l'angolo di rotazione del segmento in questione(FRotator(Pitch,Yaw,Roll))
	FRotator(Looking up and down,Running in circles 0=East, +North, -South, Tilting your head, 0=Straight, +Clockwise, -CCW)
	e viene comunque usato per ritornare il vettore ruotato di yaw gradi*/
	FVector initialDirectionVersor;
	FVector finalDirectionVersor;
	FRotator rotation;
	//previous punta al segmento di strada precedente
	//type specifica se si tratta di strada principale o secondaria
	FRoad* previous;
	RoadType type;
	/*width è una misura della larghezza della strada
	lengthControl tiene un riferimento del numero consecutivo di strade dello stesso tipo
	forward è true se la strada successiva viene aggiunta davanti a quella corrente
	priority esprimerà la posizione nella priorityqueue*/
	float width;
	int lengthControl;
	bool forward;
	float priority;
};

//metodo di ordinamento nella priorityqueue (pop priorità decrescente)
struct priorityComparator {
	bool operator() (FRoad* road1, FRoad* road2) {
		return road1->priority > road2->priority;
	}
};