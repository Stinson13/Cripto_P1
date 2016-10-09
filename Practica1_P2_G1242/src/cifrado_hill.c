#include "../includes/algoritmos.h"

void free_mpz_matrix(mpz_t** mat, int rows, int cols) {
	int i, j;
	for(i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			mpz_clear(mat[i][j]);
		}
		free(mat[i]);
	}
	free(mat);
}

int main (int argc,char *argv[]) {
	
	if (argc < 7) {
		printf("Uso: %s {-C|-D} {-m |Zm|} {-n Nk} {-k filek} [-i filein] [-o fileout]\n", argv[0]);
		return 0;
	}
	
	int modo = -1;
	int n = -1;
	int i;
	int j;
	mpz_t m;
	mpz_t **matrix;
	mpz_t a;
	mpz_t det;
	int c = 0;
	char filek[MAX_STR] = {0};
	char filein[MAX_STR] = {0};
	char fileout[MAX_STR] = {0};
	
	FILE* fk = NULL;
	FILE* fin = NULL;
	FILE* fout = NULL;
	
	mpz_init (m);
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"C", no_argument, 0, 'C'},
		   {"D", no_argument, 0, 'D'},
		   {"m", required_argument, 0, 'm'},
		   {"n", required_argument, 0, 'n'},
		   {"k", required_argument, 0, 'k'},
		   {"i", required_argument, 0, 'i'},
		   {"o", required_argument, 0, 'o'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "+CDm:n:k:i:o:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
			case 'C':
				modo = CIFRAR;
				break;
			case 'D':
				modo = DESCIFRAR;
				break;
			case 'n':
				n = atoi(optarg);
				break;
			case 'k':
				strcpy(filek, optarg);
				fk = fopen(filek, "rb");
				if (fk == NULL) {
					printf("Error al abrir %s para leer\n", filek);
					mpz_clear(m);
					return EXIT_FAILURE;
				}
				break;
			case 'm':
				mpz_set_str(m, optarg, 10);
				break;
			case 'i':
				strcpy(filein, optarg);
				fin = fopen(filein, "rb");
				if (fin == NULL) {
					printf("Error al abrir %s para leer\n", filein);
					mpz_clear(m);
					return EXIT_FAILURE;
				}
				break;
			case 'o':
				strcpy(fileout, optarg);
				fout = fopen(fileout, "wb");
				if (fout == NULL) {
					printf("Error al abrir %s para escribir\n", fileout);
					mpz_clear(m);
					return EXIT_FAILURE;
				}
				break;
			default:
				printf("Uso: %s {-C|-D} {-m |Zm|} {-n Nk} {-k filek} [-i filein] [-o fileout]\n", argv[0]);
				mpz_clear(m);
				return EXIT_FAILURE;
		}
	}
	
	if (modo == -1 || n == -1 || !mpz_sgn(m)) {
		printf("{-C|-D} {-m |Zm|} {-n Nk} {-k filek} son obligatorios\n");
		mpz_clear(m);
		return EXIT_FAILURE;
	} else if (n < 1) {
		printf("La dimension de la matriz debe ser al menos 1\n");
		mpz_clear(m);
		return EXIT_FAILURE;
	}

	mpz_inits(a, det, NULL);

	matrix = (mpz_t**) malloc(sizeof (mpz_t*) * n);

	if (!matrix) {
		printf("Error al reservar memoria\n");
		mpz_clears(m, a, det, NULL);
		return -1;
	}

	for (i = 0; i < n; i++) {
		matrix[i] = (mpz_t*) malloc(sizeof(mpz_t) * n);

		if (!matrix[i]) {
			printf("Error al reservar memoria\n");
			mpz_clears(m, a, det, NULL);
			
			//libera punteros
			free_mpz_matrix(matrix, i, n);
			return -1;
		}

		for (j = 0; j < n; j++) {
			mpz_init(matrix[i][j]);
		}
	}
	
	//mpz_array_init obsoleto segun:
	//http://stackoverflow.com/questions/26908628/gmp-mpz-array-init-is-an-obsolete-function-how-should-we-initialize-mpz-arrays
	//mpz_array_init(**matrix, n, 512);

	/*for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			mpz_init(matrix[i][j]);
		}
	}*/

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			mpz_inp_str(matrix[i][j], fk, 10);
			gmp_printf("Matrix[%d][%d]= %Zd", i, j, matrix[i][i]);
		}
	}

	determinante(matrix, n, det);
	gmp_printf("El valor del determinante de la matriz dada es = %Zd", det);

	free_mpz_matrix(matrix, n, n);

	return 0;
}
