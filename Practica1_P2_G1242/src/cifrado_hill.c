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


int main(int argc, char *argv[]) {

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
    mpz_t **matrixTrans;
    mpz_t** matrixAdj;
    int c = 0;
    char filek[MAX_STR] = {0};
    char filein[MAX_STR] = {0};
    char fileout[MAX_STR] = {0};

    FILE* fk = NULL;
    FILE* fin = NULL;
    FILE* fout = NULL;

    mpz_init(m);

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
                    if (fin != NULL) {
						fclose(fin);
					}
					if (fout != NULL) {
						fclose(fout);
					}
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
                    if (fout != NULL) {
						fclose(fout);
					}
					if (fk != NULL) {
						fclose(fk);
					}
                    return EXIT_FAILURE;
                }
                break;
            case 'o':
                strcpy(fileout, optarg);
                fout = fopen(fileout, "wb");
                if (fout == NULL) {
                    printf("Error al abrir %s para escribir\n", fileout);
                    mpz_clear(m);
                    if (fin != NULL) {
						fclose(fin);
					}
					if (fk != NULL) {
						fclose(fk);
					}
                    return EXIT_FAILURE;
                }
                break;
            default:
                printf("Uso: %s {-C|-D} {-m |Zm|} {-n Nk} {-k filek} [-i filein] [-o fileout]\n", argv[0]);
                mpz_clear(m);
                if (fin != NULL) {
					fclose(fin);
				}
				if (fout != NULL) {
					fclose(fout);
				}
				if (fk != NULL) {
					fclose(fk);
				}
                return EXIT_FAILURE;
        }
    }

    if (modo == -1 || n == -1 || !mpz_sgn(m)) {
        printf("{-C|-D} {-m |Zm|} {-n Nk} {-k filek} son obligatorios\n");
        mpz_clear(m);
		if (fin != NULL) {
			fclose(fin);
		}
		if (fout != NULL) {
			fclose(fout);
		}
		if (fk != NULL) {
			fclose(fk);
		}
        return EXIT_FAILURE;
    }

    if (n < 2) {
		printf("La dimension de la matiz debe ser al menos 2");
		mpz_clear(m);
		if (fin != NULL) {
			fclose(fin);
		}
		if (fout != NULL) {
			fclose(fout);
		}
		if (fk != NULL) {
			fclose(fk);
		}
        return EXIT_FAILURE;
	}
    

    mpz_inits(a, det, NULL);

    matrix = malloc(sizeof (mpz_t) * n);

    if (!matrix) {
        printf("Error al reservar memoria\n");
        return -1;
    }

    for (i = 0; i < n; i++) {
        matrix[i] = malloc(sizeof (mpz_t) * n);

        if (!matrix[i]) {
            printf("Error al reservar memoria\n");
            return -1;
        }
    }

    for (i = 0; i < n; i++) {
    	for (j = 0; j < n; j++) {
    		mpz_init2(matrix[i][j], 1024);
    	}
	}

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            mpz_inp_str(matrix[i][j], fk, 10);
            toModM(matrix[i][j], m);
        }
    }

    determinante(matrix, n, det, m);
    gmp_printf("El valor del determinante de la matriz dada es = %Zd\n", det);

    matrixTrans = malloc(sizeof (mpz_t) * n);

    if (!matrixTrans) {
        printf("Error al reservar memoria\n");
        return -1;
    }

    for (i = 0; i < n; i++) {
        matrixTrans[i] = malloc(sizeof (mpz_t) * n);

        if (!matrixTrans[i]) {
            printf("Error al reservar memoria\n");
            return -1;
        }
    }

    for (i = 0; i < n; i++) {
    	for (j = 0; j < n; j++) {
    		mpz_init2(matrixTrans[i][j], 1024);
    	}
	}

    matrixTransposed(matrix, n, matrixTrans);

    for (i = 0; i < n; i++) {
        gmp_printf("|%Zd %Zd %Zd|\n", matrixTrans[i][0], matrixTrans[i][1], matrixTrans[i][2]);
    }

    printf("\n");

    matrixAdj = malloc(sizeof (mpz_t) * n);

    if (!matrixAdj) {
        printf("Error al reservar memoria\n");
        return -1;
    }

    for (i = 0; i < n; i++) {
        matrixAdj[i] = malloc(sizeof (mpz_t) * n);

        if (!matrixAdj[i]) {
            printf("Error al reservar memoria\n");
            return -1;
        }
    }

    for (i = 0; i < n; i++) {
    	for (j = 0; j < n; j++) {
    		mpz_init2(matrixAdj[i][j], 1024);
    	}
	}

    matrixAdjoint(matrixTrans, n, matrixAdj, m);

    for (i = 0; i < n; i++) {
        gmp_printf("|%Zd %Zd %Zd|\n", matrixAdj[i][0], matrixAdj[i][1], matrixAdj[i][2]);
    }

    //Esta pagina dice que no se libere el resultado de mpz_array_init
    //http://web.mit.edu/gnu/doc/html/gmp_4.html
    //free_mpz_matrix(matrix, n, n);
	if (fin != NULL) {
		fclose(fin);
	}
	if (fout != NULL) {
		fclose(fout);
	}

	fclose(fk);

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			mpz_clear(matrix[i][j]);
			mpz_clear(matrixTrans[i][j]);
			mpz_clear(matrixAdj[i][j]);
		}
	}

	for (i = 0; i < n; i++) {
		free(matrix[i]);
		free(matrixTrans[i]);
		free(matrixAdj[i]);
	}

	free(matrix);
	free(matrixTrans);
	free(matrixAdj);

	mpz_clears(m, a, det, NULL);
	
    return 0;
}