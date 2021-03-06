// Copyright (C) 2019. Huawei Technologies Co., Ltd. All rights reserved.

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "tensor_computing.h"
#include "utils.h"

int main(int argc, char **argv){
    CHECK_REQUIREMENT(argc == 15);
    // in data
    U32 in = atoi(argv[1]);
    U32 ic = atoi(argv[2]);
    U32 ih = atoi(argv[3]);
    U32 iw = atoi(argv[4]);
    // weight
    U32 fn = atoi(argv[5]);
    U32 fc = atoi(argv[6]);
    U32 fh = atoi(argv[7]);
    U32 fw = atoi(argv[8]);
    // stride & padding
    U32 stride  = atoi(argv[9]);
    U32 padding = atoi(argv[10]);
    // output
    U32 on = atoi(argv[11]);
    U32 oc = atoi(argv[12]);
    U32 oh = atoi(argv[13]);
    U32 ow = atoi(argv[14]);
    CHECK_REQUIREMENT(in == 1 && fn == 1 && fc == 1);
    CHECK_REQUIREMENT(ic == oc && ic % 8 == 0);

    PoolingDesc pooling_desc;
    pooling_desc.pm = Mean;
    
    pooling_desc.stride = stride;
    pooling_desc.padding = padding;
    pooling_desc.kernelSize = fh;
    pooling_desc.rm = CEIL;

    TensorDesc input_desc= tensor4df(DT_I8, DF_NCHWC8, in, ic, ih, iw);
    TensorDesc in_desc_ref = input_desc;
    in_desc_ref.dt = DT_F16;

    TensorDesc output_desc;
    CHECK_STATUS(pooling_infer_output_size(input_desc, pooling_desc, &output_desc));
    U32 input_len = tensorNumElements(input_desc);
    U32 output_len = tensorNumElements(output_desc);
    CHECK_REQUIREMENT(input_len == in*ic*ih*iw && output_len == on*oc*oh*ow);

    F16* input_ref  = ut_input_v<F16>(input_len, UT_INIT_RANDOM);
    INT8* input = ut_input_v<INT8>(input_len, UT_INIT_ZERO);
    F16 scales[2];
    quantize_tensor(in_desc_ref, input_ref, &input_desc, input, scales);

    INT8* output = ut_input_v<INT8>(output_len, UT_INIT_ZERO);
    F16* out_d = ut_input_v<F16>(output_len, UT_INIT_ZERO);
    F16* output_ref = ut_input_v<F16>(output_len, UT_INIT_ZERO);

    if(UT_CHECK){
        CHECK_STATUS(pooling(input_desc, input,
                             pooling_desc, scales,
                             output_desc, output,
                             UT_ARCH));

        for (U32 i=0; i<output_len; i++) {
            out_d[i] = output[i] / scales[1];
        }

        CHECK_STATUS(pooling(in_desc_ref, input_ref,
                             pooling_desc, nullptr,
                             output_desc, output_ref,
                             CPU_GENERAL));

        // check
        ut_check_v<F16>(out_d, output_ref, output_len, F16(0.05), __FILE__, __LINE__);
    }

    // benchmark
    double time_start = ut_time_ms();
    for(int iter=0; iter<UT_LOOPS; iter++){
        CHECK_STATUS(pooling(input_desc, input,
                             pooling_desc, scales,
                             output_desc, output,
                             UT_ARCH));
    }
    double time_end = ut_time_ms();
    double time = (time_end - time_start) / UT_LOOPS;

    // log performance data
    char buffer[150];
    char params[120];
    sprintf(params, "(%u %u %u %u)+(%u %u %u %u)/(%u %u)=(%u %u %u %u)",
                    in, ic, ih, iw,
                    fn, fc, fh, fw,
                    stride, padding,
                    on, oc, oh, ow);
    sprintf(buffer, "%20s, %80s", "Pooling", params);
    double ops = 1.0 * on * oc * oh * ow * fh * fw;
    ut_log<INT8>(buffer, ops, time);

    free(input);
    free(input_ref);
    free(output);
    free(out_d);
    free(output_ref);

    return 0;
}
