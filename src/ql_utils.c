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
#include <complex.h>

#include "openql/ql_matrix.h"
#include "openql/ql_measure.h"
#include "openql/ql_utils.h"
#include "openql/ql_qop.h"
#include "openql/ql_qreg.h"
#include "openql/ql_types.h"

COMPLEX_FLOAT euler_formula(REAL_FLOAT phi) {
  return cos(phi) + IMAGINARY * sin(phi);
}

double cprob (COMPLEX_FLOAT a){
  return cprob_inline(a);
}
