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
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

#include "openql/ql_matrix.h"
#include "openql/ql_utils.h"
#include "openql/ql_qreg.h"
#include "openql/ql_decoherence.h"
#include "openql/ql_error.h"

ql_qreg *ql_qop_CX(ql_qreg *reg, int control, int target) {

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i<reg->size; i++) {
    if((reg->state[i] & ((MAX_UNSIGNED) 1 << control)))
      reg->state[i] ^= ((MAX_UNSIGNED) 1 << target);
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_CCX(ql_qreg *reg, int control1, int control2, int target) {

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i<reg->size; i++) {
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << control1)) {
      if(reg->state[i] & ((MAX_UNSIGNED) 1 << control2)) {
          reg->state[i] ^= ((MAX_UNSIGNED) 1 << target);
      }
    }
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_controlled_X(int controls, ql_qreg *reg, ...) {
  int *control_qbits;
  if(!(control_qbits = malloc(controls * sizeof(int)))){
      ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(controls * sizeof(int));

  va_list bits;
  va_start(bits, reg);
  for(int i = 0; i<controls; i++){
    control_qbits[i] = va_arg(bits, int);
  }
  int target = va_arg(bits, int);
  va_end(bits);

  int j;
#ifdef _OPENMP
#pragma omp parallel for private (j)
#endif
  for(int i = 0; i<reg->size; i++) {
    for(j = 0; (j < controls) && 
     (reg->state[i] & (MAX_UNSIGNED) 1 << control_qbits[j]); j++);

    if(j == controls) /* all control bits are set */
      reg->state[i] ^= ((MAX_UNSIGNED) 1 << target);
  }

  free(control_qbits);
  ql_matrix_memsize(-controls * sizeof(int));
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_X(ql_qreg *reg, int target) {

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i<reg->size; i++) {
    reg->state[i] ^= ((MAX_UNSIGNED) 1 << target);
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_Y(ql_qreg *reg, int target) {

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i<reg->size;i++) {
    reg->state[i] ^= ((MAX_UNSIGNED) 1 << target);
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << target)){
      reg->amplitude[i] *= IMAGINARY;
    }else
      reg->amplitude[i] *= -IMAGINARY;
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_Z(ql_qreg *reg, int target) {

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i<reg->size; i++) {
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << target))
      reg->amplitude[i] *= -1;
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_SWAP(ql_qreg *reg, int width) {
  int pat1 = 0, pat2 = 0;
  MAX_UNSIGNED l;
  for(int i = 0; i<reg->size; i++) {
    pat1 = reg->state[i] % ((MAX_UNSIGNED) 1 << width);
    pat2 = 0;
    for(int j = 0; j<width; j++){
      pat2 += reg->state[i] & ((MAX_UNSIGNED) 1 << (width + j));
    }
    l = reg->state[i] - (pat1 + pat2);
    l += (pat1 << width);
    l += (pat2 >> width);
    reg->state[i] = l;
  }
  return reg;
}

ql_qreg *ql_qop_CSWAP(ql_qreg *reg, int control, int width) {
  for(int i = 0; i<width; i++) {
    ql_qop_CCX(reg, control, width+i, 2*width+i+2);
    ql_qop_CCX(reg, control, 2*width+i+2, width+i);
    ql_qop_CCX(reg, control, width+i, 2*width+i+2);
  }
  return reg;
}

ql_qreg *ql_qop_U1(ql_qreg *reg, int target, ql_matrix *m) {
  if((m->cols != 2) || (m->rows != 2)){ ql_error(QL_EMSIZE);}

  int addsize=0;
  if(reg->hashw) {
    ql_qreg_reconstruct_hash(reg);

    for(int i = 0; i<reg->size; i++) {
      if(ql_qreg_get_state(reg, reg->state[i] ^ ((MAX_UNSIGNED) 1 << target)) == -1){
        addsize++;
      }
    }

    reg->state = realloc(reg->state, 
            (reg->size + addsize) * sizeof(MAX_UNSIGNED));
    reg->amplitude = realloc(reg->amplitude, 
            (reg->size + addsize) * sizeof(COMPLEX_FLOAT));

    if(reg->size && !(reg->state && reg->amplitude)){
      ql_error(QL_ENOMEM);
    }
    ql_matrix_memsize(addsize*(sizeof(COMPLEX_FLOAT) + sizeof(MAX_UNSIGNED)));

    for(int i = 0; i<addsize; i++) {
      reg->state[i+reg->size] = 0;
      reg->amplitude[i+reg->size] = 0;
    }
  }

  char *done = NULL;
  if(!(done = calloc(reg->size + addsize, sizeof(char)))){
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(reg->size + addsize * sizeof(char));

  float limit = (1.0 / ((MAX_UNSIGNED) 1 << reg->width)) * epsilon;
  int k = reg->size;
  for(int i = 0; i<reg->size; i++) {
    if(!done[i]) {
      int iset = reg->state[i] & ((MAX_UNSIGNED) 1 << target);
      int tnot = 0;

      int j = ql_qreg_get_state(reg, reg->state[i] ^ ((MAX_UNSIGNED) 1<<target));
      if(j >= 0){
        tnot = reg->amplitude[j];
      }
      COMPLEX_FLOAT t = reg->amplitude[i];

      if(iset){
        reg->amplitude[i] = m->t[2] * tnot + m->t[3] * t;
      } else{
        reg->amplitude[i] = m->t[0] * t + m->t[1] * tnot;
      }
      if(j >= 0) {
        if(iset){
          reg->amplitude[j] = m->t[0] * tnot + m->t[1] * t;
        } else {
          reg->amplitude[j] = m->t[2] * t + m->t[3] * tnot;
        }
      } else {
        if((m->t[1] == 0) && (iset)) break;
        if((m->t[2] == 0) && !(iset)) break; 

        reg->state[k] = reg->state[i] 
                      ^ ((MAX_UNSIGNED) 1 << target);
        if(iset){
          reg->amplitude[k] = m->t[1] * t;
        } else {
          reg->amplitude[k] = m->t[2] * t;
        }
        k++;
      }
      if(j >= 0){
        done[j] = 1;
      }
    }
  }
  reg->size += addsize;
  free(done);
  ql_matrix_memsize(-reg->size * sizeof(char));

  int decsize=0;
  if(reg->hashw) {
    for(int i = 0, j = 0; i<reg->size; i++) {
      if(cprob(reg->amplitude[i]) < limit) {
        j++;
        decsize++;
      } else if(j) {
        reg->state[i-j] = reg->state[i];
        reg->amplitude[i-j] = reg->amplitude[i];
      }
    }
    if(decsize) {
      reg->size -= decsize;
      reg->amplitude = realloc(reg->amplitude, 
                     reg->size * sizeof(COMPLEX_FLOAT));
      reg->state = realloc(reg->state, 
                     reg->size * sizeof(MAX_UNSIGNED));

      if(reg->size && !(reg->state && reg->amplitude)){
        ql_error(QL_ENOMEM);
      }
      ql_matrix_memsize(-decsize * (sizeof(MAX_UNSIGNED) 
                     + sizeof(COMPLEX_FLOAT)));
    }
  }

  if(reg->size > (1 << (reg->hashw-1))){
    fprintf(stderr, "Warning: inefficient hash table (size %i vs hash %i)\n", 
      reg->size, 1<<reg->hashw);
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_U2(ql_qreg *reg, int target1, int target2, ql_matrix *m) {
  if((m->cols != 4) || (m->rows != 4)){ ql_error(QL_EMSIZE); }
  for(int i = 0; i<(1 << reg->hashw); i++){
    reg->hash[i] = 0;
  } 
  for(int i = 0; i<reg->size; i++){
    ql_qreg_add_hash(reg, reg->state[i], i);
  }

  int addsize=0;
  for(int i = 0; i<reg->size; i++) {
    if(ql_qreg_get_state(reg, reg->state[i] ^ ((MAX_UNSIGNED) 1 << target1)) == -1){
      addsize++;
    }
    if(ql_qreg_get_state(reg, reg->state[i] ^ ((MAX_UNSIGNED) 1 << target2)) == -1){
      addsize++;
    }
  }
  reg->state = realloc(reg->state, 
                 (reg->size + addsize) * sizeof(MAX_UNSIGNED));
  reg->amplitude = realloc(reg->amplitude, 
                 (reg->size + addsize) * sizeof(COMPLEX_FLOAT));

  if(reg->size && !(reg->state && reg->amplitude)) {
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(addsize*(sizeof(COMPLEX_FLOAT) + sizeof(MAX_UNSIGNED)));

  for(int i = 0; i<addsize; i++) {
    reg->state[i+reg->size] = 0;
    reg->amplitude[i+reg->size] = 0;
  }

  char *done = NULL;
  if(!(done = calloc(reg->size + addsize, sizeof(char)))){
    ql_error(QL_EMSIZE);
  }
  ql_matrix_memsize(reg->size + addsize * sizeof(char));

  COMPLEX_FLOAT psi_sub[4];
  int base[4];
  int bits[2];

  bits[0] = target1;
  bits[1] = target2;

  float limit = (1.0 / ((MAX_UNSIGNED) 1 << reg->width)) / 1000000;
  int l = reg->size;
  for(int i = 0; i<reg->size; i++) {
    if(!done[i]) {
      int b = ql_bitmask(reg->state[i], 2, bits);
      base[b] = i;
      base[b ^ 1] = ql_qreg_get_state(reg, reg->state[i] 
                    ^ ((MAX_UNSIGNED) 1 << target2));
      base[b ^ 2] = ql_qreg_get_state(reg, reg->state[i]
                    ^ ((MAX_UNSIGNED) 1 << target1));
      base[b ^ 3] = ql_qreg_get_state(reg, reg->state[i]
                    ^ ((MAX_UNSIGNED) 1 << target1)
                    ^ ((MAX_UNSIGNED) 1 << target2));

      for(int j = 0; j<4; j++) {
        if(base[j] == -1) {
          base[j] = l;
  //      reg->node[l].state = reg->state[i]
          l++;
        }
        psi_sub[j] = reg->amplitude[base[j]];
      }

      for(int j = 0; j<4; j++) {
        reg->amplitude[base[j]] = 0;
        for(int k = 0; k<4; k++){
            reg->amplitude[base[j]] += M(m, k, j) * psi_sub[k];
        }
        done[base[j]] = 1;
      }

    }
  }
  reg->size += addsize;

  free(done);
  ql_matrix_memsize(-reg->size * sizeof(char));

  int decsize=0;
  for(int i = 0, j = 0; i<reg->size; i++) {
    if(cprob(reg->amplitude[i]) < limit) {
      j++;
      decsize++;
    } else if(j) {
      reg->state[i-j] = reg->state[i];
      reg->amplitude[i-j] = reg->amplitude[i];
    }
  }
  if(decsize) {
    reg->size -= decsize;
    reg->amplitude = realloc(reg->amplitude, 
                     reg->size * sizeof(COMPLEX_FLOAT));
    reg->state = realloc(reg->state, 
                     reg->size * sizeof(MAX_UNSIGNED));

    if(reg->size && !(reg->state && reg->amplitude)){
      ql_error(QL_ENOMEM);
    }
    ql_matrix_memsize(-decsize * (sizeof(MAX_UNSIGNED) 
                   + sizeof(COMPLEX_FLOAT)));
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_H(ql_qreg *reg, int target) {
  ql_matrix *m = ql_matrix_new(2, 2);

  m->t[0] = sqrt(1.0/2);  m->t[1] = sqrt(1.0/2);
  m->t[2] = sqrt(1.0/2);  m->t[3] = -sqrt(1.0/2);

  ql_qop_U1(reg, target, m);
  ql_matrix_delete(m);
  return reg;
}

ql_qreg *ql_qop_hadamards(ql_qreg *reg, int width) {
  for(int i=0; i<width; i++) {
    ql_qop_H(reg, i);
  }
  return reg;
}

ql_qreg *ql_qop_rX(ql_qreg *reg, int target, float gamma) {
  ql_matrix *m = ql_matrix_new(2, 2);

  m->t[0] = cos(gamma / 2);              m->t[1] = -IMAGINARY * sin(gamma / 2);
  m->t[2] = -IMAGINARY * sin(gamma / 2); m->t[3] = cos(gamma / 2);

  ql_qop_U1(reg, target, m);
  ql_matrix_delete(m);
  return reg;
}

ql_qreg *ql_qop_rY(ql_qreg *reg, int target, float gamma) {
  ql_matrix *m = ql_matrix_new(2, 2);

  m->t[0] = cos(gamma / 2);  m->t[1] = -sin(gamma / 2);
  m->t[2] = sin(gamma / 2);  m->t[3] = cos(gamma / 2);

  ql_qop_U1(reg, target, m);
  ql_matrix_delete(m);
  return reg;
}

ql_qreg *ql_qop_rZ(ql_qreg *reg, int target, float gamma) {
  COMPLEX_FLOAT z = euler_formula(gamma/2);

  for(int i = 0; i<reg->size; i++) {
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << target)) {
      reg->amplitude[i] *= z;
    } else {
      reg->amplitude[i] /= z;
    }
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_P_scale(ql_qreg *reg, int target, float gamma) {
  COMPLEX_FLOAT z = euler_formula(gamma);

#ifdef _OPENMP
  #pragma omp parallel for
#endif
  for(int i = 0; i<reg->size; i++) {
    reg->amplitude[i] *= z;
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_P_kick(ql_qreg *reg, int target, float gamma) {
  COMPLEX_FLOAT z = euler_formula(gamma);

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i<reg->size; i++) {
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << target)) {
      reg->amplitude[i] *= z;
    }
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_CP(ql_qreg *reg, int control, int target) {
  COMPLEX_FLOAT z = euler_formula(pi / ((MAX_UNSIGNED) 1 << (control - target)));

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i<reg->size; i++) {
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << control)) {
      if(reg->state[i] & ((MAX_UNSIGNED) 1 << target)) {
        reg->amplitude[i] *= z;
      }
    }
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_CP_inv(ql_qreg *reg, int control, int target) {
  COMPLEX_FLOAT z = euler_formula(-pi / ((MAX_UNSIGNED) 1 << (control - target)));

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i<reg->size; i++) {
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << control)) {
      if(reg->state[i] & ((MAX_UNSIGNED) 1 << target)) {
        reg->amplitude[i] *= z;
      }
    }
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_CP_kick(ql_qreg *reg, int control, int target, float gamma) {
  COMPLEX_FLOAT z = euler_formula(gamma);

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i<reg->size; i++) {
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << control)) {
      if(reg->state[i] & ((MAX_UNSIGNED) 1 << target))
        reg->amplitude[i] *= z;
    }
  }
  ql_decoherence(reg);
  return reg;
}

ql_qreg *ql_qop_CP_shift(ql_qreg *reg, int control, int target, float gamma) {
  COMPLEX_FLOAT z = euler_formula(gamma/2);

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i<reg->size; i++) {
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << control)) {
      if(reg->state[i] & ((MAX_UNSIGNED) 1 << target)){
        reg->amplitude[i] *= z;
      } else {
        reg->amplitude[i] /= z;
      }
    }
  }
  ql_decoherence(reg);
  return reg;
}

int ql_qop_counter(int inc) {
  static int counter = 0;
  if(inc > 0) {
    counter += inc;
  } else if(inc < 0) {
    counter = 0;
  }
  return counter;
}
