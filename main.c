
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define handle_error_en(en, msg) \
   do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
   do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define MAX_NUM_CHARS 50

pthread_mutex_t print_mutex;

typedef struct param_s {
   char* my_name;
   int* my_ints; 
   int num_ints;
} param_t;

typedef struct my_thread_s {
   pthread_t thread_id;
   int thread_num;
   param_t* param;
} my_thread_t;

void print_param( char* prefix, param_t* param ) {
   if ( param == NULL ) {
      printf( "ERROR: %s() params is NULL. Exiting\n", __func__ );
      exit(EXIT_FAILURE);
   }
   if ( prefix == NULL ) {
      printf( "ERROR: %s() prefix is NULL. Exiting\n", __func__ );
      exit(EXIT_FAILURE);
   }

   if ( param->my_name != NULL ) {
      printf( "%s My name is %s\n", prefix, param->my_name );
   }
   if ( param->my_ints != NULL ) {
      for ( int num_index = 0; num_index < param->num_ints; num_index++ ) {
         printf( "%s my_ints[%d] = %d\n", prefix, num_index, param->my_ints[num_index] );
      }
      printf( "\n" );
   }

}

void* my_func( void* thread_data ) {
   my_thread_t* lthread_data = ( my_thread_t* )thread_data;
   param_t* lparam = lthread_data->param;

   pthread_mutex_lock( &print_mutex );
   print_param( "Inside my_func", lparam );
   pthread_mutex_unlock( &print_mutex );

}


int main( int argc, char* argv[] ) {
   
   int num_threads = 1;

   if ( argc > 1 ) {
      char* end_ptr = NULL;
      num_threads = strtoul( argv[1], &end_ptr, 10 );
      if ( end_ptr == NULL ) {
         handle_error( "ERROR invalid argument." );
      }
   }

   my_thread_t* my_threads = malloc( num_threads * sizeof( my_thread_t ) );
   if ( my_threads == NULL ) {
      printf( "ERROR: Malloc failed for my_threads\n" );
      exit(EXIT_FAILURE);
   }
   
   param_t* params = malloc( num_threads * sizeof( param_t ) );
   if ( params == NULL ) {
      printf( "ERROR: Malloc failed for params\n" );
      exit(EXIT_FAILURE);
   }

   if ( pthread_mutex_init( &print_mutex, NULL ) ) {
      perror( "ERROR initializing the print_mutex\n" );
   } 

   for ( int thread_num = 0; thread_num < num_threads; thread_num++ ) {
      my_threads[thread_num].thread_id = thread_num + 1;
      params[thread_num].num_ints = num_threads;
      params[thread_num].my_ints = malloc( sizeof(int) * params[thread_num].num_ints );
      if ( params[thread_num].my_ints == NULL ) {
         printf( "ERROR: Malloc failed for params[%d].my_ints\n", thread_num );
         exit(EXIT_FAILURE);
      }
      for ( int index = 0; index < params[thread_num].num_ints; index++ ) {
         params[thread_num].my_ints[index] = ( thread_num * num_threads ) + index;
      }

      params[thread_num].my_name = malloc( sizeof(char) * MAX_NUM_CHARS );
      if ( params[thread_num].my_name == NULL ) {
         printf( "ERROR: Malloc failed for params[%d].my_name\n", thread_num );
         exit(EXIT_FAILURE);
      }
      sprintf( params[thread_num].my_name, "my_func number %d", thread_num );

#ifdef DEBUG
      print_param( "Inside main()", &(params[thread_num]) );
#endif

      my_threads[thread_num].param = &(params[thread_num]);

      if ( pthread_create( &my_threads[thread_num].thread_id, NULL, &my_func, 
               &my_threads[thread_num] ) ) {
         perror( "ERROR creating thread." );
      }

   } // end of for ( int thread_num = 0; thread_num < num_threads; thread_num++ ) {

   for ( int thread_num = 0; thread_num < num_threads; thread_num++ ) {
      if ( pthread_join( my_threads[thread_num].thread_id, NULL ) ) {
         perror( "ERROR joining thread." );
      }

      pthread_mutex_lock(&print_mutex);
      printf( "Joined with thread %d (id = %ld)\n",
            thread_num, my_threads[thread_num].thread_id );
      pthread_mutex_unlock(&print_mutex);
      free( params[thread_num].my_name );
      free( params[thread_num].my_ints );
   }

   pthread_mutex_destroy( &print_mutex );
   free( params );
   free( my_threads );
   exit( EXIT_SUCCESS );
}
