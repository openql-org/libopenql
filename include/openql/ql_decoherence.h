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
/**
 * @file ql_decoherence.h
 * @brief The header of ql_decoherence
 */
#ifndef __QL_DECOHERENCE_H
#define __QL_DECOHERENCE_H

#include "ql_qreg.h"

extern float ql_get_decoherence();
extern void ql_set_decoherence(float);
extern ql_qreg *ql_decoherence(ql_qreg *);

#endif  /* __QL_DECOHERENCE_H */
