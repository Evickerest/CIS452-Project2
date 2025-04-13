#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_WAIT_TIME 200

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
	int hasEggs1;
	int hasMilk1;
	int hasButter1;
	int hasEggs2;
	int hasMilk2;
	int hasButter2;
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
	sem_t refrigerator1;
	sem_t refrigerator2;
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

void giveUpPantry(Baker* baker) {
	if (baker->hasFlour == 1) sem_post(&sem.flour);
	if (baker->hasSugar == 1) sem_post(&sem.sugar);
	if (baker->hasYeast == 1) sem_post(&sem.yeast);
	if (baker->hasBakingSoda == 1) sem_post(&sem.bakingSoda);
	if (baker->hasSalt == 1) sem_post(&sem.salt);
	if (baker->hasCinnamon == 1) sem_post(&sem.cinnamon);
	baker->hasFlour = 0;
	baker->hasSugar = 0;
	baker->hasYeast = 0;
	baker->hasBakingSoda = 0;
	baker->hasSalt = 0;
	baker->hasCinnamon = 0;
}

void giveUpRefrigerator(Baker* baker) {
	if (baker->hasEggs == 1) sem_post(&sem.eggs);
	if (baker->hasMilk == 1) sem_post(&sem.milk);
	if (baker->hasButter == 1) sem_post(&sem.butter);
	baker->hasEggs = 0;
	baker->hasMilk = 0;
	baker->hasButter = 0;
}

void giveUpMixing(Baker* baker) {
	if (baker->gotBowl == 1) sem_post(&sem.bowl);
	if (baker->gotSpoon == 1) sem_post(&sem.spoon);
	if (baker->gotMixer == 1) sem_post(&sem.mixer);
	baker->gotBowl = 0;
	baker->gotSpoon = 0;
	baker->gotMixer = 0;
}

