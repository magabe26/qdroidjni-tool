qdroidjni-tool generates qt classes from android java classes with native methods.

                               USAGE


	1. Write a java class
	---------------------

```text
           # File.java
```
```java
package com.magabelab.lab;

import java.lang.String;

 class File{
		public native String open(String name,int mode);
		public native String close(int mode);
    }

```

	2. Write qdroidjni-tool configuration file
	------------------------------------------
```text
           # config.txt
```
```text
dir: "A:/ProjectA/jni"
(File,file < "A:/ProjectA/com/magabelab/lab/File.java")
```
    let us dissect the above configuration file

```text
dir: "A:/ProjectA/jni" ______ Where the generated Qt files are placed

(File,file < "A:/ProjectA/com/magabelab/lab/File.java")
  |     |           |
  |     |           |
  |     |           |____________
  |     |                        Path to File.java
  |     |
  |     |_______ 
  |             Qt file name prefix for file.h and file.cpp
  |
  |________ Qt class name
```

	3. Run qdroidjni-tool with the configuration file as input
	----------------------------------------------------------

```text
           qdroidjni-tool /path/to/config.txt
```

  After qdroidjni-tool completes, 5 files are generated

  i)   native_common.h   
  ii)  native_register.h
  iii) native_register.cpp 
  iv)  file.h
  v)   file.cpp

```text
           # i) native_common.h
```
```c++
#ifndef NATIVECOMMON_H
#define NATIVECOMMON_H

#ifdef Q_OS_ANDROID

#include <functional>
#include <stddef.h>
#include <android/log.h>
#include "jni.h"



typedef std::function<int (JNIEnv* env, const char* className,JNINativeMethod* gMethods, int numMethods)> RegisterNativeMethods;



#endif // Q_OS_ANDROID

#endif // NATIVECOMMON_H

```


```text
          # ii)  native_register.h
```
```c++

#ifndef NATIVE_REGISTER_H
#define NATIVE_REGISTER_H


#ifdef Q_OS_ANDROID


#include <QObject>
#include "native_common.h";

#define LOGE(...) do { __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__); } while(0)
#define LOGI(...) do { __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__); } while(0)
#define LOGW(...) do { __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__); } while(0)

int registerNativeMethods(JNIEnv* env, const char* className,
	JNINativeMethod* methods, int numMethods);


#endif       //Q_OS_ANDROID


#endif       //NATIVE_REGISTER_H


```

```text
          # iii) native_register.cpp 
```
```c++
#ifdef Q_OS_ANDROID

#include "native_register.h";
#include "file.h";



#define LOG_TAG "NativeRegister"

typedef union {
  JNIEnv* env;
  void* venv;
} UnionJNIEnvToVoid;

/*
* Register native methods of a class.
*/
int registerNativeMethods(JNIEnv* env, const char* className,
		JNINativeMethod* methods, int numMethods)
{
  jclass clazz;

  clazz = env->FindClass(className);
  if (clazz == nullptr) {
    LOGE("registerNativeMethods unable to find class '%s'", className);
    return JNI_FALSE;
  }
  if (env->RegisterNatives(clazz, methods, numMethods) < 0) {
    LOGE("RegisterNatives failed for '%s'", className);
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

/*
* This is called by the VM when the shared library is first loaded.
*/
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  Q_UNUSED(reserved)

  UnionJNIEnvToVoid uenv;
  uenv.venv = nullptr;
  JNIEnv *env = nullptr;

  LOGI("JNI_OnLoad");

// get the JNIEnv pointer.
  if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_6) != JNI_OK) {
    LOGE("ERROR: GetEnv failed");
    return JNI_ERR;
  }

  env = uenv.env;

// register native methods for classes
if (File::init(env, &registerNativeMethods) != JNI_TRUE) {
    LOGE("ERROR: initialization of File failed");
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}


#endif

```

```text
          # iv)  file.h
```
```c++
#ifndef FILE_H
#define FILE_H

#include <QObject>

#ifdef Q_OS_ANDROID
#include "native_common.h";
#endif

class File : public QObject
{
    Q_OBJECT
public:
    static File& instance();
private:
    File();

#ifdef Q_OS_ANDROID
    friend jint JNI_OnLoad(JavaVM* vm, void* reserved);
    static int init(JNIEnv *env,RegisterNativeMethods registerNativeMethods);
#endif

};

#endif //FILE_H
```


```text
          # iv)  file.cpp
```
```c++
#include "file.h";


#ifdef Q_OS_ANDROID

/*
*Method: open
*Signature: (Ljava/lang/String;I)Ljava/lang/String;
*/
jstring open(JNIEnv* env, jobject obj, jstring name, jint mode)
{

}

/*
*Method: close
*Signature: (I)Ljava/lang/String;
*/
jstring close(JNIEnv* env, jobject obj, jint mode)
{

}


/*
* @brief File::init is called ounce by JNI_OnLoad to register native methods
*  of java class "com.magabe.lab.File"
* @param env - JNIEnv pointer
* @param registerNativeMethods - RegisterNativeMethods function
* @return an int that is either JNI_TRUE (if the native methods have been registered successfully) or JNI_FALSE
*/
int File::init(JNIEnv *env, RegisterNativeMethods registerNativeMethods)
{
  JNINativeMethod method_table[] = {
	{"open", "(Ljava/lang/String;I)Ljava/lang/String;", reinterpret_cast<void*>(open)},
	{"close", "(I)Ljava/lang/String;", reinterpret_cast<void*>(close)}
 };

 if (!registerNativeMethods(env, "com/magabe/lab/File",
			method_table, sizeof(method_table) / sizeof(method_table[0]))) {
     return JNI_FALSE;
   }

     return JNI_TRUE;
}

#endif //Q_OS_ANDROID


// Platform independent methods


/*
* @brief File::File
*/
File::File()
{

}

/*
* @brief File::instance
*/
File& File::instance()
{
    static File i_file;
    return i_file;
}


```		


                           BUILDING qdroidjni-tool

To build qdroidjni-tool use one of build scripts ( build-debug or build-release).
	Before running the build script make sure config scripts are configurd with valid paths to c++ compiler,cmake and make 