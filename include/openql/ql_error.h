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
#ifndef __QL_ERROR_H
#define __QL_ERROR_H

enum {
  QL_SUCCESS      = 0,
  QL_ENOMEM       = 2,
  QL_EMLARGE      = 3,
  QL_EMSIZE       = 4,
  QL_EHASHFULL    = 5,
  QL_EMCMATRIX    = 65536  /* internal errors start at 65536 */
};

extern void *ql_error_handler(void *f(int));
extern const char *ql_error_string(int errno);
extern void ql_error(int errno);

#endif /* __QL_ERROR_H */
