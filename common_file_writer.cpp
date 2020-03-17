/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */


#include "common_file_writer.h"
#include "file.h"
#include <sstream>
#include "whitespace_utils.h"

Common_file_writer::Common_file_writer()
{

}

bool Common_file_writer::write_native_common_h(std::string dest_dir)
{
    std::stringstream stream;

    stream << "#ifndef NATIVECOMMON_H";
    Whitespace_utils::write_newline(stream,1);
    stream << "#define NATIVECOMMON_H";
    Whitespace_utils::write_newline(stream,2);
    stream << "#ifdef Q_OS_ANDROID";
    Whitespace_utils::write_newline(stream,2);
    stream << "#include <functional>";
    Whitespace_utils::write_newline(stream,1);
    stream << "#include <stddef.h>";
    Whitespace_utils::write_newline(stream,1);
    stream << "#include <android/log.h>";
    Whitespace_utils::write_newline(stream,1);
    stream << "#include \"jni.h\"";
    Whitespace_utils::write_newline(stream,4);
    stream << "typedef std::function<int (JNIEnv* env, const char* className,JNINativeMethod* gMethods, int numMethods)> RegisterNativeMethods;";
    Whitespace_utils::write_newline(stream,4);
    stream << "#endif // Q_OS_ANDROID";
    Whitespace_utils::write_newline(stream,2);
    stream << "#endif // NATIVECOMMON_H";

    return File::write(stream.str(),dest_dir + "/" + native_common + ".h");
}

bool Common_file_writer::write_native_register_h(std::string dest_dir)
{
    std::stringstream stream;

    stream << "#ifndef NATIVE_REGISTER_H";
    Whitespace_utils::write_newline(stream,1);
    stream << "#define NATIVE_REGISTER_H";
    Whitespace_utils::write_newline(stream,3);
    stream << "#ifdef Q_OS_ANDROID";
    Whitespace_utils::write_newline(stream,3);
    stream << "#include <QObject>";
    Whitespace_utils::write_newline(stream,1);
    stream << "#include \"" << native_common << ".h\";";
    Whitespace_utils::write_newline(stream,2);
    stream << "#define LOGE(...) do { __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__); } while(0)";
    Whitespace_utils::write_newline(stream,1);
    stream << "#define LOGI(...) do { __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__); } while(0)";
    Whitespace_utils::write_newline(stream,1);
    stream << "#define LOGW(...) do { __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__); } while(0)";
    Whitespace_utils::write_newline(stream,2);
    stream << "int registerNativeMethods(JNIEnv* env, const char* className,";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_tab(stream,1);
    stream << "JNINativeMethod* methods, int numMethods);";
    Whitespace_utils::write_newline(stream,3);
    stream << "#endif       //Q_OS_ANDROID";
    Whitespace_utils::write_newline(stream,3);
    stream << "#endif       //NATIVE_REGISTER_H";

    return File::write(stream.str(),dest_dir + "/" + native_register + ".h");

}

