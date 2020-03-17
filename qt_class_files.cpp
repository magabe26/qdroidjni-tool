/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#include "qt_class_files.h"
#include "file.h"
#include "string_utils.h"
#include "whitespace_utils.h"
#include "debug.h"
#include <sstream>
#include "comment_parser.h"
#include "common_file_writer.h"


Qt_class_files::Qt_class_files(Qt_class &qt_class)
    :qt_class_{qt_class}
{
}

bool Qt_class_files::header_file_exist()
{
    return File::exists(qt_class_.header_file_path());
}

bool Qt_class_files::source_file_exist()
{
    return File::exists(qt_class_.source_file_path());
}

Parser Qt_class_files::jni_type_parser(){
    return (string("jboolean")
            | string("jchar")
            | string("jbyte")
            | string("jshort")
            | string("jint")
            | string("jlong")
            | string("jfloat")
            | string("jdouble")
            | string("void")
            | string("jobject")
            | string("jclass")
            | string("jstring")
            | string("jarray")
            | string("jthrowable")
            | string("jsize")).seq(string("Array").optional());
}

Function_parser Qt_class_files::native_function_parser(){
    auto fun_declaration = jni_type_parser().seq(any(jni_env_pointer_parser()).plus())
            .seq(jni_env_pointer_parser())
            .seq(any(chaR(')')).plus()).seq(chaR(')'));

    return  fn(fun_declaration);
}

void Qt_class_files::copy_qt_methods_from_source(std::string source_file_content){
    std::vector<Qt_method> qt_methods;
    auto list = (comment().seq(spaceOptional()).seq(fn(qt_class_.class_name())) | fn(qt_class_.class_name())).allStringMatches(source_file_content);
    if(list.empty()){
        return;
    }
    for(std::vector<std::string>::size_type i = 0; i < list.size() ; i++){
        auto function = list.at(i);
        auto fn_boundary = fn_declaration(qt_class_.class_name()).seq(spaceOptional()).seq(chaR('{'));
        auto function_comment = get_comment(function,fn_boundary);// String_utils::trim(comment().firstStringMatch(function));
        auto function_definition = String_utils::trim(fn(qt_class_.class_name()).firstStringMatch(function));
        if(!function_definition.empty()){
            Qt_method method(qt_class_.class_name(),function_definition,function_comment);
            qt_class_.qt_methods_.push_back(method);
        }
    }
}

Qt_method Qt_class_files::get_constructor_method() const
{
    for(std::vector<Qt_method>::size_type i = 0; i < qt_class_.qt_methods().size() ; i++){
        auto method = qt_class_.qt_methods().at(i);
        if(method.is_constructor()){
            return method;
        }
    }
    return  Qt_method("","","");
}

Qt_method Qt_class_files::get_instance_method() const{
    for(std::vector<Qt_method>::size_type i = 0; i < qt_class_.qt_methods().size() ; i++){
        auto method = qt_class_.qt_methods().at(i);
        if(method.is_instance_method()){
            return method;
        }
    }
    return  Qt_method("","","");
}

Qt_method Qt_class_files::get_init_method()  const
{
    for(std::vector<Qt_method>::size_type i = 0; i < qt_class_.qt_methods().size() ; i++){
        auto method = qt_class_.qt_methods().at(i);
        if(method.is_init_method()){
            return method;
        }
    }
    return  Qt_method("","","");
}

std::vector<Qt_method> Qt_class_files::get_other_qt_methods() const
{
    std::vector<Qt_method> methods;
    for(std::vector<Qt_method>::size_type i = 0; i < qt_class_.qt_methods().size() ; i++){
        auto method = qt_class_.qt_methods().at(i);
        if((!method.is_init_method()) && (!method.is_constructor()) && (!method.is_instance_method())){
            methods.push_back(method);
        }
    }
    return  methods;
}

