/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
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
