/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class android_ImgSdk_Imaginer */

#ifndef _Included_android_ImgSdk_Imaginer
#define _Included_android_ImgSdk_Imaginer
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     android_ImgSdk_Imaginer
 * Method:    init
 * Signature: ([III)Z
 */
JNIEXPORT jboolean JNICALL Java_android_ImgSdk_Imaginer_init
  (JNIEnv *, jobject, jintArray, jint, jint);

/*
 * Class:     android_ImgSdk_Imaginer
 * Method:    isStartPoint
 * Signature: (II)[I
 */
JNIEXPORT jintArray JNICALL Java_android_ImgSdk_Imaginer_isStartPoint
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     android_ImgSdk_Imaginer
 * Method:    getNextPoint
 * Signature: (II)[I
 */
JNIEXPORT jintArray JNICALL Java_android_ImgSdk_Imaginer_getNextPoint
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     android_ImgSdk_Imaginer
 * Method:    cfinalize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_android_ImgSdk_Imaginer_cfinalize
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
