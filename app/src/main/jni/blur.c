#include <android/bitmap.h>
#include <malloc.h>
#include "blur.h"
#include <stdio.h>

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
    }

    if (AndroidBitmap_lockPixels(env, src, &srcPixels) < 0) {
        LogE(TAG, "Bitmap src lock pixels failed");
    }

    if (AndroidBitmap_lockPixels(env, dst, &dstPixels) < 0) {
        LogE(TAG, "Bitmap dst lock pixels failed");
    }

    blurPixels((int*) srcPixels, (int*) dstPixels,  info.width, info.height);

    AndroidBitmap_unlockPixels(env, src);
    AndroidBitmap_unlockPixels(env, dst);
}

void blurPixels(int* srcPixels, int* dstPixels, uint32_t col, uint32_t row) {
    int scaleFactor = 10;
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

