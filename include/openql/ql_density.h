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
#ifndef __QL_DENSITY_H
#define __QL_DENSITY_H

#include "ql_qreg.h"

#define ql_density_operation(function, rho, ...) \
do{ \
  int qint; \
  for(qint=0; qint < rho.num; qint++) \
    function(__VA_ARGS__, &rho.reg[qint]); \
} while(0)

struct ql_density_op_struct {
  int num;          /* total number of state vectors */
  float *prob;      /* probabilities of the state vectors */
  ql_qreg *reg; /* state vectors */
};
typedef struct ql_density_op_struct ql_density_op;

extern ql_density_op ql_density_new_op(ql_qreg *, int, float *);
extern ql_density_op ql_qreg2density_op(ql_qreg *);

extern ql_density_op *ql_density_reduced_op(ql_density_op *, int);

extern ql_matrix *ql_density_matrix(ql_density_op *);
extern void ql_density_delete_op(ql_density_op *);
extern float ql_purity(ql_density_op *);

extern void ql_density_print_matrix(ql_density_op *);

#endif /* __QL_DENSITY_H */
