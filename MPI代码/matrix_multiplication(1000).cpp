#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>

int main(int argc, char *argv[])
{
	double start, stop;
	int i, j, k, l;
	int *a, *b, *c, *buffer, *ans;
	int size = 1000;
	int rank, numprocs, line;

	MPI_Init(NULL, NULL);//MPI Initialize
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);//获得当前进程号
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);//获得进程个数

	line = size / numprocs;//将数据分为(进程数)个块,主进程也要处理数据
	a = (int*)malloc(sizeof(int)*size*size);
	b = (int*)malloc(sizeof(int)*size*size);
	c = (int*)malloc(sizeof(int)*size*size);
	//缓存大小大于等于要处理的数据大小，大于时只需关注实际数据那部分
	buffer = (int*)malloc(sizeof(int)*size*line);//数据分组大小
	ans = (int*)malloc(sizeof(int)*size*line);//保存数据块计算的结果

											  //主进程对矩阵赋初值，并将矩阵N广播到各进程,将矩阵M分组广播到各进程
	if (rank == 0)
	{
		//从文件中读入矩阵
		FILE *fp;

		fopen_s(&fp,"a.txt", "r");//打开文件
		for (i = 0; i<1000; i++) //读数据
			for (j = 0; j<1000; j++)
				fscanf_s(fp, "%d", &a[i*size + j]);
		fclose(fp);//关闭文件

		fopen_s(&fp,"b.txt", "r");

		for (i = 0; i<1000; i++)
			for (j = 0; j<1000; j++)
				fscanf_s(fp, "%d", &b[i*size + j]);
		fclose(fp);

		start = MPI_Wtime();
		//将矩阵N发送给其他从进程
		for (i = 1; i<numprocs; i++)
		{
			MPI_Send(b, size*size, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		//依次将a的各行发送给各从进程
		for (l = 1; l<numprocs; l++)
		{
			MPI_Send(a + (l - 1)*line*size, size*line, MPI_INT, l, 1, MPI_COMM_WORLD);
		}
		//接收从进程计算的结果
		for (k = 1; k<numprocs; k++)
		{
			MPI_Recv(ans, line*size, MPI_INT, k, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//将结果传递给数组c
			for (i = 0; i<line; i++)
			{
				for (j = 0; j<size; j++)
				{
					c[((k - 1)*line + i)*size + j] = ans[i*size + j];
				}

			}
		}
		//计算a剩下的数据
		for (i = (numprocs - 1)*line; i<size; i++)
		{
			for (j = 0; j<size; j++)
			{
				int temp = 0;
				for (k = 0; k<size; k++)
					temp += a[i*size + k] * b[k*size + j];
				c[i*size + j] = temp;
			}
		}
		/*
		 fopen_s(&fp,"c.txt", "w");
		for (i = 0; i<size; i++) {
			for (j = 0; j<size; j++)
				fprintf(fp, "%d ", c[i*size + j]);
			fputc('\n', fp);
		}
		fclose(fp);*/
		//结果测试
		//统计时间
		stop = MPI_Wtime();

		printf("rank:%d time:%lfs\n", rank, stop - start);

		free(a);
		free(b);
		free(c);
		free(buffer);
		free(ans);
	}

	//其他进程接收数据，计算结果后，发送给主进程
	else
	{
		//接收广播的数据(矩阵b)
		MPI_Recv(b, size*size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		MPI_Recv(buffer, size*line, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//计算乘积结果，并将结果发送给主进程
		for (i = 0; i<line; i++)
		{
			for (j = 0; j<size; j++)
			{
				int temp = 0;
				for (k = 0; k<size; k++)
					temp += buffer[i*size + k] * b[k*size + j];
				ans[i*size + j] = temp;
			}
		}
		//将计算结果传送给主进程
		MPI_Send(ans, line*size, MPI_INT, 0, 3, MPI_COMM_WORLD);
	}

	MPI_Finalize();//结束

	return 0;
}