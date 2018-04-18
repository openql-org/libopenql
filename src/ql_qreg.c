/*
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "openql/ql_matrix.h"
#include "openql/ql_qreg.h"
#include "openql/ql_utils.h"
#include "openql/ql_error.h"

ql_matrix *ql_qreg2matrix(ql_qreg reg) {
  ql_matrix *m = ql_matrix_new(1, 1 << reg.width);
  for(int i=0; i<reg.size; i++){
    m->t[reg.state[i]] = reg.amplitude[i];
  }
  return m;
}

ql_qreg ql_matrix2qreg(ql_matrix *m, int width) {
  if(m->cols != 1){ ql_error(QL_EMCMATRIX); }
  ql_qreg reg;
  reg.width = width;
  int size=0;
  for(int i=0; i<m->rows; i++) {
    if(m->t[i]){ size++; }
  }
  reg.size = size;
  reg.hashw = width + 2;

  reg.amplitude = calloc(size, sizeof(COMPLEX_FLOAT));
  reg.state = calloc(size, sizeof(MAX_UNSIGNED));

  if(!(reg.state && reg.amplitude)){
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(size * (sizeof(COMPLEX_FLOAT) + sizeof(MAX_UNSIGNED)));

  reg.hash = calloc(1 << reg.hashw, sizeof(int));

  if(!reg.hash){
    ql_error(QL_ENOMEM);
  } 
  ql_matrix_memsize((1 << reg.hashw) * sizeof(int));

  for(int i=0, j=0; i<m->rows; i++) {
    if(m->t[i]) {
      reg.state[j] = i;
      reg.amplitude[j] = m->t[i];
      j++;
    }
  }
  return reg;
}

ql_qreg *ql_qreg_new(int width, MAX_UNSIGNED initval) {
  ql_qreg *reg = calloc(1, sizeof(ql_qreg));
  if(!reg){
    ql_error(QL_ENOMEM);
  }
  reg->width = width;
  reg->size = 1;
  reg->hashw = width + 2;

  reg->state = calloc(1, sizeof(MAX_UNSIGNED));
  reg->amplitude = calloc(1, sizeof(COMPLEX_FLOAT));
  if(!(reg->state && reg->amplitude)){
    ql_error(QL_ENOMEM);
  }

  ql_matrix_memsize(sizeof(MAX_UNSIGNED) + sizeof(COMPLEX_FLOAT));

  reg->hash = calloc(1 << reg->hashw, sizeof(int));
  if(!reg->hash){
    ql_error(QL_ENOMEM);
  }

  ql_matrix_memsize((1 << reg->hashw) * sizeof(int));

  reg->state[0] = initval;
  reg->amplitude[0] = 1;

  return reg;
}

ql_qreg *ql_qreg_alloc(int width, int siz) {
  ql_qreg *reg = calloc(1, sizeof(ql_qreg));
  if(!reg){
    ql_error(QL_ENOMEM);
  }
  reg->width = width;
  reg->size = siz;
  reg->hashw = 0;
  reg->hash = 0;

  reg->amplitude = calloc(siz, sizeof(COMPLEX_FLOAT));
  reg->state = 0;
  if(!reg->amplitude){
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(siz*sizeof(COMPLEX_FLOAT));

  return reg;
}

ql_qreg *ql_qreg_and_state_alloc(int width, int siz) {
  ql_qreg *reg = calloc(1, sizeof(ql_qreg));
  if(!reg){
    ql_error(QL_ENOMEM);
  }
  reg->width = width;
  reg->size = siz;
  reg->hashw = 0;
  reg->hash = 0;

  reg->amplitude = calloc(siz, sizeof(COMPLEX_FLOAT));
  reg->state = calloc(siz, sizeof(MAX_UNSIGNED));

  if(!(reg->amplitude && reg->state)){
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(siz*(sizeof(COMPLEX_FLOAT)+sizeof(MAX_UNSIGNED)));

  return reg;
}

void ql_qreg_destroy_hash(ql_qreg *reg) {
  free(reg->hash);
  ql_matrix_memsize(-(1 << reg->hashw) * sizeof(int));
  reg->hash = 0;
}

void ql_qreg_delete(ql_qreg *reg) {
  if(reg->hashw && reg->hash){
    ql_qreg_destroy_hash(reg);
  }
  free(reg->amplitude);
  ql_matrix_memsize(-reg->size * sizeof(COMPLEX_FLOAT));
  reg->amplitude = 0;

  if(reg->state) {
    free(reg->state);
    ql_matrix_memsize(-reg->size * sizeof(MAX_UNSIGNED));
    reg->state = 0;
  }
}

void ql_qreg_delete_hash(ql_qreg *reg) {
  free(reg->amplitude);
  ql_matrix_memsize(-reg->size * sizeof(COMPLEX_FLOAT));
  reg->amplitude = 0;

  if(reg->state) {
    free(reg->state);
    ql_matrix_memsize(-reg->size * sizeof(MAX_UNSIGNED));
    reg->state = 0;
  }
}

void ql_qreg_copy(ql_qreg *src, ql_qreg *dst) {
  *dst = *src;
  dst->amplitude = calloc(dst->size, sizeof(COMPLEX_FLOAT));

  if(!dst->amplitude){
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(dst->size*sizeof(COMPLEX_FLOAT));

  memcpy(dst->amplitude, src->amplitude, src->size*sizeof(COMPLEX_FLOAT));

  if(src->state) {
    dst->state = calloc(dst->size, sizeof(MAX_UNSIGNED));

    if(!dst->state){
      ql_error(QL_ENOMEM);
    }
    ql_matrix_memsize(dst->size*sizeof(MAX_UNSIGNED));
    memcpy(dst->state, src->state, src->size*sizeof(MAX_UNSIGNED));
  }
  if(dst->hashw) {
    dst->hash = calloc(1 << dst->hashw, sizeof(int));
      
    if(!dst->hash){
      ql_error(QL_ENOMEM);
    }
    ql_matrix_memsize((1 << dst->hashw) * sizeof(int));
  }
}

void ql_qreg_addscratch(ql_qreg *reg, int bits) {
  reg->width += bits;

  for(int i=0; i<reg->size; i++) {
    MAX_UNSIGNED l = reg->state[i] << bits;
    reg->state[i] = l;
  }
}

void ql_qreg_mvmult(ql_qreg *x, ql_qreg *y, ql_matrix *A) {
  for(int i=0; i < A->cols; i++) {
    y->amplitude[i] = 0;
    for(int j=0; j < A->cols; j++){
      y->amplitude[i] += M(A, j, i)*x->amplitude[j];
    }
  }
} 

void ql_qreg_scalar(ql_qreg *reg, COMPLEX_FLOAT r) {
  for(int i=0; i<reg->size; i++){
    reg->amplitude[i] *= r;
  }
}

void ql_qreg_normalize(ql_qreg *reg) {
  double r = 0;
  for(int i = 0; i<reg->size; i++){
    r += cprob(reg->amplitude[i]);
  }
  ql_qreg_scalar(reg, 1./sqrt(r));
}

ql_qreg *ql_qreg_kronecker(ql_qreg *reg1, ql_qreg *reg2) {
  int w = reg1->width+reg2->width;
  int s = reg1->size * reg2->size;
  ql_qreg *reg = ql_qreg_alloc(w,s);
  reg->width = reg1->width+reg2->width;
  reg->size = reg1->size*reg2->size;
  reg->hashw = reg->width + 2;
  reg->amplitude = calloc(reg->size, sizeof(COMPLEX_FLOAT));
  reg->state = calloc(reg->size, sizeof(MAX_UNSIGNED));

  if(!(reg->state && reg->amplitude)){
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(reg->size * (sizeof(COMPLEX_FLOAT) + sizeof(MAX_UNSIGNED)));

  if(!(reg->hash = calloc(1 << reg->hashw, sizeof(int)))){
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize((1 << reg->hashw) * sizeof(int));

  for(int i=0; i<reg1->size; i++){
    for(int j=0; j<reg2->size; j++) {
      /* printf("processing |%lli> x |%lli>\n", reg1->state[i], reg2->state[j]);
         printf("%lli\n", (reg1->state[i]) << reg2->width); */

      reg->state[i*reg2->size+j] = ((reg1->state[i]) << reg2->width) | reg2->state[j];
      reg->amplitude[i*reg2->size+j] = reg1->amplitude[i] * reg2->amplitude[j];
    }
  }
  return reg;
}

