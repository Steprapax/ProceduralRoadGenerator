#include "Pwg.h"
#include "Noise.h"
#include "Generation.h"

/*assegna a road i vertici corretti del segmento: somma tra il vector1/2 e un vettore 
ruotato di 90° di lunghezza width (nota che width identificherà la larghezza del segmento. 
La componente z del vertex consente di assegnare un'altezza per la classe di assegnazione del materiale*/
void AGeneration::vertices(FRoad* road) {
	road->vertex1 = (road->vector1 + FRotator(0, 90, 0).RotateVector(road->initialDirectionVersor) * road->width) + FVector(0, 0, 100);
	road->vertex2 = (road->vector1 - FRotator(0, 90, 0).RotateVector(road->initialDirectionVersor) *road->width) + FVector(0, 0, 100);
	road->vertex3 = (road->vector2 + FRotator(0, 90, 0).RotateVector(road->finalDirectionVersor) * road->width) + FVector(0, 0, 100);
	road->vertex4 = (road->vector2 - FRotator(0, 90, 0).RotateVector(road->finalDirectionVersor) * road->width) + FVector(0, 0, 100);
}

/*formula matematica per identificare le intersezioni tra due segmenti tramite le coordinate dei punti 
nel piano cartesiano. Ritorna il punto della collisione se c'è effettivamente stata, sennò (0,0,0)
https://bar-ingegneria.forumfree.it/?t=63091608*/
FVector intersectionSegments(FVector p1, FVector p2, FVector p3, FVector p4) {
	float y1 = p1.Y;
	float y2 = p2.Y;
	float x1 = p1.X;
	float x2 = p2.X;
	float y3 = p3.Y;
	float y4 = p4.Y;
	float x3 = p3.X;
	float x4 = p4.X;

	float delta = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);

	float k1 = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / delta;
	float k2 = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / delta;

	if (k1 >= 0 && k1 <= 1 && k2 >= 0 && k2 <= 1)
		return FVector(x1 + (k1 * (x2 - x1)), y1 + (k1 * (y2 - y1)), 0);
	else
		return FVector(0.0f, 0.0f, 0.0f);
}

//La funzione che contiene il loop per la generazione
TArray<FRoad> AGeneration::roadGeneration() {

	FRandomStream seed = newSeed;

	//inizializzo l'stanza per utilizzare il rumore di Perlin. Mando in input due numeri casuali nel range(-200, 200) sulla base del seed 
	Noise::getInstance()->initialize(seed.FRandRange(-200, 200), seed.FRandRange(-200, 200)); //baseSeed.FRandRange()

	std::priority_queue<FRoad*, std::deque<FRoad*>, priorityComparator> queue;  //std::deque (double-ended queue) è un contenitore di sequenza indicizzato che consente l'inserimento e la cancellazione veloce sia l'inizio e la fine gli elementi di una deque non vengono memorizzati in modo contiguo
	TArray<FRoad*> generatedSegments;

	//Il primo segmento in assoluto della mappa viene inizializzato "a mano": tipo main, width= 2000 ecc...
	if (testLenghtControl == 0 && testType == 0) {
		FRoad* first = new FRoad();
		first->vector1 = FVector(0, 0, 0);
		first->vector2 = first->vector1 + FVector(20000, 0, 0);
		first->type = RoadType::main;
		first->initialDirectionVersor = first->vector2 - first->vector1;
		first->initialDirectionVersor.Normalize();
		first->finalDirectionVersor = first->initialDirectionVersor;
		first->width = 2000.0f;
		//identifico i vertici e push() nella queue
		vertices(first);
		queue.push(first);
	}
	///////////////test//////////////////
	else {
		FRoad* first = new FRoad();
		first->vector1 = FVector(0, 0, 0);
		first->vector2 = first->vector1 + FVector(20000, 0, 0);

		if (testType == 1) {
			first->type = RoadType::main;
			first->width = 2000.0f;
		}
		else if (testType == 2) {
			first->type = RoadType::secondary;
			first->width = 500.0f;
		}
		first->lengthControl = testLenghtControl;
		first->initialDirectionVersor = first->vector2 - first->vector1;
		first->initialDirectionVersor.Normalize();
		first->finalDirectionVersor = first->initialDirectionVersor;
		vertices(first);
		queue.push(first);
	}
	/////////////////test////////////////////

	//inizio il loop (while) finchè la coda non è vuota e length è maggiore dei segmenti
	while (queue.size() > 0 && generatedSegments.Num() < length) {
		FRoad* currentRoad = queue.top();
		queue.pop();
		if (distanceCheck(generatedSegments, currentRoad)) {
			generatedSegments.Add(currentRoad);
			if (currentRoad->previous && currentRoad->previous->vector2==currentRoad->vector1)
				currentRoad->previous->forward = true;
			addRoad(queue, currentRoad);
		}
	}

	//massima distanza per cui la fine di un segmento si attacca ad un'altra possibile strada di fronte
	float maxCollisionL = 22500.0f;
	/*le strade con l'attributo forward a true sono scartate poichè sono sicuramente collegate ad una 
	strada davanti*/
	for (int i = 0; i < generatedSegments.Num(); i++) {
		FRoad* g = generatedSegments[i];
		if (g->forward)
			continue;
		
		//tengo salvato il vecchio vector2
		FVector vector2Prev = g->vector2;
		g->vector2 += g->finalDirectionVersor*maxCollisionL;
		FRoad* closest = nullptr;
		FVector intersectionPoint;
		for (int j = 0; j < generatedSegments.Num(); j++) {
			FRoad* f = generatedSegments[j];
			//lo stesso segmento, salto al prossimo
			if (i == j) 
				continue;

			//se c'è l'intersezione salvo il segmento e il punto di intersezione
			FVector point = intersectionSegments(vector2Prev, g->vector2, f->vector1, f->vector2);
			if (point.X != 0.0f) {
				closest = f;
				intersectionPoint = point;
			}
		}
		//la nuava fine del segmento corrisponde al punto di collisione tra le due strade
		if (closest) {
			g->vector2 = intersectionPoint;
			vertices(g);
		}
		//altrimenti reimposto vector2
		else {
			g->vector2 = vector2Prev;
		}
	}

	TArray<FRoad> finishedSegments;
	for (FRoad* roads : generatedSegments) {
		finishedSegments.Add(*roads);
	}

	//ritorno l'array pronto per essere passato all'assegnamento del materiale 
	TArray<FRoad> roads;
	for (FRoad* r : generatedSegments) {
		FRoad road;
		//ciascun elemento dell'array rappresenta una strada e contiene un array di vertici di essa
		road.verteces.Add(r->vertex1);
		road.verteces.Add(r->vertex3);
		road.verteces.Add(r->vertex4);
		road.verteces.Add(r->vertex2);
		roads.Add(road);
	}
	return roads;
}

