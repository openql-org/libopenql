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
#ifndef __QL_QREG_H
#define __QL_QREG_H

#include <sys/types.h>

#include "ql_types.h"
#include "ql_matrix.h"
#include "ql_error.h"

struct ql_qreg_struct {
  int width;    /* number of qubits in the ql_qreg */
  int size;     /* number of non-zero vectors */
  int hashw;    /* width of the hash array */
  COMPLEX_FLOAT *amplitude;
  MAX_UNSIGNED *state;
  int *hash;
};

typedef struct ql_qreg_struct ql_qreg;

extern ql_matrix *ql_qreg2matrix(ql_qreg);
extern ql_qreg ql_matrix2qreg(ql_matrix *, int);

extern ql_qreg *ql_qreg_new(int, MAX_UNSIGNED);
extern ql_qreg *ql_qreg_alloc(int, int);
extern ql_qreg *ql_qreg_and_state_alloc(int, int);

extern void ql_qreg_destroy_hash(ql_qreg *);
extern void ql_qreg_delete(ql_qreg *);
extern void ql_qreg_delete_hash(ql_qreg *);
extern void ql_qreg_copy(ql_qreg *, ql_qreg *);

extern void ql_addscratch(ql_qreg *, int);
extern void ql_qreg_mvmult(ql_qreg *, ql_qreg *, ql_matrix *);
extern void ql_qreg_scalar(ql_qreg *, COMPLEX_FLOAT);
extern void ql_qreg_normalize(ql_qreg *);

extern ql_qreg *ql_kronecker(ql_qreg *, ql_qreg *);
extern ql_qreg *ql_state_collapse(ql_qreg *, int, int);
extern COMPLEX_FLOAT ql_dot_product(ql_qreg *, ql_qreg *);
extern COMPLEX_FLOAT ql_dot_product_noconj(ql_qreg *, ql_qreg *);

extern ql_qreg ql_vectoradd(ql_qreg *, ql_qreg *);
extern void ql_vectoradd_inplace(ql_qreg *, ql_qreg *);

extern ql_qreg *ql_qreg_matrix(ql_qreg *A(MAX_UNSIGNED, double), double, ql_qreg *, int);

extern void ql_qreg_print(ql_qreg *);
extern void ql_qreg_print_expn(ql_qreg *);
extern void ql_qreg_print_hash(ql_qreg *);
extern void ql_qreg_print_timeop(int, void f(ql_qreg *));

static inline
unsigned int ql_qreg_hash64(MAX_UNSIGNED key, int width) {
  unsigned int k32;
  k32 = (key & 0xFFFFFFFF) ^ (key >> 32);
  k32 *= 0x9e370001UL;
  k32 = k32 >> (32-width);
  return k32;
}

static inline 
int ql_qreg_get_state(ql_qreg *reg, MAX_UNSIGNED a) {
  if(!reg->hashw){ return a; }
  int i = ql_qreg_hash64(a, reg->hashw);

  while(reg->hash[i]) {
    if(reg->state[reg->hash[i]-1] == a){
      return reg->hash[i]-1;
    }
    i++;
    if(i == (1 << reg->hashw)){ i = 0; }
  }
  return -1;
}

static inline
void ql_qreg_add_hash(ql_qreg *reg, MAX_UNSIGNED a, int pos) {
  int mark = 0;
  int i = ql_qreg_hash64(a, reg->hashw);
  while(reg->hash[i]) {
    i++;
    if(i == (1 << reg->hashw)) {
      if(!mark) {
	      i = 0;
	      mark = 1;
	    } else {
        ql_error(QL_EHASHFULL);
      }
    }
  }
  reg->hash[i] = pos+1;
}

static inline
void ql_qreg_reconstruct_hash(ql_qreg *reg) {
  if(!reg->hashw){ return; }
  for(int i = 0; i<(1 << reg->hashw); i++){
    reg->hash[i] = 0;
  }
  for(int i = 0; i<reg->size; i++){
    ql_qreg_add_hash(reg, reg->state[i], i);
  }
}
      
static inline
int ql_bitmask(MAX_UNSIGNED a, int width, int *bits) {
  int i;
  int mask = 0;

  for(i=0; i<width; i++) {
    if(a & ((MAX_UNSIGNED) 1 << bits[i])){
      mask += 1 << i;
    }
  }
  return mask;
}
  
#endif /* __QL_QREG_H */
