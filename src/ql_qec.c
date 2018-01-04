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

#include "openql/ql_qreg.h"
#include "openql/ql_qop.h"
#include "openql/ql_decoherence.h"
#include "openql/ql_measure.h"

static int qec_type = 0;
static int qec_width = 0;

void ql_qec_set_status(int stype, int swidth) {
  qec_type = stype;
  qec_width = swidth;
}

void ql_qec_get_status(int *ptype, int *pwidth) {
  if(ptype){ *ptype = qec_type; }
  if(pwidth){ *pwidth = qec_width; }
} 

ql_qreg *ql_qec_encode(ql_qreg *reg, int type, int width) {
  float lambda = ql_get_decoherence();
  ql_set_decoherence(0);

  for(int i = 0;i<reg->width; i++) {
    if(i==reg->width-1){
      ql_set_decoherence(lambda);
    }

    if(i<width) {
      ql_qop_H(reg, reg->width+i);
      ql_qop_H(reg, 2*reg->width+i);

      ql_qop_CX(reg, reg->width+i, i);
      ql_qop_CX(reg, 2*reg->width+i, i);
    } else {
      ql_qop_CX(reg, i, reg->width+i);
      ql_qop_CX(reg, i, 2*reg->width+i);
    }
  }
  ql_qec_set_status(1, reg->width);
  reg->width *= 3;
  return reg;
}

ql_qreg *ql_qec_decode(ql_qreg *reg, int type, int width) {
  int i, a, b;
  int swidth;
  float lambda = ql_get_decoherence();
  ql_set_decoherence(0);

  swidth=reg->width/3;

  ql_qec_set_status(0, 0);

  for(i=reg->width/3-1;i>=0;i--) {
    if(i==0){
      ql_set_decoherence(lambda);
    }
    if(i<width) {
      ql_qop_CX(reg, 2*swidth+i, i);
      ql_qop_CX(reg, swidth+i, i);
	  
      ql_qop_H(reg, 2*swidth+i);
      ql_qop_H(reg, swidth+i);
    } else {
      ql_qop_CX(reg, i, 2*swidth+i);
      ql_qop_CX(reg, i, swidth+i);
    }
  }
  for(i=1;i<=swidth;i++) {
    a = ql_bmeasure(reg, swidth);
    b = ql_bmeasure(reg, 2*swidth-i);
    if(a == 1 && b == 1 && i-1 < width){
      ql_qop_Z(reg, i-1);       /* Z = HXH */
    }
  }
  return reg;
}

int ql_qec_counter(ql_qreg *reg, int inc, int frequency) {
  static int counter = 0;
  static int freq = (1<<30);

  if(inc > 0){
    counter += inc;
  } else if(inc < 0) {
    counter = 0;
  }

  if(frequency > 0){
    freq = frequency;
  }
  if(counter >= freq) {
    counter = 0;
    ql_qec_decode(reg, qec_type, qec_width);
    ql_qec_encode(reg, qec_type, qec_width);
  }
  return counter;
}

ql_qreg *ql_qec_qop_X(ql_qreg *reg, int target) {
  int tmp;

  tmp = qec_type;
  qec_type = 0;

  float lambda = ql_get_decoherence();
  ql_set_decoherence(0);

  ql_qop_X(reg, target);
  ql_qop_X(reg, target+qec_width);
  ql_set_decoherence(lambda);
  ql_qop_X(reg, target+2*qec_width);

  ql_qec_counter(reg, 1, 0);

  qec_type = tmp;
  return reg;
}

ql_qreg *ql_qec_qop_CX(ql_qreg *reg, int control, int target) {
  int tmp = qec_type;
  qec_type = 0;

  float lambda = ql_get_decoherence();
  ql_set_decoherence(0);

  ql_qop_CX(reg, control, target);
  ql_qop_CX(reg, control+qec_width, target+qec_width);
  ql_set_decoherence(lambda);
  ql_qop_CX(reg, control+2*qec_width, target+2*qec_width);

  ql_qec_counter(reg, 1, 0);

  qec_type = tmp;
  return reg;
}

ql_qreg *ql_qec_qop_CCX(ql_qreg *reg, int control1, int control2, int target) {
  MAX_UNSIGNED mask = ((MAX_UNSIGNED) 1 << target)
                    + ((MAX_UNSIGNED) 1 << (target+qec_width))
                    + ((MAX_UNSIGNED) 1 << (target+2*qec_width));

  for(int i = 0;i<reg->size; i++) {
    int c1 = 0, c2 = 0;

    if(reg->state[i] & ((MAX_UNSIGNED) 1 << control1)){ c1 = 1; }
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << (control1+qec_width))) { c1 ^= 1; }
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << (control1+2*qec_width))) { c1 ^= 1; }

    if(reg->state[i] & ((MAX_UNSIGNED) 1 << control2)){ c2 = 1; }
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << (control2+qec_width))) { c2 ^= 1; }
    if(reg->state[i] & ((MAX_UNSIGNED) 1 << (control2+2*qec_width))) { c2 ^= 1; }

    if(c1 == 1 && c2 == 1){
      reg->state[i] = reg->state[i] ^ mask;
    }
  }
  ql_decoherence(reg);
  ql_qec_counter(reg, 1, 0);
  return reg;
}
