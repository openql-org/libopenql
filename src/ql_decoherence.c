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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "openql/ql_measure.h"
#include "openql/ql_qreg.h"
#include "openql/ql_qop.h"
#include "openql/ql_utils.h"
#include "openql/ql_error.h"

static int quantum_status = 0;
static float quantum_lambda = 0;

float ql_get_decoherence() {
  return quantum_lambda;
}

void ql_set_decoherence(float l) {
  if(l) {
    quantum_status = 1;
    quantum_lambda = l;
  } else {
    quantum_status = 0;
  }
}

ql_qreg *ql_decohernce(ql_qreg *reg) {
  float u, v, s, x;
  float *nrands;
  float angle;
  int i, j;

  ql_qop_counter(1);

  if(quantum_status) {
    nrands = calloc(reg->width, sizeof(float));
    if(!nrands) {
      ql_error(QL_ENOMEM);
    }
    ql_matrix_memsize(reg->width * sizeof(float));

    for(i=0; i<reg->width; i++) {
     	do {
        u = 2 * ql_frand() - 1;
        v = 2 * ql_frand() - 1;
        s = u * u + v * v;
      } while (s >= 1);

      x = u * sqrt(-2 * log(s) / s);
      x *= sqrt(2 * quantum_lambda);
      nrands[i] = x/2;
    }

    for(i=0; i<reg->size; i++) {
      angle = 0;
      for(j=0; j<reg->width; j++) {
        if(reg->state[i] & ((MAX_UNSIGNED) 1 << j)) {
          angle += nrands[j];
        } else {
          angle -= nrands[j];
        }
      }
      reg->amplitude[i] *= euler_formula(angle);
    }
    free(nrands);
    ql_matrix_memsize(-reg->width * sizeof(float));  
  }
  return reg;
}
