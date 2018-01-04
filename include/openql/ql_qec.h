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
#ifndef __QL_QEC_H
#define __QL_QEC_H

#include "ql_qreg.h"

extern void ql_qec_set_status(int, int);
extern void ql_qec_get_status(int *, int *);

extern ql_qreg *ql_qec_encode(ql_qreg *, int, int);
extern ql_qreg *ql_qec_decode(ql_qreg *, int, int);

extern ql_qreg *ql_qec_qop_X(ql_qreg *, int);
extern ql_qreg *ql_qec_qop_CX(ql_qreg *, int, int);
extern ql_qreg *ql_qec_qop_CCX(ql_qreg *, int, int, int);

#endif /* __QL_QEC_H */
