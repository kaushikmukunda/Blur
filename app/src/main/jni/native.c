#include <jni.h>

JNIEXPORT jstring JNICALL
Java_sketch_km_com_blur_MainActivity_loadFromJni(JNIEnv *env, jobject instance) {
    return (*env)->NewStringUTF(env, "Hello Jni World3!");
}