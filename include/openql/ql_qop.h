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
#ifndef __QL_QOP_H
#define __QL_QOP_H

#include "ql_matrix.h"
#include "ql_qreg.h"

#define ql_qop_cnot ql_gate_CX
#define ql_qop_toffoli ql_gate_CCX
#define ql_qop_sigma_x ql_gate_X
#define ql_qop_pauli_x ql_gate_X
#define ql_qop_sigma_y ql_gate_Y
#define ql_qop_pauli_y ql_gate_Y
#define ql_qop_sigma_z ql_gate_Z
#define ql_qop_pauli_z ql_gate_Z
#define ql_qop_hadamard ql_gate_H

extern ql_qreg *ql_qop_CX(ql_qreg *, int, int);
extern ql_qreg *ql_qop_CCX(ql_qreg *, int, int, int);
extern ql_qreg *ql_qop_controlled_X(int, ql_qreg *, ...);

extern ql_qreg *ql_qop_X(ql_qreg *, int);
extern ql_qreg *ql_qop_Y(ql_qreg *, int);
extern ql_qreg *ql_qop_Z(ql_qreg *, int);

extern ql_qreg *ql_qop_SWAP(ql_qreg *, int);
extern ql_qreg *ql_qop_CSWAP(ql_qreg *, int, int);

extern ql_qreg *ql_qop_U1(ql_qreg *, int, ql_matrix);
extern ql_qreg *ql_qop_U2(ql_qreg *, int, int, ql_matrix);

extern ql_qreg *ql_qop_H(ql_qreg *, int);
extern ql_qreg *ql_qop_hadamards(ql_qreg *, int);

extern ql_qreg *ql_qop_rX(ql_qreg *, int, float);
extern ql_qreg *ql_qop_rY(ql_qreg *, int, float);
extern ql_qreg *ql_qop_rZ(ql_qreg *, int, float);

extern ql_qreg *ql_qop_P_scale(ql_qreg *, int, float);
extern ql_qreg *ql_qop_P_kick(ql_qreg *, int, float);

extern ql_qreg *ql_qop_CP(ql_qreg *, int, int);
extern ql_qreg *ql_qop_CP_inv(ql_qreg *, int, int);
extern ql_qreg *ql_qop_CP_kick(ql_qreg *, int, int, float);

extern int ql_qop_counter(int);

#endif /* __QL_QOP_H */
