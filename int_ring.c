#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "util.h"

#define TAG 999
#define NAME_MAX_SIZE 256

int main(int argc, char** argv) {
    int ring_size, rank, out_msg, in_msg, recieve_rank, send_rank; 
    int i, comm_rounds;
    char *hostname;
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

        gethostname(hostname, NAME_MAX_SIZE);

        recieve_rank = (rank == 0)?(ring_size - 1):(rank - 1);
        send_rank = (rank + 1) % ring_size;

        out_msg = 0;
        //Only matters if comm_rounds = 0, else will be overwritten
        in_msg = 0; 

        for(i = 0; i < comm_rounds; i++) {
            //Send and recieve the message
            MPI_Irecv(&in_msg, 1, MPI_INT, recieve_rank, TAG,\
                     MPI_COMM_WORLD, &recieve_request);
            MPI_Isend(&out_msg, 1, MPI_INT, send_rank, TAG, \
                    MPI_COMM_WORLD, &send_request);

            //Wait for messages to be sent and recieved
            MPI_Wait(&send_request, &send_status);
            MPI_Wait(&recieve_request, &recieve_status);

            //Prepare to send the last message recieved.
            out_msg = in_msg +  rank;
        }

        printf("After %d comm. rounds, ring node %d (on %s) ended with %d.\n",\
                comm_rounds, rank, hostname, in_msg);

        free(hostname);
    }
    MPI_Finalize(); 

    return EXIT_SUCCESS;
}


