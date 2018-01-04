/**
   Copyright 2018 OpenQL Project developers.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include <stdlib.h>
#include <stdio.h>

#include "openql/ql_matrix.h"
#include "openql/ql_utils.h"
#include "openql/ql_error.h"

unsigned long ql_matrix_memsize(long change) {
  static long mem = 0, max = 0;
  mem += change;
  if(mem > max){
    max = mem;
  }
  return mem;
}

ql_matrix *ql_matrix_new(int cols, int rows) {
  ql_matrix *m = calloc(1, sizeof(ql_matrix));
  if(!m){ ql_error(QL_ENOMEM); }
  m->rows = rows;
  m->cols = cols;
  m->t = calloc(cols * rows, sizeof(COMPLEX_FLOAT));
#if (DEBUG_MEM)
  printf("allocating %i bytes of memory for %ix%i matrix at 0x%X\n",
             sizeof(COMPLEX_FLOAT) * cols * rows, cols, rows, (int) m.t);
#endif  
  if(!m->t){ ql_error(QL_ENOMEM); }
  ql_matrix_memsize(sizeof(COMPLEX_FLOAT) * cols * rows);
  return m;
}

void ql_matrix_delete(ql_matrix *m) {
#if (DEBUG_MEM)	
  printf("freeing %i bytes of memory for %ix%i matrix at 0x%X\n",
	 sizeof(COMPLEX_FLOAT) * m->cols * m->rows, m->cols, m->rows,
	 (int) m->t);	
#endif  
  free(m->t);
  ql_matrix_memsize(-sizeof(COMPLEX_FLOAT) * m->cols * m->rows);
  m->t=0;
  free(m);
}

void ql_matrix_print(ql_matrix *m) {
  int print_imag = 0;
  for(int i = 0; i<m->rows; i++) {
    for(int j = 0; j<m->cols; j++) {
      if(cimag(M(m, j, i))/creal(M(m, j, i)) > 1e-3){
        print_imag = 1;
      }
	  }
  }

  int z = 0;
  while ((1 << z++) < m->rows);
  z--;

  for(int i = 0; i < m->rows; i++) {
    for(int j = 0; j < m->cols; j++) {
      if(print_imag){
        printf("%3.3f%+.3fi ", creal(M(m, j, i)), cimag(M(m, j, i)));
      } else {
        printf("%+.1f ", creal(M(m, j, i)));
      }
    }
    printf("\n");
  }
  printf("\n");
}

ql_matrix *ql_matrix_mmult(ql_matrix *A, ql_matrix *B) {
  if(A->cols != B->rows){ ql_error(QL_EMSIZE); } 
  ql_matrix *C = ql_matrix_new(B->cols, A->rows);
  for(int i = 0; i < B->cols; i++) {
    for(int j = 0; j < A->rows; j++) {
      for(int k = 0; k < B->rows; k++){
        M(C, i, j) += M(A, k, j) * M(B, i, k);
      }
    }
  }
  return C;
}

ql_matrix *ql_matrix_adjoint(ql_matrix *m) {
  COMPLEX_FLOAT tmp;
  ql_matrix *A = m;
  for(int i = 0; i < m->cols; i++) {
    for(int j = 0; j < i; j++) {
      tmp = M(A, i, j);
      M(A, i, j) = conj(M(A, j, i));
      M(A, j, i) = conj(tmp);
    }
  }
  return m;
}


