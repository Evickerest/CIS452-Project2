#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>


int NUM_BAKERS; 


// Semaphores for resources (shared resources in the kitchen)
sem_t mixer[2];        
sem_t pantry;          
sem_t refrigerator[2]; 
sem_t bowl[3];           
sem_t spoon[5];          
sem_t oven;            


// Semaphores for specific ingredients (1 per ingredient)
sem_t flour, sugar, yeast, baking_soda, salt, cinnamon;
sem_t egg, milk, butter;


// Recipe definitions
typedef enum {
   COOKIES,
   PANCAKES,
   PIZZA_DOUGH,
   PRETZELS,
   CINNAMON_ROLLS
} Recipe;


const char *recipe_names[] = {"Cookies", "Pancakes", "Pizza Dough", "Pretzels", "Cinnamon Rolls"};


// Function to acquire resources for the recipes in a consistent order to avoid deadlock
void acquire_ingredients_for_recipe(Recipe recipe, int baker_id) {
   // Acquire resources in a consistent order to avoid deadlock
   switch (recipe) {
       case COOKIES:
           sem_wait(&flour);   // Always acquire flour first
           printf("\033[31mBaker %d acquired Flour for Cookies\033[0m\n", baker_id);
           sem_wait(&sugar);   // Then acquire sugar
           printf("\033[31mBaker %d acquired Sugar for Cookies\033[0m\n", baker_id);
           sem_wait(&milk);    // Then acquire milk
           printf("\033[31mBaker %d acquired Milk for Cookies\033[0m\n", baker_id);
           sem_wait(&butter);  // Then acquire butter
           printf("\033[31mBaker %d acquired Butter for Cookies\033[0m\n", baker_id);
           break;
       case PANCAKES:
           sem_wait(&flour);       // Always acquire flour first
           printf("\033[32mBaker %d acquired Flour for Pancakes\033[0m\n", baker_id);
           sem_wait(&sugar);       // Then acquire sugar
           printf("\033[32mBaker %d acquired Sugar for Pancakes\033[0m\n", baker_id);
           sem_wait(&baking_soda); // Then acquire baking soda
           printf("\033[32mBaker %d acquired Baking Soda for Pancakes\033[0m\n", baker_id);
           sem_wait(&salt);        // Then acquire salt
           printf("\033[32mBaker %d acquired Salt for Pancakes\033[0m\n", baker_id);
           sem_wait(&egg);         // Then acquire egg
           printf("\033[32mBaker %d acquired Egg for Pancakes\033[0m\n", baker_id);
           sem_wait(&milk);        // Then acquire milk
           printf("\033[32mBaker %d acquired Milk for Pancakes\033[0m\n", baker_id);
           sem_wait(&butter);      // Then acquire butter
           printf("\033[32mBaker %d acquired Butter for Pancakes\033[0m\n", baker_id);
           break;
       case PIZZA_DOUGH:
           sem_wait(&yeast);  // Always acquire yeast first
           printf("\033[33mBaker %d acquired Yeast for Pizza Dough\033[0m\n", baker_id);
           sem_wait(&sugar);  // Then acquire sugar
           printf("\033[33mBaker %d acquired Sugar for Pizza Dough\033[0m\n", baker_id);
           sem_wait(&salt);   // Then acquire salt
           printf("\033[33mBaker %d acquired Salt for Pizza Dough\033[0m\n", baker_id);
           break;
       case PRETZELS:
           sem_wait(&flour);      // Always acquire flour first
           printf("\033[34mBaker %d acquired Flour for Pretzels\033[0m\n", baker_id);
           sem_wait(&sugar);      // Then acquire sugar
           printf("\033[34mBaker %d acquired Sugar for Pretzels\033[0m\n", baker_id);
           sem_wait(&salt);       // Then acquire salt
           printf("\033[34mBaker %d acquired Salt for Pretzels\033[0m\n", baker_id);
           sem_wait(&yeast);      // Then acquire yeast
           printf("\033[34mBaker %d acquired Yeast for Pretzels\033[0m\n", baker_id);
           sem_wait(&baking_soda); // Then acquire baking soda
           printf("\033[34mBaker %d acquired Baking Soda for Pretzels\033[0m\n", baker_id);
           sem_wait(&egg);        // Then acquire egg
           printf("\033[34mBaker %d acquired Egg for Pretzels\033[0m\n", baker_id);
           break;
       case CINNAMON_ROLLS:
           sem_wait(&flour);      // Always acquire flour first
           printf("\033[35mBaker %d acquired Flour for Cinnamon Rolls\033[0m\n", baker_id);
           sem_wait(&sugar);      // Then acquire sugar
           printf("\033[35mBaker %d acquired Sugar for Cinnamon Rolls\033[0m\n", baker_id);
           sem_wait(&salt);       // Then acquire salt
           printf("\033[35mBaker %d acquired Salt for Cinnamon Rolls\033[0m\n", baker_id);
           sem_wait(&butter);     // Then acquire butter
           printf("\033[35mBaker %d acquired Butter for Cinnamon Rolls\033[0m\n", baker_id);
           sem_wait(&egg);        // Then acquire egg
           printf("\033[35mBaker %d acquired Egg for Cinnamon Rolls\033[0m\n", baker_id);
           sem_wait(&cinnamon);   // Then acquire cinnamon
           printf("\033[35mBaker %d acquired Cinnamon for Cinnamon Rolls\033[0m\n", baker_id);
           break;
   }
}


