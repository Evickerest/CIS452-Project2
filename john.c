#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_WAIT_TIME 200

int ramsied_id = 0;
int has_remaised = 0;

// Recipe definitions
typedef enum {
   COOKIES,
   PANCAKES,
   PIZZA_DOUGH,
   PRETZELS,
   CINNAMON_ROLLS
} Recipe;

// Struct to record everything the baker has
typedef struct Baker {
	int bakerId;
	int hasFlour;
	int hasSugar;
	int hasYeast;
	int hasBakingSoda;
	int hasSalt;
	int hasCinnamon;
	int hasEggs;
	int hasMilk;
	int hasButter;
	int gotBowl;
	int gotSpoon;
	int gotMixer;
	int gotOven;
	int gotPantry;
	int gotRefrigerator;
	int gotMixed;
	int gotCooked;
	Recipe currentRecipe;
	char* color;
} Baker;

// Struct to hold semaphores
typedef struct Sem {
	sem_t mixer;
	sem_t pantry;
	sem_t refrigerator;
	sem_t bowl;
	sem_t spoon;
	sem_t oven;
	sem_t flour;
	sem_t sugar;
	sem_t yeast;
	sem_t bakingSoda;
	sem_t salt;
	sem_t cinnamon;
	sem_t eggs;
	sem_t milk;
	sem_t butter;
} Sem;

Sem sem;
const char *recipe_names[] = {"Cookies", "Pancakes", "Pizza Dough", "Pretzels", "Cinnamon Rolls"};

// Gets a random ANSI color string
char* getBakerColor() { 
	int red = rand() % 256;
	int green = rand() % 256;
	int blue = rand() % 256;
	char* buffer = malloc(sizeof(char) * 32);
	snprintf(buffer, 32, "\033[38;2;%d;%d;%dm", red, green, blue);
	return buffer;
}

// Let go off all mixing ingredients
void giveUpMixing(Baker* baker) {
	if (baker->gotBowl == 1) sem_post(&sem.bowl);
	if (baker->gotSpoon == 1) sem_post(&sem.spoon);
	if (baker->gotMixer == 1) sem_post(&sem.mixer);
	baker->gotBowl = 0;
	baker->gotSpoon = 0;
	baker->gotMixer = 0;
}

// Releases everything the baker has at the moment
void releaseIngredients(Baker* baker) {
	baker->hasFlour = 0;
	baker->hasSugar = 0;
	baker->hasYeast = 0;
	baker->hasBakingSoda = 0;
	baker->hasSalt = 0;
	baker->hasCinnamon = 0;
	baker->hasEggs = 0;
	baker->hasMilk = 0;
	baker->hasButter = 0;
	baker->gotBowl = 0;
	baker->gotSpoon = 0;
	baker->gotMixer = 0;
	baker->gotPantry = 0;
	baker->gotRefrigerator = 0;
	baker->gotMixed = 0;
	baker->gotOven = 0;
}

