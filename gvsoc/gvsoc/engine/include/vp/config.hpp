/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __VP_COMP_MODEL_CONFIG_HPP__
#define __VP_COMP_MODEL_CONFIG_HPP__

#include <string>
#include <vector>
#include <map>

#include "vp/jsmn.h"

using namespace std;

namespace vp {

  class config
  {

  public:

    virtual std::string get_str() { return ""; }
    virtual long long int get_int() { return 0; }
    virtual bool get_bool() { return false; }
    virtual config *get(std::string name) { return NULL; }
    virtual int get_nb_elem() { return 0; }
    virtual config *get_elem(int index) { return NULL; }
    virtual std::map<std::string, config *> get_childs() {
      return std::map<std::string, config *>();
    }
    virtual vp::config *get_from_list(std::vector<std::string> name_list) {
      return NULL;
    }
    config *create_config(jsmntok_t *tokens, int *_size);

  };

  class config_object : public config
  {

  public:
    config_object(jsmntok_t *tokens, int *size=NULL);

    config *get(std::string name);
    vp::config *get_from_list(std::vector<std::string> name_list);
    std::map<std::string, config *> get_childs() { return childs; }

  private:
    std::map<std::string, config *> childs;

  };

  class config_array : public config
  {

  public:
    config_array(jsmntok_t *tokens, int *size=NULL);
    vp::config *get_from_list(std::vector<std::string> name_list);

    int get_nb_elem() { return elems.size(); }
    config *get_elem(int index) {
      return elems[index]; }

  private:
    std::vector<config *> elems;
  };


  class config_string : public config
  {

  public:
    config_string(jsmntok_t *tokens);
    vp::config *get_from_list(std::vector<std::string> name_list);
    std::string get_str() { return value; }
    long long int get_int() { return strtoll(value.c_str(), NULL, 0); }

  private:
    std::string value;
  };


  class config_number : public config
  {

  public:
    config_number(jsmntok_t *tokens);
    long long int get_int() { return (int)value; }
    vp::config *get_from_list(std::vector<std::string> name_list);

  private:
    double value;

  };

  class config_bool : public config
  {

  public:
    config_bool(jsmntok_t *tokens);
    vp::config *get_from_list(std::vector<std::string> name_list);
    bool get_bool() { return (bool)value; }

  private:
    bool value;
  };

};  

#endif
