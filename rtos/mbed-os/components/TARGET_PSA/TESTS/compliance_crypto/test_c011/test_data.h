/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#include "val_crypto.h"

typedef struct {
    char                    test_desc[50];
    psa_algorithm_t         alg;
    psa_status_t            expected_status;
} test_data;

static test_data check1[] = {
#ifdef ARCH_TEST_MD2
{"Test psa_hash_setup with MD2 algorithm\n",
 PSA_ALG_MD2, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_MD4
{"Test psa_hash_setup with MD4 algorithm\n",
 PSA_ALG_MD4, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_MD5
{"Test psa_hash_setup with MD5 algorithm\n",
 PSA_ALG_MD5, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_RIPEMD160
{"Test psa_hash_setup with RIPEMD160 algorithm\n",
 PSA_ALG_RIPEMD160, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA1
{"Test psa_hash_setup with SHA1 algorithm\n",
 PSA_ALG_SHA_1, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA224
{"Test psa_hash_setup with SHA224 algorithm\n",
 PSA_ALG_SHA_224, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA256
{"Test psa_hash_setup with SHA256 algorithm\n",
 PSA_ALG_SHA_256, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA384
{"Test psa_hash_setup with SHA384 algorithm\n",
 PSA_ALG_SHA_384, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA512
{"Test psa_hash_setup with SHA512 algorithm\n",
 PSA_ALG_SHA_512, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA512_224
{"Test psa_hash_setup with SHA512_224 algorithm\n",
 PSA_ALG_SHA_512_224, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA512_256
{"Test psa_hash_setup with SHA512_256 algorithm\n",
 PSA_ALG_SHA_512_256, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA3_224
{"Test psa_hash_setup with SHA3_224 algorithm\n",
 PSA_ALG_SHA3_224, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA3_256
{"Test psa_hash_setup with SHA3_256 algorithm\n",
 PSA_ALG_SHA3_256, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA3_384
{"Test psa_hash_setup with SHA3_384 algorithm\n",
 PSA_ALG_SHA3_384, PSA_SUCCESS,
},
#endif

#ifdef ARCH_TEST_SHA3_512
{"Test psa_hash_setup with SHA3_512 algorithm\n",
 PSA_ALG_SHA3_512, PSA_SUCCESS,
},
#endif

{"Test psa_hash_setup with Invalid algorithm\n",
 PSA_ALG_INVALID, PSA_ERROR_INVALID_ARGUMENT,
},
};