void release_ingredients_for_recipe(Recipe recipe) {
   switch (recipe) {
       case COOKIES:
           sem_post(&flour);
           sem_post(&sugar);
           sem_post(&milk);
           sem_post(&butter);
           break;
       case PANCAKES:
           sem_post(&flour);
           sem_post(&sugar);
           sem_post(&baking_soda);
           sem_post(&salt);
           sem_post(&egg);
           sem_post(&milk);
           sem_post(&butter);
           break;
       case PIZZA_DOUGH:
           sem_post(&yeast);
           sem_post(&sugar);
           sem_post(&salt);
           break;
       case PRETZELS:
           sem_post(&flour);
           sem_post(&sugar);
           sem_post(&salt);
           sem_post(&yeast);
           sem_post(&baking_soda);
           sem_post(&egg);
           break;
       case CINNAMON_ROLLS:
           sem_post(&flour);
           sem_post(&sugar);
           sem_post(&salt);
           sem_post(&butter);
           sem_post(&egg);
           sem_post(&cinnamon);
           break;
   }
}


// Function to assign a color to each baker
const char* get_baker_color(int baker_id) {
   switch (baker_id % 6) {
       case 0: return "\033[31m";  // Red
       case 1: return "\033[32m";  // Green
       case 2: return "\033[33m";  // Yellow
       case 3: return "\033[34m";  // Blue
       case 4: return "\033[35m";  // Magenta
       case 5: return "\033[36m";  // Cyan
       default: return "\033[37m"; // White
   }
}