bool Qt_class_files::write_source_file()
{

    if(qt_class_.source_file_path().empty()){
        return false;
    }

    if(!source_file_exist()){
        return File::write(source_file_content(),qt_class_.source_file_path());
    }else{
        auto old_content = read_source_file();

        if(String_utils::trim(old_content).empty()){
            return File::write(source_file_content(),qt_class_.source_file_path());
        }else{
            std::vector<C_native_method> qualified_native_methods;
            auto disc_methods =  get_native_methods_from_content(old_content);
            for(std::vector<C_native_method>::size_type i = 0;i < qt_class_.c_native_methods().size() ; i++){
                auto neW = qt_class_.c_native_methods().at(i);
                bool are_same = false;
                for(std::vector<C_native_method>::size_type j = 0;j < disc_methods.size() ; j++){
                    auto olD = disc_methods.at(j);
                    if(C_native_method::are_same(olD,neW)){
                        //deal with static status
                        if(olD.represent_static_java_native_ != neW.represent_static_java_native_){
                            olD.represent_static_java_native_ = neW.represent_static_java_native_;
                        }
                        //copy parameters because may be changed
                        olD.method_parameters_.clear();
                        for(std::vector<C_native_method_parameter>::size_type i = 0;i < neW.method_parameters_.size(); i++){
                            olD.method_parameters_.push_back(neW.method_parameters_.at(i));
                        }

                        qualified_native_methods.push_back(olD);
                        are_same = true;
                        break;
                    }
                }
                if(!are_same){
                    qualified_native_methods.push_back(neW);
                }
            }

            qt_class_.c_native_methods_.clear();

            //copy qualified_native_methods
            for(std::vector<C_native_method>::size_type j = 0;j < qualified_native_methods.size() ; j++){
                qt_class_.c_native_methods_.push_back(qualified_native_methods.at(j));
            }
            //copy qt methods
            copy_qt_methods_from_source(old_content);

            //copy includes
            copy_includes(old_content);

            //copy static variable
            copy_static_variables(old_content);

            return File::write(source_file_content(),qt_class_.source_file_path());
        }
    }
}

void Qt_class_files::copy_static_variables(std::string source_file_content){

    qt_class_.android_specific_static_variables_.clear();
    qt_class_.platiform_independent_static_variables_.clear();

    auto platiform_independent = platiform_independent_static_variables(source_file_content);
    for(std::vector<std::string>::size_type i =0; i < platiform_independent.size(); i++ ){
        qt_class_.platiform_independent_static_variables_.push_back(platiform_independent.at(i));
    }

    auto android_specific = android_specific_static_variables(source_file_content);
    for(std::vector<std::string>::size_type i =0; i < android_specific.size(); i++ ){
        qt_class_.android_specific_static_variables_.push_back(android_specific.at(i));
    }
}

void Qt_class_files::write_platiform_independent_static_variables_to_source_file(std::stringstream &stream) const
{
    if(qt_class_.platiform_independent_static_variables_.empty()){
        return;
    }else{
        Whitespace_utils::write_newline(stream,2);
    }
    for(std::vector<std::string>::size_type i = 0; i< qt_class_.platiform_independent_static_variables_.size(); i++){
        stream << qt_class_.platiform_independent_static_variables_.at(i);
        Whitespace_utils::write_newline(stream,1);
    }

}

void Qt_class_files::write_android_specific_static_variables_to_source_file(std::stringstream &stream) const
{
    if(qt_class_.android_specific_static_variables_.empty()){
        return;
    }else{
        Whitespace_utils::write_newline(stream,2);
    }
    for(std::vector<std::string>::size_type i = 0; i< qt_class_.android_specific_static_variables_.size(); i++){
        stream << qt_class_.android_specific_static_variables_.at(i);
        Whitespace_utils::write_newline(stream,1);
    }

}
std::string Qt_class_files::platiform_independent_search_context(std::string source_file_content)
{
    auto android_if_def = string("#ifdef").seq(spaceOptional()).seq(string("Q_OS_ANDROID"));
    return  any(android_if_def).plus().firstStringMatch(source_file_content);
}

std::string Qt_class_files::android_specific_search_context(std::string source_file_content){
    auto android_if_def = string("#ifdef").seq(spaceOptional()).seq(string("Q_OS_ANDROID"));
    auto m = android_if_def.firstMatch(source_file_content);
    if(m.isSuccess()){
        return source_file_content.substr(m.success().end());
    }else{
        return  "";
    }
}