bool Common_file_writer::write_native_register_cpp(std::string dest_dir,const std::map<std::string, std::string>& classes)
{
    std::stringstream stream;
    stream << "#ifdef Q_OS_ANDROID";
    Whitespace_utils::write_newline(stream,2);
    stream << "#include \"" << native_register << ".h\";";
    Whitespace_utils::write_newline(stream,1);
    //write other includes
    auto map_iterator = classes.begin();
    while (map_iterator != classes.end()) {
        stream << "#include \""<< map_iterator->second <<"\";";
        Whitespace_utils::write_newline(stream,1);
        map_iterator++;
    }


    Whitespace_utils::write_newline(stream,3);
    stream << "#define LOG_TAG \"NativeRegister\"";
    Whitespace_utils::write_newline(stream,2);

    //UnionJNIEnvToVoid
    stream << "typedef union {";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "JNIEnv* env;";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "void* venv;";
    Whitespace_utils::write_newline(stream,1);
    stream << "} UnionJNIEnvToVoid;";
    Whitespace_utils::write_newline(stream,2);

    //registerNativeMethods
    stream << "/*";
    Whitespace_utils::write_newline(stream,1);
    stream << "* Register native methods of a class.";
    Whitespace_utils::write_newline(stream,1);
    stream << "*/";
    Whitespace_utils::write_newline(stream,1);
    stream << "int registerNativeMethods(JNIEnv* env, const char* className,";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_tab(stream,2);
    stream << "JNINativeMethod* methods, int numMethods)";
    Whitespace_utils::write_newline(stream,1);
    stream << "{";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "jclass clazz;";
    Whitespace_utils::write_newline(stream,2);
    Whitespace_utils::write_space(stream,2);
    stream << "clazz = env->FindClass(className);";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "if (clazz == nullptr) {";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,4);
    stream << "LOGE(\"registerNativeMethods unable to find class '%s'\", className);";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,4);
    stream << "return JNI_FALSE;";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "}";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "if (env->RegisterNatives(clazz, methods, numMethods) < 0) {";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,4);
    stream << "LOGE(\"RegisterNatives failed for '%s'\", className);";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,4);
    stream << "return JNI_FALSE;";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "}";
    Whitespace_utils::write_newline(stream,2);
    Whitespace_utils::write_space(stream,2);
    stream << "return JNI_TRUE;";
    Whitespace_utils::write_newline(stream,1);
    stream << "}";
    Whitespace_utils::write_newline(stream,2);

    //JNI_OnLoad
    stream << "/*";
    Whitespace_utils::write_newline(stream,1);
    stream << "* This is called by the VM when the shared library is first loaded.";
    Whitespace_utils::write_newline(stream,1);
    stream << "*/";
    Whitespace_utils::write_newline(stream,1);
    stream << "jint JNI_OnLoad(JavaVM* vm, void* reserved) {";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "Q_UNUSED(reserved)";
    Whitespace_utils::write_newline(stream,2);
    Whitespace_utils::write_space(stream,2);
    stream << "UnionJNIEnvToVoid uenv;";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "uenv.venv = nullptr;";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "JNIEnv *env = nullptr;";
    Whitespace_utils::write_newline(stream,2);
    Whitespace_utils::write_space(stream,2);
    stream << "LOGI(\"JNI_OnLoad\");";
    Whitespace_utils::write_newline(stream,2);
    stream << "// get the JNIEnv pointer.";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_6) != JNI_OK) {";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,4);
    stream << "LOGE(\"ERROR: GetEnv failed\");";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,4);
    stream << "return JNI_ERR;";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << "}";
    Whitespace_utils::write_newline(stream,2);
     Whitespace_utils::write_space(stream,2);
    stream << "env = uenv.env;";
    Whitespace_utils::write_newline(stream,2);
    stream << "// register native methods for classes";
    Whitespace_utils::write_newline(stream,1);
    map_iterator = classes.begin();
    while (map_iterator != classes.end()) {
        stream << "if ("<< map_iterator->first <<"::init(env, &registerNativeMethods) != JNI_TRUE) {";
        Whitespace_utils::write_newline(stream,1);
        Whitespace_utils::write_space(stream,4);
        stream << "LOGE(\"ERROR: initialization of "<< map_iterator->first << " failed\");";
        Whitespace_utils::write_newline(stream,1);
        Whitespace_utils::write_space(stream,4);
        stream << "return JNI_ERR;";
        Whitespace_utils::write_newline(stream,1);
        Whitespace_utils::write_space(stream,2);
        stream << "}";
        Whitespace_utils::write_newline(stream,2);
        map_iterator++;
    }
    Whitespace_utils::write_space(stream,2);
    stream << "return JNI_VERSION_1_6;";
    Whitespace_utils::write_newline(stream,1);
    stream << "}";
    Whitespace_utils::write_newline(stream,3);
    stream << "#endif";

    return File::write(stream.str(),dest_dir  + "/" + native_register + ".cpp");
}
