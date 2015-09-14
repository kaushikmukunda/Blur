#include <android/bitmap.h>
#include <malloc.h>
#include "blur.h"
#include <stdio.h>

#define REDMASK 0xff
#define GREENMASK REDMASK << 8
#define BLUEMASK GREENMASK << 8
#define ALPHAMASK BLUEMASK << 8

#define RED(x)    (((x) & REDMASK))
#define GREEN(x)  ((x) & GREENMASK)
#define BLUE(x)   ((x) & BLUEMASK)

JNIEXPORT jstring JNICALL
Java_sketch_km_com_blur_NativeBlur_loadFromJni(JNIEnv *env, jobject instance) {
    return (*env)->NewStringUTF(env, "Hello JNI");
}

JNIEXPORT void JNICALL
Java_sketch_km_com_blur_NativeBlur_blurImage(JNIEnv *env, jclass type, jobject src, jobject dst) {
    AndroidBitmapInfo info;
    void* srcPixels;
    void* dstPixels;
    char* TAG = "NativeBlur";

    if (AndroidBitmap_getInfo(env, src, &info) < 0) {
        LogE(TAG, "Bitmap getInfo failed!");
        return;
    }

    if (AndroidBitmap_lockPixels(env, src, &srcPixels) < 0) {
        LogE(TAG, "Bitmap src lock pixels failed");
        return;
    }

    if (AndroidBitmap_lockPixels(env, dst, &dstPixels) < 0) {
        LogE(TAG, "Bitmap dst lock pixels failed");
        return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LogE(TAG, "Unsupported image format");
        return;
    }

    char opbuffer[50];
    sprintf(opbuffer, "row=%d col=%d", info.width, info.height);
    LogE(TAG, opbuffer);

    boxBlur((uint32_t*) srcPixels, (uint32_t*) dstPixels, info.width, info.height);

    AndroidBitmap_unlockPixels(env, src);
    AndroidBitmap_unlockPixels(env, dst);
}

void boxBlur(uint32_t* srcPixels, uint32_t* dstPixels, uint32_t col, uint32_t row) {
    horizontalBlur(srcPixels, dstPixels, col, row);
    verticalBlur(dstPixels, col, row);
}

void horizontalBlur(uint32_t* srcPixels, uint32_t* dstPixels, uint32_t col, uint32_t row) {
    uint32_t i,j;
    uint32_t offset;
    uint32_t rAccum, gAccum, bAccum;
    uint8_t halfSize = 30;
    uint8_t currSize = 0;

    for (i=0; i<row; i++) {
        rAccum = 0;
        gAccum = 0;
        bAccum = 0;
        currSize = 1;
        offset = i*col;

        // Compute window
        for (j=0; j<halfSize; j++) {
            rAccum += RED(srcPixels[offset+j]);
            gAccum += GREEN(srcPixels[offset+j]);
            bAccum += BLUE(srcPixels[offset+j]);
            currSize++;
        }

        for (j=0; j<col; j++) {
            dstPixels[offset+j] = (srcPixels[offset+j] & (ALPHAMASK)) |
                    ((rAccum/currSize) & REDMASK) |
                    ((gAccum/currSize) & GREENMASK) |
                    ((bAccum/currSize) & BLUEMASK);

            // move Window
            if (j>=halfSize-1) {
                uint32_t pos = offset + j - halfSize - 1;

                rAccum -= RED(srcPixels[pos]);
                gAccum -= GREEN(srcPixels[pos]);
                bAccum -= BLUE(srcPixels[pos]);
                currSize--;
            }

            if ((j+halfSize+1) < col) {
                uint32_t pos = offset + j + halfSize + 1;
                rAccum += RED(srcPixels[pos]);
                gAccum += GREEN(srcPixels[pos]);
                bAccum += BLUE(srcPixels[pos]);
                currSize++;
            }
        }
    }
}

void verticalBlur(uint32_t* dstPixels, uint32_t col, uint32_t row) {
    uint32_t i,j;
    uint32_t rAccum, gAccum, bAccum;
    uint8_t halfSize = 10;
    uint8_t currSize = 0;

    for (i=0; i<col; i++) {
        rAccum = 0;
        gAccum = 0;
        bAccum = 0;
        currSize = 1;

        // Compute window
        for (j=0; j<halfSize; j++) {
            uint32_t pos = i + j*col;
            rAccum += RED(dstPixels[pos]);
            gAccum += GREEN(dstPixels[pos]);
            bAccum += BLUE(dstPixels[pos]);
            currSize++;
        }

        for (j=0; j<row; j++) {
            uint32_t pos = j*col + i;
            dstPixels[pos] = (dstPixels[pos] & (ALPHAMASK)) |
                    ((rAccum/currSize) & REDMASK) |
                    ((gAccum/currSize) & GREENMASK) |
                    ((bAccum/currSize) & BLUEMASK);

            // move Window
            if (j>=halfSize+1) {
                uint32_t tpos = pos - col * (halfSize + 1);

                rAccum -= RED(dstPixels[tpos]);
                gAccum -= GREEN(dstPixels[tpos]);
                bAccum -= BLUE(dstPixels[tpos]);
                currSize--;
            }

            if ((j+halfSize+1) < row) {
                uint32_t tpos = pos + col * (halfSize + 1);
                rAccum += RED(dstPixels[tpos]);
                gAccum += GREEN(dstPixels[tpos]);
                bAccum += BLUE(dstPixels[tpos]);
                currSize++;
            }

        }
    }
}

/*
 * Pixelate Blur by scaling down image followe by rescaling back up
 * using Nearest Neighbor Algorithm
 */

void pixelateBlur(int *srcPixels, int *dstPixels, uint32_t col, uint32_t row) {
    int scaleFactor = 4;
    int* shrunkPixels = malloc(sizeof(int) * col/scaleFactor * row/scaleFactor);
    resizeImage(srcPixels, shrunkPixels, col, row, col/scaleFactor, row/scaleFactor);
    resizeImage(shrunkPixels, dstPixels, col/scaleFactor, row/scaleFactor, col, row);
    free(shrunkPixels);
}

void resizeImage(int* src, int* dst, uint32_t w, uint32_t h, uint32_t nw, uint32_t nh) {
    uint32_t i, j, ni, nj;
    uint32_t wRatio = ((w << 16)/nw)+1;
    uint32_t hRatio = ((h << 16)/nh)+1;
    char opbuffer[50];
    sprintf(opbuffer, "wratio=%d hratio=%d", wRatio, hRatio);

    for (i=0; i<nh; i++) {
        for (j=0; j<nw; j++) {
            ni = (j*wRatio) >> 16;
            nj = (i*hRatio) >> 16;
            dst[i*nw +j] = src[nj * w + ni];
        }
    }
}