// The function for the baker to acquire everything, mix it, cook it
void acquireIngredients(
	Baker* baker,
	int needsFlour,
	int needsSugar,
	int needsYeast,
	int needsBakingSoda,
	int needsSalt,
	int needsCinnamon,
	int needsEggs,
	int needsMilk,
	int needsButter,
	int needsPantry,
	int needsRefrigerator
) {
	int finished = 0;
	int timer = 0;

	// Game loop until recipe is finished
	while (finished == 0) {
		// If baker needs pantry and we haven't already entered, try semaphore
		if (needsPantry == 1 && baker->gotPantry == 0 && sem_trywait(&sem.pantry) == 0) {
			printf("%sBaker %d is entering the pantry.\n", baker->color, baker->bakerId);

			// While loop so that we can break out of execution when we get our 1 ingredient
			while(1) {
				if (needsFlour      && !baker->hasFlour)      {
					printf("%sBaker %d has gotten flour!\n", baker->color, baker->bakerId);
					baker->hasFlour = 1;
					break;
				}
				if (needsSugar      && !baker->hasSugar)      {
					printf("%sBaker %d has gotten sugar!\n", baker->color, baker->bakerId);
					baker->hasSugar = 1;
					break;
				}
				if (needsYeast      && !baker->hasYeast)      {
					printf("%sBaker %d has gotten yeast!\n", baker->color, baker->bakerId);
					baker->hasYeast = 1;
					break;
				}
				if (needsBakingSoda && !baker->hasBakingSoda ) {
					printf("%sBaker %d has gotten baking soda!\n", baker->color, baker->bakerId);
					baker->hasBakingSoda = 1;
					break;
				}
				if (needsSalt       && !baker->hasSalt      )       {
					printf("%sBaker %d has gotten salt!\n", baker->color, baker->bakerId);
					baker->hasSalt = 1;
					break;
				}
				if (needsCinnamon   && !baker->hasCinnamon )   {
					printf("%sBaker %d has gotten cinnamon!\n", baker->color, baker->bakerId);
					baker->hasCinnamon = 1;
					break;
				}
				break;
			}

			// Check if the baker has gotten everything it needs
			if ((baker->hasFlour      | (needsFlour == 0)) &
				(baker->hasSugar      | (needsSugar == 0)) &
				(baker->hasYeast      | (needsYeast == 0)) &
				(baker->hasBakingSoda | (needsBakingSoda == 0)) &
				(baker->hasSalt       | (needsSalt == 0)) &
				(baker->hasCinnamon   | (needsCinnamon == 0))) {
				baker->gotPantry = 1;
			}

			sem_post(&sem.pantry);
		}

		if (needsRefrigerator == 1 && baker->gotRefrigerator == 0 && sem_trywait(&sem.refrigerator) == 0) {
			printf("%sBaker %d is entering the refrigerator.\n", baker->color, baker->bakerId);

			while (1) {
				if (needsEggs  && !baker->hasEggs  )   {
					printf("%sBaker %d has gotten eggs!\n", baker->color, baker->bakerId);
					baker->hasEggs = 1;
					break;
				}
				if (needsMilk   && !baker->hasMilk  )   {
					printf("%sBaker %d has gotten Milk!\n", baker->color, baker->bakerId);
					baker->hasMilk = 1;
					break;
				}
				if (needsButter && !baker->hasButter) {
					printf("%sBaker %d has gotten butter!\n", baker->color, baker->bakerId);
					baker->hasButter = 1;
					break;
				}
				break;
			}

			// Check if the baker got everything from the refrigerator
			if ((baker->hasEggs   | (needsEggs == 0)) &
				(baker->hasMilk   | (needsMilk == 0)) &
				(baker->hasButter | (needsButter == 0))) {
				baker->gotRefrigerator = 1;
			}

			sem_post(&sem.refrigerator);
		}

		timer = 0;	

		// Acquire mixing ingridents
		if (baker->gotMixed == 0 && (baker->gotPantry == 1 || needsPantry == 0) && (baker->gotRefrigerator == 1 || needsRefrigerator == 0)) {
			printf("%sBaker %d has gotten all the ingredients, trying to acquire mixing requirements.\n", baker->color, baker->bakerId);

			// While the baker hasen't gotten everything for mixing
			while (!(baker->gotBowl  &
					 baker->gotSpoon &
					 baker->gotMixer)) {
				timer++;

				if (!baker->gotBowl  && sem_trywait(&sem.bowl) == 0)  {
					printf("%sBaker %d has gotten bowl!\n", baker->color, baker->bakerId);
					baker->gotBowl = 1;
				}
				if (!baker->gotSpoon && sem_trywait(&sem.spoon) == 0) {
					printf("%sBaker %d has gotten spoon!\n", baker->color, baker->bakerId);
					baker->gotSpoon = 1;
				}
				if (!baker->gotMixer && sem_trywait(&sem.mixer) == 0) {
					printf("%sBaker %d has gotten mixer!\n", baker->color, baker->bakerId);
					baker->gotMixer = 1;
				}

				/*printf("Baker %d: Current Time: %d/%d\n", baker->bakerId, timer, waitTime);*/

				if (timer > MAX_WAIT_TIME) {
					printf("%sBaker %d is bailing out on acquiring mixing requirments.\n", baker->color, baker->bakerId);
					giveUpMixing(baker);
					break;
				}
			}

			if (timer <= MAX_WAIT_TIME) {
				printf("%sBaker %d has gotten all the mixing requirments!\n", baker->color, baker->bakerId);
				sem_post(&sem.bowl);
				sem_post(&sem.spoon);
				sem_post(&sem.mixer);
				baker->gotMixed = 1;
			}
		}

		if (baker->gotMixed == 1) {
			printf("%sBaker %d is trying to get the oven.\n", baker->color, baker->bakerId);

			while (baker->gotOven == 0) {
				if(sem_trywait(&sem.oven) == 0) baker->gotOven = 1;
			}

			sem_post(&sem.oven);
			printf("%sBaker %d has gotten oven!\n", baker->color, baker->bakerId);
			printf("%sBaker %d has cooked the recipie.\n", baker->color, baker->bakerId);
			finished = 1;
		}
	}
};

