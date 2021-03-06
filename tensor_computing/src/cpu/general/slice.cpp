// Copyright (C) 2019. Huawei Technologies Co., Ltd. All rights reserved.

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include <string.h>
#include <vector>
#include "cpu/general/tensor_computing_general.h"

EE slice_general(TensorDesc inputDesc, void* input,
    std::vector<TensorDesc> outputDesc, std::vector<void*>* output)
{
    UNUSED(inputDesc);
    if (nullptr == input)
        CHECK_STATUS_WITH_RETURN(NULL_POINTER);
    U32 num = outputDesc.size();
    if (num < 1) return NOT_MATCH;

    U8 *ptr = (U8 *)input;
    for (U32 i = 0; i < num; i++) {
        if (nullptr == (*output)[i])
            CHECK_STATUS_WITH_RETURN(NULL_POINTER);
        memcpy((*output)[i], ptr, tensorNumBytes(outputDesc[i]));
        ptr += tensorNumBytes(outputDesc[i]);
    }
    return SUCCESS;
}