// Baker thread function
void *baker(void *arg) {
   int baker_id = *(int *)arg;
   static int ramsied_baker = -1; // Holds the ID of the Ramsied baker
   const char *color = get_baker_color(baker_id);  // Get color for this baker


   // Only one baker will be Ramsied
   if (ramsied_baker == -1 && rand() % NUM_BAKERS == 0) {
       ramsied_baker = baker_id;
       printf("%sBaker %d has been Ramsied!\033[0m\n", color, baker_id); // Red color for Ramsied baker
   }


   // Step 1: Perform each recipe
   for (int i = 0; i < 5; i++) { // Loop through each recipe
       Recipe recipe = i;
       printf("%sBaker %d is starting recipe: %s\033[0m\n", color, baker_id, recipe_names[recipe]);


       // Acquire pantry ingredients (only one baker can be in the pantry at a time)
       sem_wait(&pantry);


       // Acquire ingredients for the current recipe
       acquire_ingredients_for_recipe(recipe, baker_id);


       // Access Mixing Resources: Bowl, Spoon, Mixer
       sem_wait(&bowl);
       sem_wait(&spoon);
       sem_wait(&mixer);


       // Mixing the ingredients
       printf("%sBaker %d is mixing ingredients for %s\033[0m\n", color, baker_id, recipe_names[recipe]);


       // Access Oven and bake
       sem_wait(&oven);
       printf("%sBaker %d is baking %s\033[0m\n", color, baker_id, recipe_names[recipe]);


       // After completion of the recipe
       printf("%sBaker %d has finished %s\033[0m\n", color, baker_id, recipe_names[recipe]);


       // Release all resources and ingredients used for the recipe
       sem_post(&oven);
       sem_post(&mixer);
       sem_post(&spoon);
       sem_post(&bowl);


       // Release the acquired ingredients
       release_ingredients_for_recipe(recipe);


       // Release pantry
       sem_post(&pantry);


       // If the baker was Ramsied, release all semaphores and restart the current recipe
       if (baker_id == ramsied_baker) {
           printf("%sBaker %d was Ramsied and is restarting recipe: %s\033[0m\n", color, baker_id, recipe_names[recipe]);
           // Release all semaphores and restart the recipe
           sem_post(&oven);
           sem_post(&mixer);
           sem_post(&spoon);
           sem_post(&bowl);
           release_ingredients_for_recipe(recipe);
           sem_post(&pantry);
           i--;  // Retry the same recipe
       }
   }


   return NULL;
}


int main() {
   // Prompt user for the number of bakers
   printf("Enter the number of bakers: ");
   scanf("%d", &NUM_BAKERS);


   // Initialize semaphores for resources
   sem_init(&mixer, 0, 2);        // 2 mixers available
   sem_init(&pantry, 0, 1);        // Only one baker can be in the pantry at a time
   sem_init(&refrigerator[0], 0, 1); // 2 refrigerators, only 1 can be in each at a time
   sem_init(&refrigerator[1], 0, 1);
   sem_init(&bowl, 0, 3);          // 3 bowls available
   sem_init(&spoon, 0, 5);         // 5 spoons available
   sem_init(&oven, 0, 1);          // Only 1 oven available


   // Ingredients semaphores for pantry and fridge
   sem_init(&flour, 0, 1);
   sem_init(&sugar, 0, 1);
   sem_init(&yeast, 0, 1);
   sem_init(&baking_soda, 0, 1);
   sem_init(&salt, 0, 1);
   sem_init(&cinnamon, 0, 1);
   sem_init(&egg, 0, 1);
   sem_init(&milk, 0, 1);
   sem_init(&butter, 0, 1);


   // Seed random number generator for Ramsied selection
   srand(time(NULL));


   // Create baker threads
   pthread_t *bakers = malloc(NUM_BAKERS * sizeof(pthread_t));
   int *baker_ids = malloc(NUM_BAKERS * sizeof(int));


   for (int i = 0; i < NUM_BAKERS; i++) {
       baker_ids[i] = i + 1;
       pthread_create(&bakers[i], NULL, baker, (void *)&baker_ids[i]);
   }


   // Wait for all baker threads to finish
   for (int i = 0; i < NUM_BAKERS; i++) {
       pthread_join(bakers[i], NULL);
   }


   // Clean up
   free(bakers);
   free(baker_ids);


   // Destroy semaphores
   sem_destroy(&mixer);
   sem_destroy(&pantry);
   sem_destroy(&refrigerator[0]);
   sem_destroy(&refrigerator[1]);
   sem_destroy(&bowl);
   sem_destroy(&spoon);
   sem_destroy(&oven);
   sem_destroy(&flour);
   sem_destroy(&sugar);
   sem_destroy(&yeast);
   sem_destroy(&baking_soda);
   sem_destroy(&salt);
   sem_destroy(&cinnamon);
   sem_destroy(&egg);
   sem_destroy(&milk);
   sem_destroy(&butter);


   return 0;
}