void Qt_class_files::copy_includes(std::string source_file_content){

    auto android_if_def = string("#ifdef").seq(spaceOptional()).seq(string("Q_OS_ANDROID"));
    auto search_content =  any(android_if_def).plus().firstStringMatch(source_file_content);
    if(!search_content.empty()) {
        auto include_parser =  string("#include").seq(space()).seq(any(chaR(';')).plus()).seq(chaR(';'));
        include_parser.replaceInMapped(search_content,[this](std::string include)->std::string{
            qt_class_.platiform_independent_includes_.push_back(String_utils::trim(include));
            return include;
        });
    }

    auto m = android_if_def.firstMatch(source_file_content);
    if(m.isSuccess()){
        search_content = source_file_content.substr(m.success().end());
        if(!search_content.empty()) {
            auto include_parser =  string("#include").seq(space()).seq(any(chaR(';')).plus()).seq(chaR(';'));
            include_parser.replaceInMapped(search_content,[this](std::string include)->std::string{
                qt_class_.android_specific_includes_.push_back(String_utils::trim(include));
                return include;
            });
        }
    }
}

Parser Qt_class_files::static_variable_parser(){
    return  string("static").seq(space()).seq((letter() | chaR('_')).seq((chaR('_') | word()).star()))
            .seq(space()).seq((letter() | chaR('_')).seq((chaR('_') | word()).star())).seq(spaceOptional())
            .seq(chaR('=')).seq(spaceOptional()).seq(any(chaR(';')).plus()).seq(chaR(';'));
}

std::vector<std::string> Qt_class_files::platiform_independent_static_variables(std::string source_file_content)
{
    auto search_content =  platiform_independent_search_context(source_file_content);
    return static_variable_parser().allStringMatches(search_content);
}

std::vector<std::string> Qt_class_files::android_specific_static_variables(std::string source_file_content)
{
    auto search_content =  android_specific_search_context(source_file_content);
    return static_variable_parser().allStringMatches(search_content);
}

Parser Qt_class_files::jni_env_pointer_parser(){
    return string("JNIEnv").seq(spaceOptional()).seq(chaR('*'));
}

std::string Qt_class_files::method_table_string(std::string table_name) const{
    std::stringstream stream;
    stream << "JNINativeMethod "<< table_name <<"[] = {";

    Whitespace_utils::write_newline(stream,1);
    for(std::vector<Java_native_method>::size_type i = 0;i< qt_class_.c_native_methods().size(); i++){
        auto j_n_method = qt_class_.c_native_methods().at(i);
        auto name = j_n_method.name();
        std::string signature = j_n_method.signature();
        Whitespace_utils::write_tab(stream,1);
        stream << "{\"" << name << "\", \"" << signature << "\", reinterpret_cast<void*>(" << name << ")}";
        if(i != (qt_class_.c_native_methods().size() - 1)){
            stream << ",";
        }
        Whitespace_utils::write_newline(stream,1);
    }
    stream << " };";
    return stream.str();
}

std::string Qt_class_files::source_file_content() const
{
    std::stringstream stream;
    write_includes(stream);
    write_platiform_independent_static_variables_to_source_file(stream);
    Whitespace_utils::write_newline(stream,2);
    stream << "#ifdef Q_OS_ANDROID";
    write_android_specific_includes(stream);
    write_android_specific_static_variables_to_source_file(stream);
    Whitespace_utils::write_newline(stream,2);
    write_native_methods_to_source_file(stream);
    Whitespace_utils::write_newline(stream,1);
    write_init_method_source_file(stream);
    Whitespace_utils::write_newline(stream,2);
    stream << "#endif //Q_OS_ANDROID";
    Whitespace_utils::write_newline(stream,3);
    stream << "// Platform independent methods";
    Whitespace_utils::write_newline(stream,3);
    write_constructor_method(stream);
    Whitespace_utils::write_newline(stream,2);
    write_instance_method(stream);
    Whitespace_utils::write_newline(stream,2);

    auto methods = get_other_qt_methods();
    for(std::vector<Qt_method>::size_type i =0; i < methods.size(); i++){
        auto method = methods.at(i);
        if(method.is_valid()){
            if(method.has_comment()){
                stream << method.comment();
            }else{
                stream << "/*";
                Whitespace_utils::write_newline(stream,1);
                stream << "* @brief " << method.class_name_ << "::"<< method.method_name();
                Whitespace_utils::write_newline(stream,1);
                stream << "*/";
            }
            Whitespace_utils::write_newline(stream,1);
            stream << method.definition();
            Whitespace_utils::write_newline(stream,2);
        }
    }
    return stream.str();
}

