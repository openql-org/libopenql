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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>

#include "openql/ql_qreg.h"
#include "openql/ql_utils.h"
#include "openql/ql_error.h"

double ql_frand() {
  return (double) rand() / RAND_MAX;
}

MAX_UNSIGNED ql_measure(ql_qreg reg) {
  double r = ql_frand();
  for (int i = 0; i<reg.size; i++) {
    r -= cprob(reg.amplitude[i]);
    if(0 >= r) { return reg.state[i]; }
  }
  return -1;
}

int ql_bmeasure(ql_qreg *reg, int pos) {
  MAX_UNSIGNED pos2 = (MAX_UNSIGNED) 1 << pos;

  double pa=0;
  for(int i = 0; i<reg->size; i++) {
    if(!(reg->state[i] & pos2)){
      pa += cprob(reg->amplitude[i]);
    }
  }
  double r = ql_frand();
  int result=0;
  if (r > pa){
    result = 1;
  }
  ql_qreg *out = ql_state_collapse(reg, pos, result);
  ql_qreg_delete_hash(reg);
  reg = out;
  return result;
}

int ql_bmeasure_bit(ql_qreg *reg, int pos) {
  MAX_UNSIGNED pos2 = (MAX_UNSIGNED) 1 << pos;

  int size=0;
  double pa=0;
  for(int i = 0; i<reg->size; i++) {
    if(!(reg->state[i] & pos2)){
      pa += cprob(reg->amplitude[i]);
    }
  }

  double d = 0;
  double r = ql_frand();
  int result=0;
  if (r > pa){
    result = 1;
  }

  for(int i = 0;i<reg->size; i++) {
    if(reg->state[i] & pos2) {
      if(!result){
        reg->amplitude[i] = 0;
      } else {
	      d += cprob(reg->amplitude[i]);
	      size++;
	    }
    } else {
      if(result){
        reg->amplitude[i] = 0;
      } else {
	      d += cprob(reg->amplitude[i]);
	      size++;
	    }
    }
  }

  ql_qreg out;
  out.size = size;
  out.state = calloc(size, sizeof(MAX_UNSIGNED));
  out.amplitude = calloc(size, sizeof(COMPLEX_FLOAT));

  if(!(out.state && out.amplitude)){
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(size * (sizeof(MAX_UNSIGNED) + sizeof(COMPLEX_FLOAT)));

  out.hashw = reg->hashw;
  out.hash = reg->hash;
  out.width = reg->width;

  for(int i = 0, j = 0; i<reg->size; i++) {
    if(reg->amplitude[i]) {
      out.state[j] = reg->state[i];
      out.amplitude[j] = reg->amplitude[i] * 1 / (float) sqrt(d);
      j++;
    }
  }
  ql_qreg_delete_hash(reg);
  *reg = out;
  return result;
}
