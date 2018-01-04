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
#include "openql/ql_qreg.h"
#include "openql/ql_qop.h"

void ql_qft(ql_qreg *reg, int width) {
  for(int i=width-1; i>=0; i--) {
    for(int j=width-1; j>i; j--){
      ql_qop_CP(reg, j, i);
    }
    ql_qop_H(reg, i);
  }
}

void ql_qft_inv(ql_qreg *reg, int width) {
  for(int i=0; i<width; i++) {
    ql_qop_H(reg, i);
    for(int j=i+1; j<width; j++){
      ql_qop_CP_inv(reg, j, i);
    }
  }
}
