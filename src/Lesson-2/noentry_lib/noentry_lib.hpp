/**
 * @file noentry_lib.hpp
 * @author M JAYANTH VARMA (jayanthvarma134@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#define MAX_LABEL_SIZE 128
#ifdef __cplusplus
extern "C" {
#endif

typedef struct NoEntryCtx NoEntryCtx;

// Init parameters structure as input, required for instantiating noentry_lib
typedef struct
{
  int processingWidth;
  int processingHeight;
  int fullFrame;
} NoEntryInitParams;

// Detected/Labelled object structure, stores bounding box info along with label
typedef struct
{
  float left;
  float top;
  float width;
  float height;
  char label[MAX_LABEL_SIZE];
} NoEntryObject;

// Output data returned after processing
typedef struct
{
  int numObjects;
  NoEntryObject object[4];
} NoEntryOutput;

// Initialize library context
NoEntryCtx * NoEntryCtxInit (NoEntryInitParams *init_params);

// Dequeue processed output
NoEntryOutput *NoEntryProcess (NoEntryCtx *ctx, unsigned char *data);

// Deinitialize library context
void NoEntryCtxDeinit (NoEntryCtx *ctx);

#ifdef __cplusplus
}
#endif