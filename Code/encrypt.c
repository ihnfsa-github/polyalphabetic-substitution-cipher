#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "det.h"
#define STRLEN 1000

void encrypt(int* X, int** A, int* B, int n, FILE* fp, int *loca, int *str);

int
main(int argc, char** argv)
{
	struct timeval start, end;
	gettimeofday(&start, NULL);

	int dimen, i, ch, count, strcount;
	int str[STRLEN] = {0};

	if (argc != 9)
		my_err_quit("usage: encrypt -c <Dimension> -i <Plaintext File> <Key A File> <Key B File> -o <Ciphertext File>");

	if ((dimen = (int)strtol(argv[2],NULL,10)) <= 1)
		my_err_quit("<Dimension> must be a number bigger than 1");

	int** A = (int**)malloc(dimen * sizeof(int *));
	for (i = 0; i < dimen; ++i)
		A[i] = (int*)malloc(dimen * sizeof(int));
	
	read_matrix(argv[5], A, dimen);

	int* B = (int *)malloc(dimen * sizeof(int));
	
	read_array(argv[6], B, dimen);
	
	// 清空目标文件内容
	FILE *cfp0 = fopen(argv[8],"w");
	fclose(cfp0);
	
	FILE *cfp = NULL;
	if ((cfp = fopen(argv[8],"a")) == NULL)
		perror("fopen error"), exit(1);

	FILE *pfp = NULL;
	if ((pfp = fopen(argv[4],"r")) == NULL)
		perror("fopen error"), exit(1);
	
	int* X = (int *)malloc(dimen * sizeof(int));
	int* loca = (int *)calloc(dimen, sizeof(int)); 

	count = 0, strcount = 0;
	while(1){
		ch = fgetc(pfp);
		if (ch >= 65 && ch <= 90)
			X[count] = ch - 65;
		else if (ch >= 97 && ch <= 122)
			X[count] = ch - 97;
		else if (ch == EOF)
			break;
		else {
			loca[count] = loca[count] + 1;
			str[strcount] = ch;
			strcount++;
			if (strcount == STRLEN)
				my_err_quit("non-alpha characters is too long");
			continue;
		}
		count++;
		if (count == dimen){
			count = 0;
			encrypt(X, A, B, dimen, cfp, loca, str);
			strcount = 0;
		}
	}
	
	if (count != 0) {
		for (i = count; i < dimen; ++i)
			X[i] = 0;
		encrypt(X, A, B, dimen, cfp, loca, str);
	}
	strcount = 0;
	if (loca[0] > 0){
		for(;loca[0] > 0; loca[0] = loca[0] - 1){
			fprintf(cfp, "%c", str[strcount]);
			strcount++;
		}
	}

	printf("\n");
	// 退出前操作
	fclose(pfp);
	fclose(cfp);

	for (i = 0; i < dimen; i++)
		free(*(A + i));
	free(A);
	
	free(B);

	free(X);
	
	gettimeofday(&end, NULL);
	long long total_time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	printf("Use time %lld us\n", total_time);
	
	return 0;
}

void encrypt(int* X, int** A, int* B, int n, FILE* fp, int *loca, int *str)
{
	int i, j, C, k;
	
	k = 0;
	for (i = 0; i < n; ++i) {
		if (loca[i] > 0){
			for (; loca[i] > 0; loca[i] = loca[i] - 1){
				fprintf(fp, "%c", (char)str[k]);
				k++;
			}
		}
		C = 0;
		for (j = 0; j < n; ++j) {
			C += mod(A[i][j] * X[j], MOD);
		}
		C = mod(C + B[i], MOD);
		fprintf(fp, "%c", (char)C + 65);
	}

	return;
}
