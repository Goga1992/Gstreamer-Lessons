/**
 * Copyright (c) 2017-2020, NVIDIA CORPORATION.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "dsexample2_lib.h"
#include <stdio.h>
#include <stdlib.h>

struct DsExample2Ctx
{
    DsExample2InitParams initParams;
};

DsExample2Ctx *
DsExample2CtxInit (DsExample2InitParams * initParams)
{
    DsExample2Ctx *ctx = (DsExample2Ctx *) calloc (1, sizeof (DsExample2Ctx));
    ctx->initParams = *initParams;
    return ctx;
}

// In case of an actual processing library, processing on data wil be completed
// in this function and output will be returned
DsExample2Output *
DsExample2Process (DsExample2Ctx * ctx, unsigned char *data)
{
    DsExample2Output *out =
        (DsExample2Output*)calloc (1, sizeof (DsExample2Output));

    if (data != NULL)
    {
        // Process your data here
    }
    // Fill output structure using processed output
    // Here, we fake some detected objects and labels

    out->numObjects = 1;
    out->object[0] = (DsExample2Object)
    {
        0, 0, 0, 0, ""
    };
    // Set the object label
    snprintf (out->object[0].label, 64, "Obj_label");
    return out;
}

void
DsExample2CtxDeinit (DsExample2Ctx * ctx)
{
    free (ctx);
}