std::string Qt_class_files::header_file_content() const
{
    std::stringstream stream;
    stream << "#ifndef " << String_utils::to_upper(qt_class_.class_name()) << "_H";
    Whitespace_utils::write_newline(stream,1);
    stream << "#define " << String_utils::to_upper(qt_class_.class_name()) << "_H";
    Whitespace_utils::write_newline(stream,2);
    stream << "#include <QObject>";
    Whitespace_utils::write_newline(stream,2);
    stream << "#ifdef Q_OS_ANDROID";
    Whitespace_utils::write_newline(stream,1);
    stream << "#include \"" << native_common << ".h\";";
    Whitespace_utils::write_newline(stream,1);
    stream << "#endif";
    Whitespace_utils::write_newline(stream,2);
    stream << "class " << qt_class_.class_name()<< " : public QObject";
    Whitespace_utils::write_newline(stream,1);
    stream << "{";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,4);
    stream << "Q_OBJECT";
    Whitespace_utils::write_newline(stream,1);
    stream << "public:";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,4);
    stream << "static "<< qt_class_.class_name() <<"&";
    Whitespace_utils::write_space(stream,1);
    stream << "instance();";
    Whitespace_utils::write_newline(stream,1);
    stream << "private:";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,4);
    stream << qt_class_.class_name() <<"();";
    Whitespace_utils::write_newline(stream,2);
    stream << "#ifdef Q_OS_ANDROID";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,4);
    stream << "friend jint JNI_OnLoad(JavaVM* vm, void* reserved);";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,4);
    stream << "static int init(JNIEnv *env,RegisterNativeMethods registerNativeMethods);";
    Whitespace_utils::write_newline(stream,1);
    stream << "#endif";
    Whitespace_utils::write_newline(stream,2);
    stream << "};";
    Whitespace_utils::write_newline(stream,2);
    stream << "#endif //" << String_utils::to_upper(qt_class_.class_name()) << "_H";
    return stream.str();
}

void Qt_class_files::write_native_methods_to_source_file(std::stringstream &stream) const
{
    for(std::vector<C_native_method>::size_type i = 0; i< qt_class_.c_native_methods().size(); i++){
        stream << qt_class_.c_native_methods().at(i).to_string();
    }

}

void Qt_class_files::write_init_method_source_file(std::stringstream &stream, std::string table_name) const
{

    auto init_method = get_init_method();
    if(init_method.is_valid() && init_method.has_comment()){
        stream << init_method.comment();

    }else{
        stream << "/*";
        Whitespace_utils::write_newline(stream,1);
        stream << "* @brief " << qt_class_.class_name() << "::init is called ounce by JNI_OnLoad to register native methods";
        Whitespace_utils::write_newline(stream,1);
        stream << "*  of java class \""<< chaR('/').replaceIn(qt_class_.java_class_full_path_,".") << "\"";
        Whitespace_utils::write_newline(stream,1);
        stream << "* @param env - JNIEnv pointer";
        Whitespace_utils::write_newline(stream,1);
        stream << "* @param registerNativeMethods - RegisterNativeMethods function";
        Whitespace_utils::write_newline(stream,1);
        stream << "* @return an int that is either JNI_TRUE (if the native methods have been registered successfully) or JNI_FALSE";
        Whitespace_utils::write_newline(stream,1);
        stream << "*/";
    }

    Whitespace_utils::write_newline(stream,1);
    stream << "int " + qt_class_.class_name() + "::init(JNIEnv *env, RegisterNativeMethods registerNativeMethods)";
    Whitespace_utils::write_newline(stream,1);
    stream << "{";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,2);
    stream << method_table_string(table_name);
    Whitespace_utils::write_newline(stream,2);
    Whitespace_utils::write_space(stream,1);
    stream << "if (!registerNativeMethods(env, \""<< qt_class_.java_class_full_path_ << "\",";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_tab(stream,3);
    stream  << table_name << ", " << "sizeof(" << table_name << ") / sizeof(" << table_name <<"[0]))) {";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,5);
    stream << "return JNI_FALSE;";
    Whitespace_utils::write_newline(stream,1);
    Whitespace_utils::write_space(stream,3);
    stream << "}";
    Whitespace_utils::write_newline(stream,2);
    Whitespace_utils::write_space(stream,5);
    stream << "return JNI_TRUE;";
    Whitespace_utils::write_newline(stream,1);
    stream << "}";
}