ql_qreg *ql_qreg_state_collapse(ql_qreg *reg, int pos, int value) {
  MAX_UNSIGNED pos2 = (MAX_UNSIGNED) 1 << pos;

  int size=0;
  double d=0;
  for(int i=0;i<reg->size;i++) {
    if(((reg->state[i] & pos2) && value) 
    || (!(reg->state[i] & pos2) && !value)) {
      d += cprob(reg->amplitude[i]);
      size++;
    }
  }

  ql_qreg *out = ql_qreg_alloc(reg->width-1,size);
  out->width = reg->width-1;
  out->size = size;
  out->amplitude = calloc(size, sizeof(COMPLEX_FLOAT));
  out->state = calloc(size, sizeof(MAX_UNSIGNED));

  if(!(out->state && out->amplitude)){
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(size * (sizeof(COMPLEX_FLOAT) + sizeof(MAX_UNSIGNED)));
  out->hashw = reg->hashw;
  out->hash = reg->hash;

  MAX_UNSIGNED lpat=0, rpat=0;
  for(int i=0, j=0; i<reg->size; i++) {
    if(((reg->state[i] & pos2) && value) 
       || (!(reg->state[i] & pos2) && !value)) {
      for(int k=0, rpat=0; k<pos; k++){
        rpat += (MAX_UNSIGNED) 1 << k;
      }
      rpat &= reg->state[i];

      for(int k=sizeof(MAX_UNSIGNED)*8-1, lpat=0; k>pos; k--){
        lpat += (MAX_UNSIGNED) 1 << k;
      }
      lpat &= reg->state[i];

      out->state[j] = (lpat >> 1) | rpat;
      out->amplitude[j] = reg->amplitude[i] * 1 / (float) sqrt(d);
	
      j++;
    }
  }
  return out;
}

COMPLEX_FLOAT ql_dot_product(ql_qreg *reg1, ql_qreg *reg2) {
  COMPLEX_FLOAT f = 0;

  if(reg2->hashw){
    ql_qreg_reconstruct_hash(reg2);
  }
  if(reg1->state) {
    for(int i=0; i<reg1->size; i++) {
      int j = ql_qreg_get_state(reg2, reg1->state[i]);
      if(j > -1) { /* state exists in reg2 */
        f += conj(reg1->amplitude[i]) * reg2->amplitude[j];
      }
    }
  } else {
    for(int i=0; i<reg1->size; i++) {
      int j = ql_qreg_get_state(reg2, i);
      if(j > -1) { /* state exists in reg2 */
        f += conj(reg1->amplitude[i]) * reg2->amplitude[j];
      }
    }
  }
  return f;
}

COMPLEX_FLOAT ql_dot_product_noconj(ql_qreg *reg1, ql_qreg *reg2) {
  if(reg2->hashw){
    ql_qreg_reconstruct_hash(reg2);
  }
  COMPLEX_FLOAT f = 0;
  if(!reg2->state) {
    for(int i=0; i<reg1->size; i++){
      f += reg1->amplitude[i] * reg2->amplitude[reg1->state[i]];
    }
  } else {
    for(int i=0; i<reg1->size; i++) {
      int j = ql_qreg_get_state(reg2, reg1->state[i]);
      if(j > -1) { /* state exists in reg2 */
        f += reg1->amplitude[i] * reg2->amplitude[j];
      }
    }
  }
  return f;
}

ql_qreg ql_vectoradd(ql_qreg *reg1, ql_qreg *reg2) {
  ql_qreg reg;
  ql_qreg_copy(reg1, &reg);

  int addsize = 0;
  if(reg1->hashw || reg2->hashw) {
    ql_qreg_reconstruct_hash(reg1);
    ql_qreg_copy(reg1, &reg);
      
    for(int i=0; i<reg2->size; i++) {
      if(ql_qreg_get_state(reg1, reg2->state[i]) == -1){
        addsize++;
      }
    }
  }
  if(addsize) {
    reg.size += addsize;

    reg.amplitude = realloc(reg.amplitude, reg.size*sizeof(COMPLEX_FLOAT));
    reg.state = realloc(reg.state, reg.size*sizeof(MAX_UNSIGNED));

    if(!(reg.state && reg.amplitude)){
      ql_error(QL_ENOMEM);
    }

    ql_matrix_memsize(addsize * (sizeof(COMPLEX_FLOAT) + sizeof(MAX_UNSIGNED)));
  }
  int k = reg1->size;
  if(!reg2->state) {
    for(int i=0; i<reg2->size; i++){
      reg.amplitude[i] += reg2->amplitude[i];
    }
  } else {
    for(int i=0; i<reg2->size; i++) {
      int j = ql_qreg_get_state(reg1, reg2->state[i]);
      if(j >= 0){
        reg.amplitude[j] += reg2->amplitude[i];
      } else {
	      reg.state[k] = reg2->state[i];
	      reg.amplitude[k] = reg2->amplitude[i];
	      k++;
	    }
    }
  }
  return reg;
}

void ql_vectoradd_inplace(ql_qreg *reg1, ql_qreg *reg2) {
  int i, j, k;
  int addsize = 0;

  if(reg1->hashw || reg2->hashw) {
    ql_qreg_reconstruct_hash(reg1);

    for(i=0; i<reg2->size; i++) {
      if(ql_qreg_get_state(reg1, reg2->state[i]) == -1){
        addsize++;
      }
    }
  }

  if(addsize) {
    reg1->amplitude = realloc(reg1->amplitude, 
                (reg1->size+addsize)*sizeof(COMPLEX_FLOAT));
    reg1->state = realloc(reg1->state, (reg1->size+addsize)
                *sizeof(MAX_UNSIGNED));

    if(!(reg1->state && reg1->amplitude)){
      ql_error(QL_ENOMEM);
    }
    ql_matrix_memsize(addsize * (sizeof(COMPLEX_FLOAT) + sizeof(MAX_UNSIGNED)));
  }

  k = reg1->size;
  if(!reg2->state) {
    for(i=0; i<reg2->size; i++){
      reg1->amplitude[i] += reg2->amplitude[i];
    }
  } else {
    for(i=0; i<reg2->size; i++) {
      j = ql_qreg_get_state(reg1, reg2->state[i]);
      if(j >= 0){
        reg1->amplitude[j] += reg2->amplitude[i];
      } else {
	      reg1->state[k] = reg2->state[i];
	      reg1->amplitude[k] = reg2->amplitude[i];
	      k++;
	    }
    }
    reg1->size += addsize;
  }
}

ql_qreg *ql_matrix_qreg(ql_qreg *A(MAX_UNSIGNED, double), double t, ql_qreg *reg, int flags) {
  ql_qreg *reg2 = ql_qreg_alloc(reg->width, reg->size);
  if(!reg2){
    ql_error(QL_ENOMEM);
  }
  reg2->hashw = 0;
  reg2->hash = 0;
  reg2->state = 0;

  if(!(reg2->amplitude = calloc(reg2->size, sizeof(COMPLEX_FLOAT)))) {
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(reg2->size * sizeof(COMPLEX_FLOAT));

  if(reg->state) {
    if(!(reg2->state = calloc(reg2->size, sizeof(MAX_UNSIGNED)))){
      ql_error(QL_ENOMEM);
    }
    ql_matrix_memsize(reg2->size * sizeof(MAX_UNSIGNED));
  }

  ql_qreg *tmp;
#ifdef _OPENMP
  #pragma omp parallel for private (*tmp)
#endif
  for(int i = 0; i<reg->size; i++) {
    if(reg2->state){
      reg2->state[i] = i;
    }
    tmp = A(i, t);
    reg2->amplitude[i] = ql_dot_product_noconj(tmp, reg);
    if(!(flags & 1)){
      ql_qreg_delete(tmp);
    }
  }
  return reg2;
}

void ql_qreg_print(ql_qreg *reg) {
  for(int i=0; i<reg->size; i++) {
    printf("% f %+fi|%lli> (%e) (|", creal(reg->amplitude[i]),
             cimag(reg->amplitude[i]), reg->state[i], 
             cprob(reg->amplitude[i]));
    for(int j=reg->width-1;j>=0;j--) {
      if(j % 4 == 3){
        printf(" ");
      }
      printf("%i", ((((MAX_UNSIGNED) 1 << j) & reg->state[i]) > 0));
    }
    printf(">)\n");
  }
  printf("\n");
}

void ql_qreg_print_expn(ql_qreg *reg) {
  for(int i=0; i<reg->size; i++) {
    printf("%i: %lli\n", i, reg->state[i] - i * (1 << (reg->width / 2)));
  }
}

void ql_qreg_print_hash(ql_qreg *reg) {
  for(int i=0; i < (1 << reg->hashw); i++) {
    if(i){
      printf("%i: %i %llu\n", i, reg->hash[i]-1, reg->state[reg->hash[i]-1]);
    }
  }
}

void ql_qreg_print_timeop(int width, void f(ql_qreg *)) {
  ql_matrix *m = ql_matrix_new(1 << width, 1 << width);

  for(int i=0;i<(1 << width); i++) {
    ql_qreg *tmp = ql_qreg_new(width, i);
    f(tmp);
    for(int j=0; j<tmp->size; j++){
      M(m, tmp->state[j], i) = tmp->amplitude[j];
    }
    ql_qreg_delete(tmp);
  }
  ql_matrix_print(m);
  ql_matrix_delete(m);
}

