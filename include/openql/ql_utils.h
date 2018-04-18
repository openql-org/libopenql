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
#ifndef __QL_UTILS_H
#define __QL_UTILS_H

#include <complex.h>
#include "ql_types.h"
#include "ql_qreg.h"

#define pi 4 * atan(1)
#define epsilon 1e-6
#define byte unsigned char
#define num_regs 4

extern double cprob (COMPLEX_FLOAT a);
extern COMPLEX_FLOAT euler_formula(REAL_FLOAT phi);

static inline
double cprob_inline(COMPLEX_FLOAT a) {
  REAL_FLOAT r, i;
  r = creal(a);
  i = cimag(a);
  return r * r + i * i;
}

#endif  /* __QL_UTILS_H */
