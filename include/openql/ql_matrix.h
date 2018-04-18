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
#ifndef __QL_MATRIX_H
#define __QL_MATRIX_H

#include "ql_types.h"

/* A ROWS x COLS matrix with complex elements */

struct ql_matrix_struct {
  int rows;
  int cols;
  COMPLEX_FLOAT *t;
};

typedef struct ql_matrix_struct ql_matrix;

#define M(m,_x,_y) m->t[(_x)+(_y)*m->cols]

extern unsigned long ql_matrix_memsize(long);

extern ql_matrix *ql_matrix_new(int /*cols*/, int /*rows*/);
extern void ql_matrix_delete(ql_matrix *);
extern void ql_matrix_print(ql_matrix *);

extern ql_matrix *ql_matrix_mmult(ql_matrix *, ql_matrix *);
extern ql_matrix *ql_matrix_adjoint(ql_matrix *);

#endif /* __QL_MATRIX_H */