// Releases everything the baker has at the moment
void giveUp(Baker* baker) {
	if (baker->hasFlour == 1) sem_post(&sem.flour);
	if (baker->hasSugar == 1) sem_post(&sem.sugar);
	if (baker->hasYeast == 1) sem_post(&sem.yeast);
	if (baker->hasBakingSoda == 1) sem_post(&sem.bakingSoda);
	if (baker->hasSalt == 1) sem_post(&sem.salt);
	if (baker->hasCinnamon == 1) sem_post(&sem.cinnamon);
	if (baker->hasEggs == 1) sem_post(&sem.eggs);
	if (baker->hasMilk == 1) sem_post(&sem.milk);
	if (baker->hasButter == 1) sem_post(&sem.butter);
	if (baker->gotBowl == 1) sem_post(&sem.bowl);
	if (baker->gotSpoon == 1) sem_post(&sem.spoon);
	if (baker->gotMixer == 1) sem_post(&sem.mixer);
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
	int timer = 0;
	int finished = 0;

	// Game loop until recipe is finished
	while (finished == 0) {
		timer = 0;
		
		// If baker needs pantry and we haven't already entered, try semaphore
		if (needsPantry == 1 && baker->gotPantry == 0 && sem_trywait(&sem.pantry) == 0) {
			printf("%sBaker %d is entering the pantry.\n", baker->color, baker->bakerId);

			// Keep trying ingredients until we get everything we need
			while (!((baker->hasFlour     | (needsFlour == 0)) &
				    (baker->hasSugar      | (needsSugar == 0)) &
				    (baker->hasYeast      | (needsYeast == 0)) &
				    (baker->hasBakingSoda | (needsBakingSoda == 0)) &
				    (baker->hasSalt       | (needsSalt == 0)) &
				    (baker->hasCinnamon   | (needsCinnamon == 0)))) {
				timer++;

				if (needsFlour      && !baker->hasFlour      && sem_trywait(&sem.flour) == 0)      {
					printf("%sBaker %d has gotten flour!\n", baker->color, baker->bakerId);
					baker->hasFlour = 1;
				}
				if (needsSugar      && !baker->hasSugar      && sem_trywait(&sem.sugar) == 0)      {
					printf("%sBaker %d has gotten sugar!\n", baker->color, baker->bakerId);
					baker->hasSugar = 1;
				}
				if (needsYeast      && !baker->hasYeast      && sem_trywait(&sem.yeast) == 0)      {
					printf("%sBaker %d has gotten yeast!\n", baker->color, baker->bakerId);
					baker->hasYeast = 1;
				}
				if (needsBakingSoda && !baker->hasBakingSoda && sem_trywait(&sem.bakingSoda) == 0) {
					printf("%sBaker %d has gotten baking soda!\n", baker->color, baker->bakerId);
					baker->hasBakingSoda = 1;
				}
				if (needsSalt       && !baker->hasSalt       && sem_trywait(&sem.salt) == 0)       {
					printf("%sBaker %d has gotten salt!\n", baker->color, baker->bakerId);
					baker->hasSalt = 1;
				}
				if (needsCinnamon   && !baker->hasCinnamon   && sem_trywait(&sem.cinnamon) == 0)   {
					printf("%sBaker %d has gotten cinnamon!\n", baker->color, baker->bakerId);
					baker->hasCinnamon = 1;
				}

				/*printf("Baker %d: Current time: %d/%d\n", baker->bakerId, timer, waitTime);*/

				// If we expand the max amount of trying time
				if (timer > MAX_WAIT_TIME) {
					printf("%sBaker %d is bailing out of the pantry.\n", baker->color, baker->bakerId);
					giveUpPantry(baker);
					sem_post(&sem.pantry);
					break;
				}
			}

			// If we did not expand max waiting time, i.e., we got everything, exit and release pantry 
			if (timer <= MAX_WAIT_TIME) {
				printf("%sBaker %d has gotten everything needed from the pantry!\n", baker->color, baker->bakerId); 
				sem_post(&sem.pantry);
				baker->gotPantry = 1;
			}
		}
		timer = 0;

		// If we need the refrigerator and we haven't entered it yet, try semaphore
		if (needsRefrigerator == 1 && baker->gotRefrigerator == 0 && sem_trywait(&sem.refrigerator) == 0) {
			printf("%sBaker %d is entering the refrigerator.\n", baker->color, baker->bakerId);

			// Loop until we get everything we need
			while (!((baker->hasEggs  | (needsEggs == 0)) &
				    (baker->hasMilk   | (needsMilk == 0)) &
				    (baker->hasButter | (needsButter == 0)))) {
				timer++;
				if (needsEggs   && !baker->hasEggs   && sem_trywait(&sem.eggs) == 0)   {
					printf("%sBaker %d has gotten eggs!\n", baker->color, baker->bakerId);
					baker->hasEggs = 1;
				}
				if (needsMilk   && !baker->hasMilk   && sem_trywait(&sem.milk) == 0)   {
					printf("%sBaker %d has gotten Milk!\n", baker->color, baker->bakerId);
					baker->hasMilk = 1;
				}
				if (needsButter && !baker->hasButter && sem_trywait(&sem.butter) == 0) {
					printf("%sBaker %d has gotten butter!\n", baker->color, baker->bakerId);
					baker->hasButter = 1;
				}

				/*printf("Baker %d: Current time: %d/%d\n", baker->bakerId, timer, waitTime);*/

				if (timer > MAX_WAIT_TIME) {
					printf("%sBaker %d is bailing out of the refrigerator.\n", baker->color, baker->bakerId);
					giveUpRefrigerator(baker);
					sem_post(&sem.refrigerator);
					break;
				}
			}

			if (timer <= MAX_WAIT_TIME) {
				printf("%sBaker %d has gotten everything needed from the refrigerator!\n", baker->color, baker->bakerId);
				sem_post(&sem.refrigerator);
				baker->gotRefrigerator = 1;
			}
		}
		timer = 0;
		if (baker->gotMixed == 0 && (baker->gotPantry == 1 || needsPantry == 0) && (baker->gotRefrigerator == 1 || needsRefrigerator == 0)) {
			printf("%sBaker %d has gotten all the ingredients, trying to acquire mixing requirements.\n", baker->color, baker->bakerId);

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
				baker->gotMixed = 1;
			}
		}
		timer = 0;

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
		giveUp(&baker);

		baker.gotPantry = 0;
		baker.gotRefrigerator = 0;
		baker.gotMixed = 0;
		baker.gotOven = 0;

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

	// Seed random number generator for Ramsied selection
	srand(time(NULL));

	// Initialize Semaphores
	int returns[19];
	puts("Setting up semaphores...");
	returns[0] = sem_init(&sem.mixer, 0, 2);
	returns[1] = sem_init(&sem.pantry, 0, 1);
	returns[2] = sem_init(&sem.refrigerator1, 0, 1);
	returns[3] = sem_init(&sem.refrigerator2, 0, 1);
	returns[4] = sem_init(&sem.bowl, 0, 3);
	returns[5] = sem_init(&sem.spoon, 0, 5);
	returns[6] = sem_init(&sem.oven, 0, 1);
	returns[7] = sem_init(&sem.flour, 0, 1);
	returns[8] = sem_init(&sem.sugar, 0, 1);
	returns[9] = sem_init(&sem.yeast, 0, 1);
	returns[10] = sem_init(&sem.bakingSoda, 0, 1);
	returns[11] = sem_init(&sem.salt, 0, 1);
	returns[12] = sem_init(&sem.cinnamon, 0, 1);
	returns[13] = sem_init(&sem.eggs1, 0, 1);
	returns[14] = sem_init(&sem.milk1, 0, 1);
	returns[15] = sem_init(&sem.butter1, 0, 1);
	returns[16] = sem_init(&sem.eggs2, 0, 1);
	returns[17] = sem_init(&sem.milk2, 0, 1);
	returns[18] = sem_init(&sem.butter2, 0, 1);

	// Check success for sem_init
	for (int i = 0; i < 15; i++) {
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
	returns[2] = sem_destroy(&sem.refrigerator1);
	returns[3] = sem_destroy(&sem.refrigerator2);
	returns[4] = sem_destroy(&sem.bowl);
	returns[5] = sem_destroy(&sem.spoon);
	returns[6] = sem_destroy(&sem.oven);
	returns[7] = sem_destroy(&sem.flour);
	returns[8] = sem_destroy(&sem.sugar);
	returns[9] = sem_destroy(&sem.yeast);
	returns[10] = sem_destroy(&sem.bakingSoda);
	returns[11] = sem_destroy(&sem.salt);
	returns[12] = sem_destroy(&sem.cinnamon);
	returns[13] = sem_destroy(&sem.eggs1);
	returns[14] = sem_destroy(&sem.milk1);
	returns[15] = sem_destroy(&sem.butter1);
	returns[16] = sem_destroy(&sem.eggs2);
	returns[17] = sem_destroy(&sem.milk2);
	returns[18] = sem_destroy(&sem.butter2);

	// Check success for sem_destroy 
	for (int i = 0; i < 15; i++) {
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