void Qt_class_files::write_includes(std::stringstream &stream) const
{
    //declaration files
    stream << "#include \"" << qt_class_.file_prefix() + ".h" << "\";";
    Whitespace_utils::write_newline(stream,1);
    //platiform independent includes
    write_platiform_independent_includes(stream);
}

void Qt_class_files::write_platiform_independent_includes(std::stringstream &stream) const
{
    for(std::vector<std::string>::size_type i  = 0; i < qt_class_.platiform_independent_includes_.size() ; i++){
        auto header = string(qt_class_.file_prefix() + ".h");
        auto include = qt_class_.platiform_independent_includes_.at(i);
        if(!header.hasMatch(include)){//filter the source header file
            stream << include;
            Whitespace_utils::write_newline(stream,1);
        }
    }
}

void Qt_class_files::write_android_specific_includes(std::stringstream &stream) const
{
    if(!qt_class_.android_specific_includes_.empty()){
        Whitespace_utils::write_newline(stream,2);
    }else{
        return;
    }
    for(std::vector<std::string>::size_type i  = 0; i < qt_class_.android_specific_includes_.size() ; i++){
        auto header = string(qt_class_.file_prefix() + ".h");
        auto include = qt_class_.android_specific_includes_.at(i);
        if(!header.hasMatch(include)){//filter the source header file
            stream << include;
            Whitespace_utils::write_newline(stream,1);
        }
    }
}

void Qt_class_files::write_constructor_method(std::stringstream &stream, std::string code) const
{
    auto constructor_method = get_constructor_method();
    if(constructor_method.is_valid()){
        if(!constructor_method.has_comment()){
            stream << "/*";
            Whitespace_utils::write_newline(stream,1);
            stream << "* @brief " << qt_class_.class_name() << "::"<<qt_class_.class_name();
            Whitespace_utils::write_newline(stream,1);
            stream << "*/";
        }else{
            stream << constructor_method.comment();
        }
        Whitespace_utils::write_newline(stream,1);
        stream << constructor_method.definition();
    }else{
        stream << "/*";
        Whitespace_utils::write_newline(stream,1);
        stream << "* @brief " << qt_class_.class_name() << "::"<<qt_class_.class_name();
        Whitespace_utils::write_newline(stream,1);
        stream << "*/";
        Whitespace_utils::write_newline(stream,1);
        stream << qt_class_.class_name() << "::" << qt_class_.class_name()<< "()";
        Whitespace_utils::write_newline(stream,1);
        stream << "{";
        Whitespace_utils::write_newline(stream,1);
        stream << code;
        Whitespace_utils::write_newline(stream,1);
        stream << "}";
    }

}