/*ritorno il rotator dato dalla somma del rotator della strada precedente e del nuovo rotator 
scelto in modo random all'interno  del range(-degrees, degrees) ma in base al seed dato
*/
FRotator AGeneration::randomRotation(FRotator originalRot, float degrees) {
	FRandomStream seed = newSeed;
	FRotator newRotator = originalRot + FRotator(0, seed.FRandRange(-degrees, degrees), 0);
	FRotator rotator = newRotator;
	return rotator;
}

/*controllo che il segmento corrente non sia troppo vicino ai segmenti già salvati.
Sono semplici distanze euclidee tra i punti centrali tra coppie di segmenti.
Ritorna false se tale distanza è minore di 10000 (vado sul sicuro)*/
bool AGeneration::distanceCheck(TArray<FRoad*> &roads, FRoad* current) {
	for (FRoad* road : roads) {
		if (FVector::Dist((road->vector2 + road->vector1) / 2, (current->vector2 + current->vector1) / 2) < 10000) {
			return false;
		}
	}
	return true;
}

//La funzione prende in ingresso la priorityqueue e la strada corrente considerata(appena estratta dalla coda).
void AGeneration::addRoad(std::priority_queue<FRoad*, std::deque<FRoad*>, priorityComparator> &queue, FRoad* currentRoad) {
	float mainRoadSize = 2000.0f;
	float sndRoadSize = 500.0f;
	FRandomStream seed = newSeed;
	
	//se current è una strada principale e la condizione sul MaxMainPath è rispettata posso chiamare addForward(params);
	if (currentRoad->type == RoadType::main) {
		if (currentRoad->lengthControl < maxMainPath)
			addForward(queue, RoadType::main, currentRoad, mainRoadSize);

		/*con una probabilità molto maggiore verrà invocata per due volte la funzione addSide(una per aggiunta
		sulla sinistra e una per l'aggiunta sulla destra) per aggiungere una strada secondaria;con una probabilità 
		bassa addSide verrà invocata per aggiungere alla coda una strada principale*/
		if (seed.FRandRange(0, 1) < 0.1)
			addSide(queue, RoadType::main, currentRoad, mainRoadSize, true);
		else
			addSide(queue, RoadType::secondary, currentRoad, sndRoadSize, true);
		if (seed.FRandRange(0, 1) < 0.1)
			addSide(queue, RoadType::main, currentRoad, mainRoadSize, false);
		else
			addSide(queue, RoadType::secondary, currentRoad, sndRoadSize, false);
	}
	/*Se currentRoad è una strada secondaria che rispetta il controllo su MaxSecondaryPath verranno 
	invocate addForward e addSide(due volte per l'aggiunta a destra e a sinistra)*/
	else if (currentRoad->type == RoadType::secondary) {
		if (currentRoad->lengthControl < maxSecondaryPath) {
			addForward(queue, RoadType::secondary, currentRoad, sndRoadSize);
			addSide(queue, RoadType::secondary, currentRoad, sndRoadSize, true);
			addSide(queue, RoadType::secondary, currentRoad, sndRoadSize, false);
		}
	}
}

