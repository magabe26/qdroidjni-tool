/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#include <iostream>
#include "configuration_file.h"

using namespace std;

int main(int argc, char *argv[])
{

      if(argc < 2){
          std::cout << "Usage: " << "qjnidroid-tool " << "<config-file>" << "\n\n";
          return 1;
      }


    auto i_group = Configuration_file::parse(argv[1]);
    for(std::vector<Config_instruction_group>::size_type i = 0; i<i_group.size(); i++){
        i_group.at(i).execute();
    }

    return 0;
}