void Qt_class_files::write_instance_method(std::stringstream &stream) const
{

    auto instance_method = get_instance_method();

    if(instance_method.is_valid()){
        if(instance_method.has_comment()){
            stream << instance_method.comment();
        }else{
            stream << "/*";
            Whitespace_utils::write_newline(stream,1);
            stream << "* @brief " << qt_class_.class_name() << "::instance";
            Whitespace_utils::write_newline(stream,1);
            stream << "*/";
        }
        Whitespace_utils::write_newline(stream,1);
        stream << instance_method.definition();
    }else{
        stream << "/*";
        Whitespace_utils::write_newline(stream,1);
        stream << "* @brief " << qt_class_.class_name() << "::instance";
        Whitespace_utils::write_newline(stream,1);
        stream << "*/";

        Whitespace_utils::write_newline(stream,1);
        stream << qt_class_.class_name();
        stream << "&" ;
        Whitespace_utils::write_space(stream,1);
        stream  << qt_class_.class_name() << "::instance()";
        Whitespace_utils::write_newline(stream,1);
        stream << "{";
        Whitespace_utils::write_newline(stream,1);
        Whitespace_utils::write_space(stream,4);
        stream << "static " << qt_class_.class_name();
        Whitespace_utils::write_space(stream,1);
        stream << "i_" << String_utils::to_lower(qt_class_.class_name()) << ";";
        Whitespace_utils::write_newline(stream,1);
        Whitespace_utils::write_space(stream,4);
        stream << "return";
        Whitespace_utils::write_space(stream,1);
        stream << "i_" << String_utils::to_lower(qt_class_.class_name()) << ";";
        Whitespace_utils::write_newline(stream,1);
        stream << "}";
    }

}

std::map<std::string,std::string> Qt_class_files::get_signature_map_from_content(std::string source_file_content){
    Parser set_signature_native_table_parser = chaR('{').seq(any(chaR(',')).plus().seq(chaR(',')))
            .seq(any(chaR(',')).plus());
    Parser entry_native_table_parser =  set_signature_native_table_parser.seq(chaR(',')).seq(any(chaR('}')).plus().seq(chaR('}'))).seq(chaR(',').optional());
    Parser open = string("JNINativeMethod").oR(string("auto"))
            .seq(space()).seq(any(chaR('[')).plus().seq(chaR('['))).seq(spaceOptional()).seq(chaR(']'))
            .seq(spaceOptional()).seq(chaR('=')).seq(spaceOptional()).seq(chaR('{'));
    Parser native_table_parser = open
            .seq(spaceOptional().seq(entry_native_table_parser).seq(spaceOptional()).star())
            .seq(spaceOptional()).seq(chaR('}')).seq(spaceOptional()).seq(chaR(';'));

    auto native_table = native_table_parser.firstStringMatch(source_file_content);
    std::map<std::string,std::string> name_signature_pair_map;
    entry_native_table_parser.replaceInMapped(open.removeFrom(native_table),[&set_signature_native_table_parser,&name_signature_pair_map](std::string entry)->std::string{
        auto pair = chaR('{').removeFrom(set_signature_native_table_parser.firstStringMatch(entry));
        auto quote = chaR('"').oR(chaR('\''));
        auto match =  chaR(',').firstMatch(pair);
        if(match.isSuccess()){
            C_native_method_parameter param;
            auto name =  String_utils::trim(quote.removeFrom(pair.substr(0,match.success().start())));
            auto signature  = String_utils::trim(quote.removeFrom(pair.substr(match.success().end(),pair.length() - match.success().end())));
            name_signature_pair_map[name] = signature;
        }

        return entry;
    });
    return name_signature_pair_map;
}

std::string Qt_class_files::get_comment(const std::string& function_with_comment,Parser&  fn_boundary ){
    auto fn_boundary_match = fn_boundary.firstMatch(function_with_comment);
    auto comment_match = comment().firstMatch(function_with_comment);

    std::string native_function_comment;
    if(comment_match.isSuccess() && fn_boundary_match.isSuccess()){
        if(comment_match.success().end() < fn_boundary_match.success().end()){
            return String_utils::trim(comment_match.success().value());
        }
    }
    return  "";
}

