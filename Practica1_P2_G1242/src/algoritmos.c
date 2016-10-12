#include "../includes/algoritmos.h"

void getGDC(mpz_t a, mpz_t b, mpz_t gdc) {

    mpz_t max;
    mpz_t min;
    mpz_t r;
    mpz_t q;

    if (mpz_cmp_ui(a, 0L) == 0 || mpz_cmp_ui(b, 0L) == 0) {
    	mpz_set_ui(gdc, 0L);
    	return;
    }

    mpz_inits(max, min, q, NULL);
    mpz_init_set_ui(r, 2L);
    //gmp_printf ("a es %Zd y b es %Zd\n", a, b);
    //tell which one is the divident and the divisor
    if (mpz_cmp(a, b) > 0) {
        mpz_set(max, a);
        mpz_set(min, b);
    } else {
        mpz_set(max, b);
        mpz_set(min, a);
    }

    while (1) {
        mpz_fdiv_qr(q, r, max, min);
        //gmp_printf ("Q es %Zd y R es %Zd\n", gdc, r);
        if (mpz_sgn(r) == 0)
            break;
        mpz_set(max, min);
        mpz_set(min, r);
    }
    mpz_set(gdc, min);

    mpz_clears(max, min, r, q, NULL);
}

int getMultInverse(mpz_t a, mpz_t b, mpz_t inverse) {

    mpz_t gdc;
    mpz_init(gdc);

    getGDC(a, b, gdc);

    if (mpz_cmp_ui(gdc, 1L)) {
        mpz_clear(gdc);
        return -1;
    }

    mpz_t max;
    mpz_t min;
    mpz_inits(max, min, NULL);

    if (mpz_cmp(a, b) > 0) {
        mpz_set(max, a);
        mpz_set(min, b);
    } else {
        mpz_set(max, b);
        mpz_set(min, a);
    }

    // Casos especiales
    if (!mpz_cmp_ui(min, 0L) || !mpz_cmp_ui(min, 1L)) {
        mpz_set(inverse, min);
        return 0;
    }

    mpz_t x;
    mpz_t y;
    mpz_t x1;
    mpz_t y1;
    mpz_t x2;
    mpz_t y2;
    mpz_t q;
    mpz_t r;
    mpz_t ret;
    mpz_t maxAux;
    mpz_inits(x, y, x1, y1, x2, y2, q, r, ret, maxAux, NULL);

    mpz_set(maxAux, max);
    mpz_set_ui(x2, 1L);
    mpz_set_ui(y1, 1L);

    while (mpz_cmp_ui(r, 1L)) {
        // max = q*min + r
        mpz_fdiv_qr(q, r, max, min);

        // x2 - q*x1
        mpz_mul(ret, q, x1);
        mpz_sub(x, x2, ret);

        // y2 - q*y1
        mpz_mul(ret, q, y1);
        mpz_sub(y, y2, ret);

        // Reajustar valores
        mpz_set(max, min);
        mpz_set(min, r);
        mpz_set(x2, x1);
        mpz_set(x1, x);
        mpz_set(y2, y1);
        mpz_set(y1, y);
    }

    if (mpz_cmp_ui(y, 0L) < 0) {
        mpz_add(ret, maxAux, y);
        mpz_set(inverse, ret);

    } else {
        mpz_set(inverse, y);
    }

    mpz_clears(gdc, max, min, x, y, x1, y1, x2, y2, q, r, ret, maxAux, NULL);
    return 0;
}

void toUpperOnly(char* src) {
    int i, j;
    for (i = 0, j = 0; src[i] != '\0'; i++) {
        if (isalpha(src[i])) {
            src[j] = (char) (toupper(src[i]));
            j++;
        }
    }
    src[j] = '\0';
}

void toModM(mpz_t x, mpz_t m) {
	mpz_t q;
	mpz_init(q);
	
	mpz_tdiv_q(q, x, m);
	
	if (mpz_sgn(x) < 0) {
		//|x| < m
		if (mpz_sgn(q) == 0) {
			//gmp_printf("(1) %Zd %% %Zd = %Zd + %Zd\n", x, m, x, m);
			mpz_add(x, x, m);
		} else {
			mpz_abs(q, q);
			mpz_add_ui(q, q, 1L);
			//gmp_printf(" (2) %Zd %% %Zd = %Zd + %Zd * %Zd\n", x, m, x, q, m);
			mpz_mul(q, m, q);
			mpz_add(x, x, q);
		}
	} else if (mpz_cmp(x, m) >= 0) {
		//gmp_printf(" (3) %Zd %% %Zd = %Zd - %Zd * %Zd\n", x, m, x, q, m);
		mpz_mul(q, m, q);
		mpz_sub(x, x, q);
	}
	//else x is already % m
	
	mpz_clear(q);
}

