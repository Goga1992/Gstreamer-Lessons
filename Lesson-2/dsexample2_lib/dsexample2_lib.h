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

#ifndef __DSEXAMPLE2_LIB__
#define __DSEXAMPLE2_LIB__

#define MAX_LABEL_SIZE 128
#ifdef __cplusplus
extern "C" {
#endif

typedef struct DsExample2Ctx DsExample2Ctx;

// Init parameters structure as input, required for instantiating dsexample2_lib
typedef struct
{
  int frame_num;
  int unique_id;
  int gpu_id;
} DsExample2InitParams;

// Detected/Labelled object structure, stores bounding box info along with label
typedef struct
{
  float left;
  float top;
  float width;
  float height;
  char label[MAX_LABEL_SIZE];
} DsExample2Object;

// Output data returned after processing
typedef struct
{
  int numObjects;
  DsExample2Object object[4];
} DsExample2Output;

// Initialize library context
DsExample2Ctx * DsExample2CtxInit (DsExample2InitParams *init_params);

// Dequeue processed output
DsExample2Output *DsExample2Process (DsExample2Ctx *ctx, unsigned char *data);

// Deinitialize library context
void DsExample2CtxDeinit (DsExample2Ctx *ctx);

#ifdef __cplusplus
}
#endif

#endif
