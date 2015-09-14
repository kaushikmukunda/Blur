#ifndef BLUR_BLUR_H
#define BLUR_BLUR_H

#include <jni.h>
#include <android/bitmap.h>

#include "log_helper.h"

void boxBlur(uint32_t *srcPixels, uint32_t *dstPixels, uint32_t col, uint32_t row);
void pixelateBlur(int *srcPixels, int *dstPixels, uint32_t width, uint32_t height);
void resizeImage(int* src, int* dst, uint32_t w, uint32_t h, uint32_t nw, uint32_t nh);

#endif //BLUR_BLUR_H