// Function to acquire resources for the recipes in a consistent order to avoid deadlock
void cook(Baker* baker, Recipe recipe) {
	switch(recipe) {
		case COOKIES:
			//flour, sugar, yeast, bakingSoda, salt, cinnamon, eggs, milk, butter, pantry, refrigerator
			acquireIngredients(baker, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1);
			break;
		case PANCAKES:
			acquireIngredients(baker, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1);
			break;
		case PIZZA_DOUGH:
			acquireIngredients(baker, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0);
			break;
		case PRETZELS:
			acquireIngredients(baker, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1);
			break;
		case CINNAMON_ROLLS:
			acquireIngredients(baker, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1);
			break;
	}
}

// Baker thread function
void *baker(void *arg) {

	Baker baker = *(Baker*)arg;
	char* color = baker.color;
	int id = baker.bakerId ;

	// Loop through all five recipies
	for (int i = 0; i < 5; i++) {
		Recipe recipe = (baker.currentRecipe + i) % 5;
		printf("%sBaker %d is starting recipe: %s\n", color, id, recipe_names[recipe]);
		cook(&baker, recipe);

		// Ramsey baker on 4th recipe
		if (ramsied_id == id && i == 3 && has_remaised == 0) {
    		i--;
			has_remaised = 1;
			printf("%sBaker %d has been ramsied!\n", color, id);
			releaseIngredients(&baker);
    		continue;
			
		}

		releaseIngredients(&baker);

		printf("%sBaker %d has finished recipe: %s\n", color, id, recipe_names[recipe]);
	}

	printf("%sBaker %d is done!\n", color, id); 
	return NULL;
}


int main() {
	int numBakers;

	// Prompt user for the number of bakers
	printf("Enter the number of bakers: ");
	scanf("%d", &numBakers);

	// ramseid random event 
	srand(time(NULL));
	ramsied_id = rand() % numBakers;

	// Initialize Semaphores
	int returns[6];
	puts("Setting up semaphores...");
	returns[0] = sem_init(&sem.mixer, 0, 2);
	returns[1] = sem_init(&sem.pantry, 0, 1);
	returns[2] = sem_init(&sem.refrigerator, 0, 2);
	returns[3] = sem_init(&sem.bowl, 0, 3);
	returns[4] = sem_init(&sem.spoon, 0, 5);
	returns[5] = sem_init(&sem.oven, 0, 1);

	// Check success for sem_init
	for (int i = 0; i < 6; i++) {
		if (returns[i] == -1) {
			printf("Error initalizing semaphore: %s\n", strerror(errno));
			return 1;
		}
	}

	// Create our Bakers
	puts("Creating bakers...");
	Baker *bakers = (Baker*)malloc(numBakers * sizeof(Baker));

	// Create baker object
	for (int i = 0; i < numBakers; i++) {
		char* color = getBakerColor(); 
		bakers[i] = (Baker){i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, rand() % 5, color};
	}

	// Create our baker threads
	pthread_t bakerThreads[numBakers];

	// Create pthreads
	puts("Initializing baker threads...");
	for (int i = 0; i < numBakers; i++) {
		if ((pthread_create(&bakerThreads[i], NULL, baker, (void *)&bakers[i])) == -1) {
			printf("Failed to create pthread %d: %s\n", i, strerror(errno));

			// Clean up the threads that have been spun up so far
			for (int j = 0; j <= i; j++) pthread_cancel(bakerThreads[j]);
			return 1;
		}
	}

	// Wait for all baker threads to finish
	puts("Waiting for baker threads...");
	for (int i = 0; i < numBakers; i++) pthread_join(bakerThreads[i], NULL);

	// Destroy semaphores
	returns[0] = sem_destroy(&sem.mixer);
	returns[1] = sem_destroy(&sem.pantry);
	returns[2] = sem_destroy(&sem.refrigerator);
	returns[3] = sem_destroy(&sem.bowl);
	returns[4] = sem_destroy(&sem.spoon);
	returns[5] = sem_destroy(&sem.oven);

	// Check success for sem_destroy 
	for (int i = 0; i < 6; i++) {
		if (returns[i] == -1) {
			printf("Error destroying semaphore %d: %s\n", i, strerror(errno)); 
		}
	}

	// Free all the memory malloc'd
	for (int i = 0; i < numBakers; i++) {
		free(bakers[i].color);
	}
	free(bakers);

	return 0;
}
