/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#include "function_parser.h"
#include "debug.h"
#include "string_utils.h"

Function_parser::Function_parser(std::shared_ptr<DelegateInterface> fn_declaration)
    :Parser("fn_parser",fn_declaration){}

std::shared_ptr<DelegateInterface> Function_parser::makeShared(){
    return std::shared_ptr<DelegateInterface>(new Function_parser(d1_),&DelegateInterface::deleter);
}

Result Function_parser::parseOn(Context context){
    const  auto buffer = context.buffer();
    const auto pos = context.position();
    if(pos < buffer.length()){
        const auto fn_declaration_match =  d1_->parse(buffer,pos);
        if(fn_declaration_match.isSuccess()){
            const auto start = fn_declaration_match.success().start();
            const auto function_scope_begin_match = spaceOptional().seq(chaR('{')).parse(buffer,fn_declaration_match.success().end());
            if(function_scope_begin_match.isFailure()){
                return Failure(function_scope_begin_match.failure().position());
            }else{
                auto list=  chaR('{').oR(chaR('}')).allMatches(buffer,function_scope_begin_match.success().end());
                if(list.empty()){
                    return Failure(function_scope_begin_match.success().end());
                }

                int stack = 1;//the 1 verified by function_scope_begin_match
                std::vector<Success>::size_type  end = 0;
                for(std::vector<Success>::size_type i = 0; i < list.size() ; i++){
                    const auto match = list.at(i);
                    if(chaR('{').hasMatch(match.value())){
                        stack++;
                    }else{
                        stack --;
                    }
                    end = match.end();

                    if(stack == 0){
                        return Success(start,end);
                    }
                }
                return Failure(end);
            }
        }else{
            return Failure(fn_declaration_match.failure().position());
        }

    }else{
        return Failure(buffer.length());
    }
}


Parser fn_declaration(std::string class_name){
    auto pointer_or_ref = (chaR('*') | chaR('&'));
    auto name = (letter() | chaR('_')).seq((chaR('-') | chaR('_') | word()).star());
    auto return_type = (name.seq(pointer_or_ref) | name);
    auto class_scope_name = string(class_name).seq(string("::")).seq((chaR('~').seq(string(class_name))) | name);

    auto with_return_type = return_type.seq(space()).seq(class_scope_name)
            .seq(spaceOptional()).seq(chaR('('))
            .seq(any(chaR(')'),")").star().seq(chaR(')')));

    auto with_no_return_type = spaceOptional().seq( class_scope_name)
            .seq(spaceOptional()).seq(chaR('('))
            .seq(any(chaR(')'),")").star().seq(chaR(')')));

    return  (with_return_type | with_no_return_type);
}

Function_parser fn(std::string class_name)
{
    Parser dec = fn_declaration(class_name);
    return Function_parser(const_cast<Parser&>(dec).makeShared());
}

Function_parser fn(const Parser &fn_declaration){
    return Function_parser(const_cast<Parser&>(fn_declaration).makeShared());
}
