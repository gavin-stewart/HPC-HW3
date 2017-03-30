CC=mpicc

all: int_ring array_ring

int_ring: int_ring.c
	${CC} int_ring.c -o int_ring

array_ring: array_ring.c
	${CC} array_ring.c -o array_ring

clean:
	rm -f int_ring array_ring
