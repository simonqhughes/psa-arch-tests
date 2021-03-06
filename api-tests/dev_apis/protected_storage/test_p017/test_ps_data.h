/** @file
 * Copyright (c) 2019, Arm Limited or ps affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/
#ifndef _TEST_P017_PS_DATA_TESTS_H_
#define _TEST_P017_PS_DATA_TESTS_H_

#include "val_protected_storage.h"

#define SST_FUNCTION val->ps_function

typedef struct {
    enum ps_function_code  api;
    psa_status_t        status;
} test_data;

static const test_data p017_data[] = {
{
 VAL_PS_GET_SUPPORT, PSA_STORAGE_SUPPORT_SET_EXTENDED /* Check if Optional API supported */
},
{
 VAL_PS_CREATE, PSA_SUCCESS /* Storage creation */
},
{
 VAL_PS_SET_EXTENDED, PSA_SUCCESS /* Write data in created storage  */
},
{
 VAL_PS_SET_EXTENDED, PSA_ERROR_STORAGE_FAILURE /* Try to set data in noncontinous manner */
},
{
 VAL_PS_SET_EXTENDED, PSA_SUCCESS /* Overwrite the existing data */
},
{
 VAL_PS_GET_INFO, PSA_SUCCESS /* Check for valid storage attributes */
},
{
 0, 0 /* Unused index */
},
{
 0, 0 /* Unused index */
},
{
 VAL_PS_REMOVE, PSA_SUCCESS /* Remove the UID */
},
};
#endif /* _TEST_P017_PS_DATA_TESTS_H_ */
