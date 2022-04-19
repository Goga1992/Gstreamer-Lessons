/**
 * @file noentry_lib.cpp
 * @author M JAYANTH VARMA (jayanthvarma134@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "noentry_lib.hpp"
#include <stdio.h>
#include <stdlib.h>

struct NoEntryCtx
{
    NoEntryInitParams initParams;
};

NoEntryCtx *
NoEntryCtxInit (NoEntryInitParams * initParams)
{
    NoEntryCtx *ctx = (NoEntryCtx *) calloc (1, sizeof (NoEntryCtx));
    ctx->initParams = *initParams;
    return ctx;
}

// In case of an actual processing library, processing on data wil be completed
// in this function and output will be returned
NoEntryOutput *
NoEntryProcess (NoEntryCtx * ctx, unsigned char *data)
{
    NoEntryOutput *out =
        (NoEntryOutput*)calloc (1, sizeof (NoEntryOutput));

    if (data != NULL)
    {
        // Process your data here
    }
    // Fill output structure using processed output
    // Here, we fake some detected objects and labels
    if (ctx->initParams.fullFrame)
    {
        out->numObjects = 2;
        out->object[0] = (NoEntryObject)
        {
            (float)(ctx->initParams.processingWidth) / 8,
                (float)(ctx->initParams.processingHeight) / 8,
                (float)(ctx->initParams.processingWidth) / 8,
                (float)(ctx->initParams.processingHeight) / 8, "Obj0"
        };

        out->object[1] = (NoEntryObject)
        {
            (float)(ctx->initParams.processingWidth) / 2,
                (float)(ctx->initParams.processingHeight) / 2,
                (float)(ctx->initParams.processingWidth) / 8,
                (float)(ctx->initParams.processingHeight) / 8, "Obj1"
        };
    }
    else
    {
        out->numObjects = 1;
        out->object[0] = (NoEntryObject)
        {
            (float)(ctx->initParams.processingWidth) / 8,
                (float)(ctx->initParams.processingHeight) / 8,
                (float)(ctx->initParams.processingWidth) / 8,
                (float)(ctx->initParams.processingHeight) / 8, ""
        };
        // Set the object label
        snprintf (out->object[0].label, 64, "Obj_label");
    }

    return out;
}

void
NoEntryCtxDeinit (NoEntryCtx * ctx)
{
    free (ctx);
}
