#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "util.h"

#define TAG 999
#define NAME_MAX_SIZE 256
#define ARRAY_SIZE 2000000

int main(int argc, char** argv) {
    int ring_size, rank, recieve_rank, send_rank; 
    int i, comm_rounds;
    char *hostname;
    char *in_msg, *out_msg;  //Pass characters.
    MPI_Request recieve_request, send_request;
    MPI_Status recieve_status, send_status;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ring_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("%d\b", argc);
    
    if(argc < 2) {
        if(rank == 0) {
            printf("Usage: %s rounds\n", argv[0]);
        }
    } else {
        comm_rounds = atoi(argv[1]);

        hostname = malloc(NAME_MAX_SIZE * sizeof(char));
        in_msg = malloc(ARRAY_SIZE * sizeof(char));
        out_msg = malloc(ARRAY_SIZE * sizeof(char));

        gethostname(hostname, NAME_MAX_SIZE);

        sprintf(out_msg, "Hi, I'm node %d running on %s.", rank, hostname);

        recieve_rank = (rank == 0)?(ring_size - 1):(rank - 1);
        send_rank = (rank + 1) % ring_size;

        for(i = 0; i < comm_rounds; i++) {
            //Send and recieve the message
            MPI_Irecv(in_msg, ARRAY_SIZE, MPI_CHAR, recieve_rank, TAG,\
                     MPI_COMM_WORLD, &recieve_request);
            MPI_Isend(out_msg, ARRAY_SIZE, MPI_CHAR, send_rank, TAG, \
                    MPI_COMM_WORLD, &send_request);

            //Wait for messages to be sent and recieved
            MPI_Wait(&send_request, &send_status);
            MPI_Wait(&recieve_request, &recieve_status);

            //Prepare to send the last message recieved.
            char *tmp = out_msg;
            out_msg = in_msg;
            in_msg = tmp;
        }

        printf("After %d comm. rounds, ring node %d (on %s) ended with message:\
\n\t\t\"%s\".\n", comm_rounds, rank, hostname, in_msg);

        free(hostname);
        free(out_msg);
        free(in_msg);
    }
    MPI_Finalize(); 

    return EXIT_SUCCESS;
}


