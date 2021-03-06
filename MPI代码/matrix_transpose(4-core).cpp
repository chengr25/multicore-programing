/*
* This program demonstrates the use of MPI_Alltoall when
* transpsong a square matrix.
* For simplicity, the number of processes is 4 and the dimension
* of the matrix is fixed to 128
*/

#include <stdio.h>
#include <mpi.h>

void
trans(double *a, int n)
/* transpose square matrix a, dimension nxn
* Consider this as a black box for the MPI course
*/

{
	int i, j;
	int ij, ji, l;
	double tmp;
	ij = 0;
	l = -1;
	for (i = 0; i < n; i++)
	{
		l += n + 1;
		ji = l;
		ij += i + 1;
		for (j = i + 1; j < n; j++)
		{
			tmp = a[ij];
			a[ij] = a[ji];
			a[ji] = tmp;
			ij++;
			ji += n;
		}
	}
}

int
main(int argc, char *argv[])
{
	double a[256][64];
	double b[256][64];
	double startwtime = 0.0, endwtime;
	int i, j, nprocs, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
	{
		printf("Transposing a 256x256 matrix, divided among %d processors\n",nprocs);
		startwtime = MPI_Wtime();
	}

	for (i = 0; i < 256; i++)
		for (j = 0; j < 64; j++)
			a[i][j] = 1000 * i + j + 64 * rank; 
											

	MPI_Alltoall(
		&a[0][0],	/* address of data to send  */
		64 * 64,	/* number of items to send to one process  */
		MPI_DOUBLE,	/* type of data  */
		&b[0][0],	/* address for receiving the data  */					/* NOTE: send data and receive data may NOT overlap */
		64 * 64,	/* number of items to receive					from any process  */
		MPI_DOUBLE,	/* type of receive data  */
		MPI_COMM_WORLD);

	/* MPI_Alltoall does not a transpose of the data received, we have to
	* do this ourself: */

	for (i = 0; i < 4; i++)
		trans(&b[i * 64][0], 64);

	/* now check the result */
	for (i = 0; i < 256; i++)
		for (j = 0; j < 64; j++)
		{
			if (b[i][j] != 1000 * (j + 64 * rank) + i)
			{
				printf("process %d found b[%d][%d] = %f, but %f was expected\n",
					rank, i, j, b[i][j], (double)(1000 * (j + 64 * rank) + i));
				MPI_Abort(MPI_COMM_WORLD, 1);
				return 1;
			}
		}
	if (rank == 0) {
		printf("Transpose seems ok\n");
		endwtime = MPI_Wtime();
		printf("wall clock time = %f\n", endwtime - startwtime);
	}

	MPI_Finalize();
	return 0;
}
