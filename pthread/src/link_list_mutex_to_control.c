
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_rand.h"
#include "timer.h"


const int MAX_KEY = 100000000;


struct list_node_s {
   int    data;
   struct list_node_s* next;
};


struct      list_node_s* head = NULL;  
int         thread_count;
int         total_ops;
double      insert_percent;
double      search_percent;
double      delete_percent;
pthread_mutex_t mutex;
pthread_mutex_t count_mutex;
int         member_total=0, insert_total=0, delete_total=0;

/* Setup and cleanup */
void        Usage(char* prog_name);
void        Get_input(int* inserts_in_main_p);

/* Thread function */
void*       Thread_work(void* rank);

/* List operations */
int         Insert(int value);
void        Print(void);
int         Member(int value);
int         Delete(int value);
void        Free_list(void);
int         Is_empty(void);

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   long i; 
   int key, success, attempts;
   pthread_t* thread_handles;
   int inserts_in_main;
   unsigned seed = 1;
   double start, finish;

   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1],NULL,10);

   Get_input(&inserts_in_main);

   i = attempts = 0;
   while ( i < inserts_in_main && attempts < 2*inserts_in_main ) {
      key = my_rand(&seed) % MAX_KEY;
      success = Insert(key);
      attempts++;
      if (success) i++;
   }
//    printf("Inserted %ld keys in empty list\n", i);
//    printf("Before starting threads, list = \n");
//    Print();
//    printf("\n");


   thread_handles = malloc(thread_count*sizeof(pthread_t));
   pthread_mutex_init(&mutex, NULL);
   pthread_mutex_init(&count_mutex, NULL);

   GET_TIME(start);
   for (i = 0; i < thread_count; i++)
      pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);

   for (i = 0; i < thread_count; i++)
      pthread_join(thread_handles[i], NULL);
   GET_TIME(finish);
//    printf("Elapsed time = %e seconds\n", finish - start);
   printf("Total ops = %d\n", total_ops);
   printf("member ops = %d\n", member_total);
   printf("insert ops = %d\n", insert_total);
   printf("delete ops = %d\n", delete_total);

//    printf("After threads terminate, list = \n");
//    Print();
//    printf("\n");
// 
   Free_list();
   pthread_mutex_destroy(&mutex);
   pthread_mutex_destroy(&count_mutex);
   free(thread_handles);

   return 0;
} 


void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
} 

/*-----------------------------------------------------------------*/
void Get_input(int* inserts_in_main_p) {

   printf("Cuantas claves debe insertars en los hilos principales\n");
   scanf("%d", inserts_in_main_p);
   printf("CUantas operaciones totales debe ejcutarse\n");
   scanf("%d", &total_ops);
   printf("Porcentaje de operaciones que debe realizarce en buscar (entre 0 and 1)\n");
   scanf("%lf", &search_percent);
   printf("porcentaje de operaciones que debe ser en insercion(entre 0 and 1)\n");
   scanf("%lf", &insert_percent);
   delete_percent = 1.0 - (search_percent + insert_percent);
} 


int Insert(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   struct list_node_s* temp;
   int rv = 1;
   
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }

   if (curr == NULL || curr->data > value) {
      temp = malloc(sizeof(struct list_node_s));
      temp->data = value;
      temp->next = curr;
      if (pred == NULL)
         head = temp;
      else
         pred->next = temp;
   } else { 
      rv = 0;
   }

   return rv;
} 


void Print(void) {
   struct list_node_s* temp;
//    printf("list = ");
   temp = head;
   while (temp != (struct list_node_s*) NULL) {
//       printf("%d ", temp->data);
      temp = temp->next;
   }
   printf("\n");
}  /* Print */


/*-----------------------------------------------------------------*/
int  Member(int value) {
   struct list_node_s* temp;

   temp = head;
   while (temp != NULL && temp->data < value)
      temp = temp->next;

   if (temp == NULL || temp->data > value) {
//       printf("%d is not in the list\n", value);
      return 0;
   } else {
//       printf("%d is in the list\n", value);
      return 1;
   }
} 


int Delete(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   int rv = 1;

   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }
   
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) { /* first element in list */
         head = curr->next;
//          printf("Freeing %d\n", value);
         free(curr);
      } else { 
         pred->next = curr->next;
//          printf("Freeing %d\n", value);
         free(curr);
      }
   } else { 
      rv = 0;
   }

   return rv;
}  

/*-----------------------------------------------------------------*/
void Free_list(void) {
   struct list_node_s* current;
   struct list_node_s* following;

   if (Is_empty()) return;
   current = head; 
   following = current->next;
   while (following != NULL) {
//       printf("Freeing %d\n", current->data);
      free(current);
      current = following;
      following = current->next;
   }
//    printf("Freeing %d\n", current->data);
   free(current);
}  


int  Is_empty(void) {
   if (head == NULL)
      return 1;
   else
      return 0;
}  

void* Thread_work(void* rank) {
   long my_rank = (long) rank;
   int i, val;
   double which_op;
   unsigned seed = my_rank + 1;
   int my_member=0, my_insert=0, my_delete=0;
   int ops_per_thread = total_ops/thread_count;

   for (i = 0; i < ops_per_thread; i++) {
      which_op = my_drand(&seed);
      val = my_rand(&seed) % MAX_KEY;
      if (which_op < search_percent) {
         pthread_mutex_lock(&mutex);
         Member(val);
         pthread_mutex_unlock(&mutex);
         my_member++;
      } else if (which_op < search_percent + insert_percent) {
         pthread_mutex_lock(&mutex);
         Insert(val);
         pthread_mutex_unlock(&mutex);
         my_insert++;
      } else { 
         pthread_mutex_lock(&mutex);
         Delete(val);
         pthread_mutex_unlock(&mutex);
         my_delete++;
      }
   }
   pthread_mutex_lock(&count_mutex);
   member_total += my_member;
   insert_total += my_insert;
   delete_total += my_delete;
   pthread_mutex_unlock(&count_mutex);

   return NULL;
} 