//La funzione ha lo scopo di aggiungere alla coda di priorità una strada davanti a quella corrente considerata
void AGeneration::addForward(std::priority_queue<FRoad*, std::deque<FRoad*>, priorityComparator> &queue, RoadType newType, FRoad* previousRoad, float width) {
	FRoad* newRoad = new FRoad();
	FVector roadLength = FVector(20000,0,0);

	//Il vector1 della nuova strada corrisponde al vector2 di quella precedente
	newRoad->vector1 = previousRoad->vector2;
	FRotator rotator = randomRotation(previousRoad->rotation, degrees);
	newRoad->rotation = rotator;
	//il vector2 invece è la somma tra vector1 e vettore roadLength ruotato di y(rotation)* /
	newRoad->vector2 = newRoad->vector1 + newRoad->rotation.RotateVector(roadLength);
	
	//versore direzionale
	newRoad->initialDirectionVersor = previousRoad->vector2 - previousRoad->vector1;
	newRoad->initialDirectionVersor.Normalize();
	newRoad->finalDirectionVersor = newRoad->vector2 - newRoad->vector1;
	newRoad->finalDirectionVersor.Normalize();
	newRoad->width = width;

	/*La priorità del segmento è data dalla somma di : -val(numero tra(0, 1)) +
	+ la priorità della strada precedente * 0, 1 + (-1) se la strada è principale*/
	float val = Noise::getInstance()->noise(newRoad->vector2.X, newRoad->vector2.Y);
	newRoad->priority = -val + ((newRoad->type == RoadType::main) ? -1 : 0) + std::abs(0.1*previousRoad->priority);
	newRoad->previous = previousRoad;
	newRoad->lengthControl = previousRoad->lengthControl + 1;
	newRoad->type = previousRoad->type;

	//identifico i vertici e push() nelle queue
	vertices(newRoad);
	queue.push(newRoad);
}

//La funzione ha lo scopo di aggiungere alla coda di priorità una strada di lato a quella corrente considerata
void AGeneration::addSide(std::priority_queue<FRoad*, std::deque<FRoad*>, priorityComparator> &queue, RoadType newType, FRoad* previousRoad, float width, bool left) {
	FRoad* newRoad = new FRoad();
	FVector roadLength = FVector(20000, 0, 0);

	//Il vector1 della nuova strada corrisponde al punto centrale del segmento precedente 
	newRoad->vector1 = previousRoad->vector1 + (previousRoad->vector2 - previousRoad->vector1) / 2;
	
	/*Il vettore di rotazione è identificato con randomRotation(): passo la rotazione del segmento della strada precedente sommata ad un 
	vettore ruotato di 90° se left è true(si va a sinistra) o 270° se left è false(se si va a destra)*/
	FRotator rotator = randomRotation(previousRoad->rotation + (left ? FRotator(0, 90, 0) : FRotator(0, 270, 0)), degrees);
	newRoad->rotation = rotator;
	//il vector2 invece è la somma tra vector1 e vettore roadLength ruotato di y(rotation)* /
	newRoad->vector2 = newRoad->vector1 + newRoad->rotation.RotateVector(roadLength);
	
	//versore direzionale
	newRoad->initialDirectionVersor = FRotator(0, left ? 90 : 270, 0).RotateVector(previousRoad->vector2 - previousRoad->vector1);
	newRoad->initialDirectionVersor.Normalize();
	newRoad->finalDirectionVersor = newRoad->vector2 - newRoad->vector1;
	newRoad->finalDirectionVersor.Normalize();
	newRoad->width = width;
		
	/*La priorità del segmento è data dalla somma di : -val(numero tra(0, 1)) +
	+ la priorità della strada precedente * 0, 1 + (-0.5) se la strada è principale*/
	float val = Noise::getInstance()->noise(newRoad->vector2.X, newRoad->vector2.Y);
	newRoad->priority = -val + ((newRoad->type == RoadType::main) ? -0.5 : 0) + std::abs(0.1*previousRoad->priority);
	newRoad->previous = previousRoad;
	newRoad->lengthControl = (previousRoad->type == RoadType::main && newType != RoadType::main) ? 1 : previousRoad->lengthControl + 1;
	newRoad->type = newType;

	//identifico i vertici e push() nelle queue
	vertices(newRoad);
	queue.push(newRoad);
}

void AGeneration::BeginPlay()
{
	Super::BeginPlay();	
}

void AGeneration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}