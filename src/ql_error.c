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

#include "openql/ql_error.h"

const char *ql_error_string(int errno) {
  switch(errno) {
  case QL_SUCCESS:      return "success";
  case QL_ENOMEM:       return "malloc failed";
  case QL_EMLARGE:      return "matrix too large";
  case QL_EMSIZE:       return "wrong matrix size";
  case QL_EMCMATRIX:    return "single-column matrix expected";
  case QL_EHASHFULL:    return "hash table full";
  default:              return "unknown error code";
  }
}

void *ql_error_handler(void *f(int)) {
  static void *errfunc = NULL;
  if(f){
    errfunc = f;
  }
  return errfunc;
}

void ql_error(int errno) {
  void (*p)(int) = ql_error_handler(0);
  if(p != NULL){
    p(errno);
  } else {
    fflush(stdout);
    fprintf(stderr, "ERROR: %s\n", ql_error_string(errno));
    fflush(stderr);
    abort();
  }
} 
