/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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

#include "val_interfaces.h"
#include "val_target.h"
#include "test_s006.h"
#ifdef ITS_TEST
#include "test_its_data.h"
#elif PS_TEST
#include "test_ps_data.h"
#endif

#define TEST_BUFF_SIZE 30

client_test_t test_s006_sst_list[] = {
    NULL,
    psa_sst_flags_not_supported,
    NULL,
};

static uint8_t write_buff[TEST_BUFF_SIZE] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x23, 0xF6, 0x07, 0x08, 0x0D, 0x0A, 0x1B, 0x0C, 0x5D, 0x0E,\
  0x70, 0xA1, 0xFF, 0xFF, 0x14, 0x73, 0x46, 0x97, 0xE8, 0xDD, 0xCA, 0x0B, 0x3C, 0x0D, 0x2E};

static int32_t psa_sst_remove_api(psa_storage_uid_t uid, uint32_t data_len,
                                   uint8_t *data_buff, psa_storage_create_flags_t create_flag)
{
    uint32_t status;

    /* Call the get_info function and match the attributes */
    status = SST_FUNCTION(s006_data[2].api, uid, &info);
    TEST_ASSERT_EQUAL(status, s006_data[2].status, TEST_CHECKPOINT_NUM(2));
    printf("create_flag=0x%x, info.flags=0x%x\n", create_flag, info.flags);
    TEST_ASSERT_EQUAL(info.flags, create_flag, TEST_CHECKPOINT_NUM(3));

    /* Remove the UID  */
    status = SST_FUNCTION(s006_data[4].api, uid);
    TEST_ASSERT_EQUAL(status, s006_data[4].status, TEST_CHECKPOINT_NUM(4));

    return VAL_STATUS_SUCCESS;
}

int32_t psa_sst_flags_not_supported(caller_security_t caller)
{
   psa_storage_create_flags_t flag = 0x80000000;
   uint32_t status = VAL_STATUS_SUCCESS;
   psa_storage_uid_t uid = UID_BASE_VALUE + 5;
   int32_t test_status;

   /* Calling set function with different create flag value */

   val->print(PRINT_TEST, "[Check 1] Call set API with valid flag values\n", 0);

   /* The test is incorrect in that it sets a flag bit when an object
    * is created, and expects the bit always to be set in the flags retrieved by
    * psa_xx_get_info(). The specification defines this is not always the case.
    *
    * The following outlines psuedo-code for this test:
    *
    *  set create_flags = 0x800000000
    *  while (create_flags != 0)
    *    {
    *      psa_ps_set()
    *      psa_ps_get()
    *      // check xxx
    *      psa_ps_get_info()
    *      // check flag retrieved is the same at the value used to set
    *      psa_ps_remove()
    *      create_flags >>= 1
    *    }
    * However:
    *   - This test doesn't respect the behaviour of defined flags:
    *       - PSA_STORAGE_FLAG_WRITE_ONCE
    *           - If this bit is set then the object cannot be deleted.
    *             psa_xx_remove() will fail but the test current checks the
    *             object can successfully be removed.
    *       - PSA_STORAGE_FLAG_NO_CONFIDENTIALITY
    *           - The psa_trusted_storage_linux implementation always stores
    *             files with authentication (because files are stored with
    *             ecryptfs). Therefore if _NO_CONFIDENTIALITY is requested then
    *             its accepted but files are still stored confidentially. This
    *             is allowed by the spec. The spec also says that if this
    *             happens the the _NO_CONFIDENTIALITY flag should not be
    *             reported with get_info().
    *       - PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION
    *           - This flags should be observed with the psa_xx_get_info()
    *             after creation.
    *  - Only 3 of the flag bits are defined. The specification should be
    *    modified to say something 1) the undefined flag bits
    *    are reserved and the operation of the flags is undefined, or 2)
    *    if flag bit is set then the value should be retained with the object
    *    i.e. the behaviour being tested for here.
    * The solution if to not test for the following flags:
    *   - PSA_STORAGE_FLAG_WRITE_ONCE
    *   - PSA_STORAGE_FLAG_NO_CONFIDENTIALITY
    */

   while (flag)
   {
       if ( (flag & PSA_STORAGE_FLAG_WRITE_ONCE) || (flag & PSA_STORAGE_FLAG_NO_CONFIDENTIALITY) )
       {
           flag = flag >> 1;
           continue;
       }
       /* Create storage with flag value */
       status = SST_FUNCTION(s006_data[1].api, uid, TEST_BUFF_SIZE, write_buff,
                                                         (flag & (~PSA_STORAGE_FLAG_WRITE_ONCE)));

       if (status == s006_data[1].status)
       {
          test_status = psa_sst_remove_api(uid, TEST_BUFF_SIZE, write_buff,
                                          (flag & (~PSA_STORAGE_FLAG_WRITE_ONCE)));
          if (test_status != VAL_STATUS_SUCCESS)
             return test_status;
       }
       else if (status == s006_data[0].status)
       {
          /* Remove UID should fail  */
          status = SST_FUNCTION(s006_data[5].api, uid);
          TEST_ASSERT_EQUAL(status, s006_data[5].status, TEST_CHECKPOINT_NUM(5));
       }

       flag = flag >> 1;
   };

   return status;
}