void determinante(mpz_t** matrix, int n, mpz_t det, mpz_t m) {

    int i;
    int j;
    int k;
    mpz_t result;
    mpz_t suma;

    if (n == 2) {
        mpz_t r1, r2;
        mpz_inits(r1, r2, NULL);
        mpz_mul(r1, matrix[0][0], matrix[1][1]);
        mpz_mul(r2, matrix[1][0], matrix[0][1]);
        mpz_sub(det, r1, r2);
        toModM(det, m);
        mpz_clears(r1, r2, NULL);
        return;
    }

    mpz_t **nm;

    nm = malloc(sizeof (mpz_t) * (n - 1));

    if (!nm) {
        printf("Error al reservar memoria\n");
        return;
    }

    for (i = 0; i < (n - 1); i++) {
        nm[i] = malloc(sizeof (mpz_t) * (n - 1));

        if (!nm[i]) {
            printf("Error al reservar memoria\n");
            return;
        }
    }

   	for (i = 0; i < (n - 1); i++) {
    	for (j = 0; j < (n - 1); j++) {
    		mpz_init2(nm[i][j], 1024);
    	}
	}

	mpz_inits(result, suma, NULL);

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
        	if (j != i) {
                for (k = 1; k < n; k++) {
                    int index = -1;

                    if (j < i) {
                        index = j;
                    } else if (j > i) {
                        index = j - 1;
                    }
                    mpz_set(nm[index][k - 1], matrix[j][k]);
                }
            }
        }
        
		determinante(nm, (n - 1), result, m);
            
        if (i % 2 == 0) {
			mpz_mul(result, matrix[i][0], result);
            mpz_add(suma, suma, result);

        } else {
        	mpz_mul(result, matrix[i][0], result);
            mpz_sub(suma, suma, result);
        }

        toModM(suma, m);
    }

    mpz_set(det, suma);

    mpz_clears(result, suma, NULL);
    
    for (i = 0; i < (n - 1); i++) {
		for (j = 0; j < (n - 1); j++) {
			mpz_clear(nm[i][j]);
		}
	}

	for (i = 0; i < (n - 1); i++) {
		free(nm[i]);
	}

	free(nm);

	return;
}

void matrixTransposed(mpz_t** matrix, int n, mpz_t** matrixTrans, mpz_t m) {

	int i;
	int j;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			mpz_set(matrixTrans[i][j], matrix[j][i]);
			toModM(matrixTrans[i][j], m);
		}
	}
}

void matrixCofactors(mpz_t** matrix, int n, mpz_t** matrixCof, mpz_t m) {

	int i;
	int j;
	int k;
	int l;
	int a;
	int b;
	int matrix_size_adj;
	mpz_t base;
	mpz_t rop;

	if (n == 2) {
		mpz_set(matrixCof[0][0], matrix[1][1]);
		mpz_mul_si(matrixCof[0][1], matrix[1][0], -1L);
		toModM(matrixCof[0][1], m);
		mpz_mul_si(matrixCof[1][0], matrix[0][1], -1L);	
		toModM(matrixCof[1][0], m);
		mpz_set(matrixCof[1][1], matrix[0][0]);	

		return;

	} else {
		matrix_size_adj = (n - 1);
	}

	mpz_inits(base, rop, NULL);

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			mpz_t** det;
			mpz_t valueDet;

			det = malloc(sizeof (mpz_t) * matrix_size_adj);

		    if (!det) {
		        printf("Error al reservar memoria\n");
		        return;
		    }

		    for (a = 0; a < matrix_size_adj; a++) {
		        det[a] = malloc(sizeof (mpz_t) * matrix_size_adj);

		        if (!det[a]) {
		            printf("Error al reservar memoria\n");
		            return;
		        }
		    }

		    for (a = 0; a < matrix_size_adj; a++) {
		    	for (b = 0; b < matrix_size_adj; b++) {
		    		mpz_init2(det[a][b], 1024);
		    	}
			}

			for (k = 0; k < n; k++) {
				if (k != i) {
					for (l = 0; l < n; l++) {
						if (l != j) {
							int index1 = k < i ? k : k-1;
							int index2 = l < j ? l : l-1;

							mpz_set(det[index1][index2], matrix[k][l]);
						}
					}
				}
			}

			mpz_init(valueDet);

			/*for (a = 0; a < (n - 1); a++) {
				gmp_printf("|%Zd %Zd|\n", det[a][0], det[a][1]);
			}
			printf("\n");*/

			determinante(det, matrix_size_adj, valueDet, m);

			mpz_set_si (base, -1L);
			mpz_pow_ui (rop, base, (long)(i+j+2));
			mpz_mul(matrixCof[i][j] , valueDet, rop);
			//gmp_printf("matrixCof[%d][%d] = %Zd\n", i, j, matrixCof[i][j]);
			toModM(matrixCof[i][j], m);

			for (a = 0; a < matrix_size_adj; a++) {
				for (b = 0; b < matrix_size_adj; b++) {
					mpz_clear(det[a][b]);
				}
			}

			for (a = 0; a < matrix_size_adj; a++) {
				free(det[a]);
			}

			free(det);

			mpz_clear(valueDet);
		}
	}

	mpz_clears(base, rop, NULL);

	return;
}

void matrixAdjoint(mpz_t** matrix, int n, mpz_t** matrixAdj, mpz_t m) {

	matrixCofactors(matrix, n, matrixAdj, m);
	return;
}

void mulMatrixConst(mpz_t cons, mpz_t** matrix, mpz_t** matrixRes, int n) {

	mpz_t res;

	mpz_init(res);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			mpz_mul(res, matrix[i][j], cons);
			mpz_set(matrixRes[i][j], res);
		}
	}

	mpz_clear(res);
	return;
}

int matrixInverse(mpz_t** matrix, int n, mpz_t m, mpz_t** matrixInv) {

	int i;
	int j;
	mpz_t det;
	mpz_t detInv;
	mpz_t** matrixTrans;
	mpz_t** matrixAdj;

	mpz_inits(det, detInv, NULL);

	determinante(matrix, n, det, m);

	if (getMultInverse(det, m, detInv) < 0) {
		return -1;
	}

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

    matrixTransposed(matrix, n, matrixTrans, m);

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

    mulMatrixConst(detInv, matrixAdj, matrixInv, n);

    for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			mpz_clear(matrixTrans[i][j]);
			mpz_clear(matrixAdj[i][j]);
		}
	}

	for (i = 0; i < n; i++) {
		free(matrixTrans[i]);
		free(matrixAdj[i]);
	}

	free(matrixTrans);
	free(matrixAdj);

	mpz_clears(det, detInv, NULL);
	return 0;
}





