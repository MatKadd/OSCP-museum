#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

//code by Mateusz Kądziela, 148271

sem_t sem;
int cap_a = 0, cap_b = 0, t_min = 0, t_max = 0;     //initializing global variables
float p = 0;

void *visitor(void* i) {
    int id = *(int*)i;
    int watch = rand() % (t_max - t_min) + t_min;   //getting a time for staying in a room for each visitor
    while(1) {              //entering hall A
        sem_wait(&sem);     //waiting for semaphore to open
        if (cap_a > 1) {    //if there is space in hall A, enter (we are always leaving one spot free, so people from B can exit easily)
            printf("Visitor %d has entered hall A\n", id);
            cap_a--;        //decreasing capacity left in hall A
            sem_post(&sem); //releasing semaphore
            sleep(watch);   //watching art
            break;
        }
        sem_post(&sem);     //if there was no space in hall A, release semaphore
    }
    int rng = rand()%100;
    if (rng <= 100*p){      //deciding if one enters hall B
        while(1){           //entering hall B
            sem_wait(&sem); //waiting for semaphore to open
            if (cap_b > 0){ //if there is space in hall B, enter
                printf("Visitor %d has entered hall B\n", id);
                cap_a++;    //increasing capacity left in hall A
                cap_b--;    //decreasing capacity left in hall B
                sem_post(&sem); //releasing semaphore
                sleep(watch);   //watching art
                break;
            }
            sem_post(&sem); //if there was no space in hall B, release semaphore
        }
        //leaving museum from hall B (no need to check if we can leave through A because there is always one spot free there)
        sem_wait(&sem); //waiting for semaphore to open
        printf("Visitor %d has left hall B and then hall A\n", id);
        cap_b++;    //increasing capacity left in hall B
        sem_post(&sem); //releasing semaphore
    }
    else {  //leaving museum from hall A
        sem_wait(&sem); //waiting for semaphore to open
        printf("Visitor %d has left hall A\n", id);
        cap_a++;    //increasing capacity left in hall A
        sem_post(&sem); //releasing semaphore
    }
    return NULL;
}

int main() {
    int visitors = 0;
    srand(time(NULL));
    sem_init(&sem, 0, 1);   //initializing semaphore for threads, starting value 1
    //getting input
    puts("How many people will visit the museum?");
    scanf("%d", &visitors);
    puts("What is the capacity of hall A?");
    scanf("%d", &cap_a);
    puts("What is the capacity of hall B (smaller than A)?");
    scanf("%d", &cap_b);
    puts("What is the minimal time a visitor spends in a hall (in seconds)?");
    scanf("%d", &t_min);
    puts("What is the maximal time a visitor spends in a hall (in seconds)?");
    scanf("%d", &t_max);
    puts("What is the probability that a visitor enters hall B?");
    scanf("%f", &p);
    pthread_t threads[visitors]; //create an array of threads
    int array[visitors];    //array for numbers from 1 to visitors
    for (int i = 0; i < visitors; i++){
        array[i] = i+1;
    }
    //creating threads
    for (int i = 0; i < visitors; i++){
        pthread_create(&threads[i], NULL, visitor, &array[i]);
    }
    //terminating threads
    for (int i = 0; i < visitors; i++){
        pthread_join(threads[i], NULL);
    }
    //terminating semaphore
    sem_destroy(&sem);
    return 0;
}
