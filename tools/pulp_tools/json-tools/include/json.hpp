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

#ifndef __JSON_HPP__
#define __JSON_HPP__

#include "jsmn.h"
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <map>
#include "string.h"

namespace js {

  class config
  {

  public:

    virtual std::string get_str() { return ""; }
    virtual long long int get_int() { return 0; }
    virtual long long int get_int(std::string name)
    {
      js::config *config = this->get(name);
      if (config == NULL) return 0;
      return config->get_int();
    }

    virtual void dump(std::string indent="");
    virtual config *get(std::string) { return NULL; }
    virtual config *get_elem(int) { return NULL; }
    virtual size_t get_size() { return 0; }
    virtual bool get_bool() { return false; }
    virtual config *get_from_list(std::vector<std::string>) {
      return NULL;
    }

    virtual int get_child_int(std::string) { return 0; }
    virtual bool get_child_bool(std::string) { return false; }
    virtual std::string get_child_str(std::string) { return ""; }

    virtual std::vector<config *> get_elems() {
      return std::vector<config *> ();
    }
    virtual std::map<std::string, config *> get_childs() {
      return std::map<std::string, config *>();
    }
    config *create_config(jsmntok_t *tokens, int *_size);

    std::map<std::string, config *> childs;
  };

  class config_object : public config
  {

  public:
    config_object(jsmntok_t *tokens, int *size=NULL);

    config *get(std::string name);
    config *get_from_list(std::vector<std::string> name_list);
    std::map<std::string, config *> get_childs() { return childs; }

    int get_child_int(std::string name);
    bool get_child_bool(std::string name);
    std::string get_child_str(std::string name);

    void dump(std::string indent="");

  };

  class config_array : public config
  {

  public:
    config_array(jsmntok_t *tokens, int *size=NULL);
    config *get_from_list(std::vector<std::string> name_list);

    std::vector<config *> get_elems() { return elems; }
    config *get_elem(int index) { return elems[index]; }

    size_t get_size() { return elems.size(); }

    void dump(std::string indent="");

  private:
    std::vector<config *> elems;
  };


  class config_string : public config
  {

  public:
    config_string(jsmntok_t *tokens);
    config *get_from_list(std::vector<std::string> name_list);
    std::string get_str() { return value; }
    long long int get_int() { return strtoll(value.c_str(), NULL, 0); }
    bool get_bool() { return strcmp(value.c_str(), "True") == 0 ||  strcmp(value.c_str(), "true") == 0; }

    void dump(std::string indent="");

  private:
    std::string value;
  };


  class config_number : public config
  {

  public:
    config_number(jsmntok_t *tokens);
    long long int get_int() { return (int)value; }
    config *get_from_list(std::vector<std::string> name_list);

    void dump(std::string indent="");

  private:
    double value;

  };

  class config_bool : public config
  {

  public:
    config_bool(jsmntok_t *tokens);
    bool get_bool() { return (bool)value; }
    config *get_from_list(std::vector<std::string> name_list);

    void dump(std::string indent="");

  private:
    bool value;
  };

  config *import_config_from_string(std::string config_string);

  config *import_config_from_file(std::string config_path);

}

#endif