std::vector<C_native_method> Qt_class_files::get_native_methods_from_content(std::string source_file_content){
    std::vector<C_native_method> methods;

    const auto signatures_map = get_signature_map_from_content(source_file_content);
    auto native_funs = (comment().seq(spaceOptional()).optional()).seq(native_function_parser()).allMatches(source_file_content);

    for(std::vector<std::string>::size_type i =0;i< native_funs.size();i++){
        const  auto native_function_with_comment = native_funs.at(i).value();
        auto fn_boundary = jni_type_parser().seq(any(jni_env_pointer_parser()).plus())
                .seq(jni_env_pointer_parser())
                .seq(any(chaR(')')).plus()).seq(chaR(')')).seq(spaceOptional()).seq(chaR('{'));


        auto native_function_comment = get_comment(native_function_with_comment,fn_boundary);

        auto native_function = native_function_parser().firstStringMatch(native_function_with_comment);
        auto function_code =   (jni_type_parser().seq(any(jni_env_pointer_parser()).plus().seq(jni_env_pointer_parser()))
                                .seq(any(chaR(')')).plus().seq(chaR(')'))).seq(spaceOptional()).seq(chaR('{'))).removeFrom(String_utils::trim(native_function));

        auto tmp = chaR('}').lastMatch(function_code);
        if(tmp.isSuccess()){
            function_code = function_code.substr(0,tmp.success().start());
        }else{
            //error in code, delete it
            function_code = "";
        }

        Parser fun_declaration = jni_type_parser().seq(any(jni_env_pointer_parser()).plus().seq(jni_env_pointer_parser()))
                .seq(any(chaR(')')).plus().seq(chaR(')')));

        fun_declaration.replaceInMapped(native_function,[&methods,this,&signatures_map,&function_code,&native_function_comment](std::string native_function_declaration)->std::string{
            native_function_declaration = String_utils::trim(native_function_declaration);

            C_native_method method;
            //keep comment state
            method.comment_ = native_function_comment;
            //keep function code state
            method.function_code_ = function_code;

            method.use_jni_env_parameter_ =  (!string("JNIEnv").seq(spaceOptional()).seq(chaR('*')).seq(spaceOptional()).seq(chaR(',')).hasMatch(native_function_declaration));
            if(method.use_jni_env_parameter_){
                auto param =   string("JNIEnv").seq(spaceOptional()).seq(chaR('*')).seq(spaceOptional()).seq(any(chaR(',')).plus()).firstStringMatch(native_function_declaration);
                method.jni_env_parameter_ = String_utils::trim((string("JNIEnv").seq(spaceOptional()).seq(chaR('*')).seq(spaceOptional()) | chaR(',')).removeFrom(param));
            }

            method.use_class_or_object_parameter_ =  (!string("jclass").oR(string("jobject")).seq(spaceOptional()).seq(chaR(',') | chaR(')')).hasMatch(native_function_declaration));
            if(method.use_class_or_object_parameter_){
                auto param  = string("jclass").oR(string("jobject")).seq(spaceOptional()).seq((chaR('_') | word()).plus()).seq(chaR(',') | chaR(')')).firstStringMatch(native_function_declaration);
                method.class_or_object_parameter_ = String_utils::trim((string("jclass").oR(string("jobject")).seq(spaceOptional()) | chaR(',') | chaR(')')).removeFrom(param));
            }

            const auto return_type_match = jni_type_parser().firstMatch(native_function_declaration);
            if(return_type_match.isSuccess()){
                const auto return_type_start = return_type_match.success().start();
                const  auto return_type_end  = return_type_match.success().end();

                if(return_type_start == 0){
                    method.return_type_ = String_utils::trim(native_function_declaration.substr(0,return_type_end));
                    const auto name_match = chaR('(').firstMatch(native_function_declaration,return_type_end);
                    if(name_match.isSuccess()){
                        method.name_ = String_utils::trim(native_function_declaration.substr(return_type_end ,name_match.success().start() - return_type_end));
                        std::string parameter_str = chaR('(').oR(chaR(')')).removeFrom( native_function_declaration.substr(name_match.success().end(),native_function_declaration.length() - name_match.success().end()));
                        Parser native_parameter_signature_parameters_parser = (jni_env_pointer_parser().seq(any(chaR(',')).star()).seq(chaR(',')))
                                .seq(any(chaR(',') | lastChar()).star().seq(chaR(',') | lastChar()));

                        std::string native_parameter_signarure_parameters = native_parameter_signature_parameters_parser.firstStringMatch(parameter_str);

                        if(!native_parameter_signarure_parameters.empty()){

                            method.represent_static_java_native_ = string("jclass").hasMatch(native_parameter_signarure_parameters);
                            parameter_str =  String_utils::trim(native_parameter_signature_parameters_parser.removeFrom(parameter_str));

                            bool error = false;
                            auto  set_error = [&error]() -> void{ if(!error){ error = true; } };

                            const  std::map<std::string,std::string> empty_map;
                            auto do_mangling_internal = [&empty_map,&set_error](std::string jni_type) -> std::string{
                                auto java_type = to_java_type(jni_type);
                                if(java_type.empty()){
                                    set_error();
                                    return "";
                                }
                                auto result = do_mangling(java_type,empty_map,"package");
                                if(result.empty()){
                                    set_error();
                                    return "";
                                }else{
                                    return result;
                                }
                            };

                            if(!parameter_str.empty()){
                                std::vector<std::string> params;

                                if(chaR(',').hasMatch(parameter_str)){
                                    auto p = any(chaR(',') | lastChar()).plus().seq(chaR(',') | lastChar());
                                    auto tmp_list = p.allStringMatches(parameter_str);
                                    for(std::vector<std::string>::size_type i = 0; i < tmp_list.size(); i++){
                                        params.push_back(String_utils::trim(chaR(',').removeFrom(tmp_list.at(i))));
                                    }
                                }else{
                                    params.push_back(parameter_str);
                                }

                                for(std::vector<std::string>::size_type i = 0; i < params.size(); i++){
                                    auto param_str = String_utils::trim(params.at(i));
                                    if(!param_str.empty()){
                                        //using space to separate, there param_str must be a trimmed string
                                        auto match =  space().firstMatch(param_str);
                                        C_native_method_parameter param;
                                        if(match.isSuccess()){
                                            param.type_  = String_utils::trim(param_str.substr(0,match.success().start()));
                                            param.name_ = String_utils::trim(param_str.substr(match.success().end(),param_str.length() - match.success().end()));
                                        }else{
                                            param.type_  = String_utils::trim(param_str);
                                        }
                                        if(param.is_valid()){
                                            method.method_parameters_.push_back(param);
                                        }
                                    }

                                }

                                //set-up signatures
                                std::stringstream signature_stream;
                                signature_stream << "(";
                                for(std::vector<C_native_method_parameter>::size_type i = 0; i < method.method_parameters_.size(); i++){
                                    signature_stream << do_mangling_internal(method.method_parameters_.at(i).type());
                                }

                                signature_stream << ")";
                                signature_stream << do_mangling_internal(method.return_type_);
                                method.signature_ = signature_stream.str();

                                if(string("Lpackage/Object").hasMatch(method.signature_)){
                                    auto iter = signatures_map.find(method.name_);
                                    if(iter != signatures_map.end()){
                                        method.signature_ = iter->second;
                                    }
                                }

                            }else{
                                //set-up signatures
                                method.signature_ = "()" + do_mangling_internal(method.return_type_);
                                if(string("Lpackage/Object").hasMatch(method.signature_)){
                                    auto iter = signatures_map.find(method.name_);
                                    if(iter != signatures_map.end()){
                                        method.signature_ = iter->second;
                                    }
                                }
                            }

                            if(!error){
                                methods.push_back(method);
                            }
                        }
                    }
                }
            }

            return native_function_declaration;
        });

    }

    return  methods;
}

std::string Qt_class_files::read_source_file()
{
    return File::read(qt_class_.source_file_path());
}


bool Qt_class_files::write_header_file()
{
    const std::string& content = header_file_content();

    if(qt_class_.header_file_path().empty() || content.empty()){
        return false;
    }

    if(!header_file_exist()){
        return File::write(content,qt_class_.header_file_path());
    }else{
        return true;
    }
}

std::string Qt_class_files::read_header_file()
{
    return File::read(qt_class_.header_file_path());
}
