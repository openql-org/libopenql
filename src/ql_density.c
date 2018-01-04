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

#include "openql/ql_density.h"
#include "openql/ql_qreg.h"
#include "openql/ql_matrix.h"
#include "openql/ql_utils.h"
#include "openql/ql_error.h"

ql_density_op ql_density_new_op(ql_qreg *reg, int num, float *prob) {
  ql_density_op rho;
  rho.num = num;
  if(!(rho.prob = calloc(num, sizeof(float)))){
    ql_error(QL_ENOMEM);
  }
  if(!(rho.reg = calloc(num, sizeof(ql_qreg)))){
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(num * (sizeof(float) + sizeof(ql_qreg)));

  rho.prob[0] = prob[0];
  int *phash = reg[0].hash;
  int hashw = reg[0].hashw;
  rho.reg[0] = reg[0];

  reg[0].size = 0;
  reg[0].width = 0;
  reg[0].state = 0;
  reg[0].amplitude = 0;
  reg[0].hash = 0;

  for(int i=1; i<num; i++) {
    rho.prob[i] = prob[i];
    rho.reg[i] = reg[i];
    rho.reg[i].hash = phash;
    rho.reg[i].hashw = hashw;

    reg[i].size = 0;
    reg[i].width = 0;
    reg[i].state = 0;
    reg[i].amplitude = 0;
    reg[i].hash = 0;
  }
  return rho;
}
    
ql_density_op ql_qreg2density_op(ql_qreg *reg) {
  float f = 1;
  return ql_density_new_op(reg, 1, &f);
}

ql_density_op *ql_density_reduced_op(ql_density_op *rho, int pos) {
  if(!(rho->prob = realloc(rho->prob, 2*rho->num*sizeof(float)))){
    ql_error(QL_ENOMEM);
  }
  if(!(rho->reg = realloc(rho->reg, 2*rho->num*sizeof(ql_qreg)))){
    ql_error(QL_ENOMEM);
  }
  ql_matrix_memsize(rho->num * (sizeof(float) + sizeof(ql_qreg)));

  MAX_UNSIGNED pos2 = (MAX_UNSIGNED) 1 << pos;
  for(int i=0; i<rho->num; i++) {
    double ptmp = rho->prob[i];
    ql_qreg rtmp = rho->reg[i];
    double p0 = 0;
  
    for(int j=0; j<rho->reg[i].size; j++) {
      if(!(rho->reg[i].state[j] & pos2)){
        p0 += cprob(rho->reg[i].amplitude[j]);
      }
    }

    rho->prob[i] = ptmp * p0;
    rho->prob[rho->num + i] = ptmp * (1-p0);
    rho->reg[i] = *ql_state_collapse(&rtmp, pos, 0);
    rho->reg[rho->num + i] = *ql_state_collapse(&rtmp, pos, 1);

    ql_qreg_delete_hash(&rtmp); 
  }
  rho->num *= 2;
  return rho;
}

ql_matrix *ql_density_matrix(ql_density_op *rho) {
  int dim = 1 << rho->reg[0].width;
  if(dim < 0){
    ql_error(QL_EMLARGE);
  }

  ql_matrix *m = ql_matrix_new(dim, dim);
  for(int k=0; k<rho->num; k++) {
    ql_qreg_reconstruct_hash(&rho->reg[k]);

    for(int i=0; i<dim; i++) {
      for(int j=0; j<dim; j++) {
	      int l1 = ql_qreg_get_state(&(rho->reg[k]), i);
	      int l2 = ql_qreg_get_state(&(rho->reg[k]), j);
	      if((l1 > -1) && (l2 > -1)){
          M(m, i, j) += rho->prob[k] * rho->reg[k].amplitude[l2]
                        *conj(rho->reg[k].amplitude[l1]);
        }
      }
    }
  }
  return m;
}

void ql_density_print_matrix(ql_density_op *rho) {
  ql_matrix *m = ql_density_matrix(rho);
  ql_matrix_print(m);
  ql_matrix_delete(m);
}

void ql_density_delete_op(ql_density_op *rho) {
  ql_qreg_destroy_hash(&rho->reg[0]);

  for(int i = 0; i<rho->num; i++) {
    ql_qreg_delete_hash(&rho->reg[i]);
  }
  free(rho->prob);
  free(rho->reg);
  ql_matrix_memsize(-rho->num * (sizeof(float) + sizeof(ql_qreg)));

  rho->prob = 0;
  rho->reg = 0;
}

float ql_purity(ql_density_op *rho) {
  float f = 0;
  for(int i = 0; i<rho->num; i++){
    f += rho->prob[i]*rho->prob[i];
  }
  COMPLEX_FLOAT g, dp;
  for(int i = 0; i<rho->num; i++) {
    for(int j = 0; j<i; j++) {
      dp = ql_dot_product(&rho->reg[i], &rho->reg[j]);

      for(int k = 0; k<rho->reg[i].size; k++) {
	      int l = ql_qreg_get_state(&(rho->reg[j]), rho->reg[i].state[k]);

	      /* Compute p_i p_j <k|\psi_iX\psi_i|\psi_jX\psi_j|k> */
	      if(l > -1) {
          g = rho->prob[i] * rho->prob[j] * dp 
            * rho->reg[i].amplitude[k]
            * conj(rho->reg[j].amplitude[l]);
        } else {
          g = 0;
        }
	      f += 2 * creal(g);
	    }
    }
  }
  return f;
}
