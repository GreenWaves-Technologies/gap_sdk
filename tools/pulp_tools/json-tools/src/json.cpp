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

#include "json.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <stdio.h>
#include "string.h"
#include <streambuf>
#include <stdlib.h>

std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

js::config *js::config::create_config(jsmntok_t *tokens, int *_size)
{
  jsmntok_t *current = tokens;
  config *config = NULL;

  switch (current->type)
  {
    case JSMN_PRIMITIVE:
      if (strcmp(current->str, "True") == 0 || strcmp(current->str, "False") == 0 ||
        strcmp(current->str, "true") == 0 || strcmp(current->str, "false") == 0)
      {
        config = new config_bool(current);
      }
      else
      {
        config = new config_number(current);
      }
      current++;
      break;

    case JSMN_OBJECT: {
      int size;
      config = new config_object(current, &size);
      current += size;
      break;
    }

    case JSMN_ARRAY: {
      int size;
      config = new config_array(current, &size);
      current += size;
      break;
    }

    case JSMN_STRING:
      config = new config_string(current);
      current++;
      break;

    case JSMN_UNDEFINED:
      break;
  }

  if (_size) {
    *_size = current - tokens;
  }

  return config;
}


void js::config::dump(std::string)
{
}

js::config *js::config_string::get_from_list(std::vector<std::string> name_list)
{
  if (name_list.size() == 0) return this;
  return NULL;
}

void js::config_string::dump(std::string)
{
  fprintf(stderr, "\"%s\"", this->value.c_str());
}

js::config *js::config_number::get_from_list(std::vector<std::string> name_list)
{
  if (name_list.size() == 0) return this;
  return NULL;
}

void js::config_number::dump(std::string)
{
  fprintf(stderr, "\"%f\"", this->value);
}

js::config *js::config_array::get_from_list(std::vector<std::string> name_list)
{
  if (name_list.size() == 0) return this;
  return NULL;
}

void js::config_array::dump(std::string indent)
{
  bool is_first = true;
  fprintf(stderr, "[\n");
  for (auto x: this->elems)
  {
    if (!is_first)
      fprintf(stderr, ",\n");
    x->dump(indent + "  ");
    is_first = false;
  }
  fprintf(stderr, "\n%s]\n", indent.c_str());
}

js::config *js::config_bool::get_from_list(std::vector<std::string> name_list)
{
  if (name_list.size() == 0) return this;
  return NULL;
}

void js::config_bool::dump(std::string)
{
  fprintf(stderr, "\"%s\"", this->value ? "true" : "false");
}

void js::config_object::dump(std::string indent)
{
  bool is_first = true;

  fprintf(stderr, "{\n");
  for (auto x: this->childs)
  {
    if (!is_first)
      fprintf(stderr, ",\n");
    fprintf(stderr, "%s  \"%s\": ", indent.c_str(), x.first.c_str());
    x.second->dump(indent + "  ");
    is_first = false;
  }
  fprintf(stderr, "\n%s}\n", indent.c_str());
}

js::config *js::config_object::get_from_list(std::vector<std::string> name_list)
{
  if (name_list.size() == 0) return this;

  js::config *result = NULL;
  std::string name;
  int name_pos = 0;

  for (auto& x: name_list) {
    if (x != "*" && x != "**")
    {
      name = x;
      break;
    }
    name_pos++;
  }

  for (auto& x: childs) {

    if (name == x.first)
    {
      result = x.second->get_from_list(std::vector<std::string>(name_list.begin () + name_pos + 1, name_list.begin () + name_list.size()));
      if (name_pos == 0 || result != NULL) return result;

    }
    else if (name_list[0] == "*")
    {
      result = x.second->get_from_list(std::vector<std::string>(name_list.begin () + 1, name_list.begin () + name_list.size()));
      if (result != NULL) return result;
    }
    else if (name_list[0] == "**")
    {
      result = x.second->get_from_list(name_list);
      if (result != NULL) return result;
    }
  }

  return result;
}

js::config *js::config_object::get(std::string name)
{
  return get_from_list(split(name, '/'));
}

js::config_string::config_string(jsmntok_t *tokens)
{
  value = std::string(tokens->str);
}

js::config_number::config_number(jsmntok_t *tokens)
{
  value = atof(tokens->str);
}

js::config_bool::config_bool(jsmntok_t *tokens)
{
  value = strcmp(tokens->str, "True") == 0 || strcmp(tokens->str, "true") == 0;
}

js::config_array::config_array(jsmntok_t *tokens, int *_size)
{
  jsmntok_t *current = tokens;
  jsmntok_t *top = current++;
  
  for (int i=0; i<top->size; i++)
  {
    int child_size;
    elems.push_back(create_config(current, &child_size));
    current += child_size;
  }


  if (_size) {
    *_size = current - tokens;
  }
}

js::config_object::config_object(jsmntok_t *tokens, int *_size)
{
  jsmntok_t *current = tokens;
  jsmntok_t *t = current++;

  for (int i=0; i<t->size; i++)
  {
    jsmntok_t *child_name = current++;
    int child_size;
    config *child_config = create_config(current, &child_size);
    current += child_size;

    if (child_config != NULL)
    {
      childs[child_name->str] = child_config;

    }
  }

  if (_size) {
    *_size = current - tokens;
  }
}

js::config *js::import_config_from_file(std::string config_path)
{
  std::ifstream t(config_path);
  std::string str((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());
  return import_config_from_string(str);

}

js::config *js::import_config_from_string(std::string config_str)
{
  const char *config_string = strdup(config_str.c_str());
  jsmn_parser parser;
  
  jsmn_init(&parser);
  int nb_tokens = jsmn_parse(&parser, config_string, strlen(config_string), NULL, 0);

  std::vector<jsmntok_t> tokens(nb_tokens);

  jsmn_init(&parser);
  nb_tokens = jsmn_parse(&parser, config_string, strlen(config_string), &(tokens[0]), nb_tokens);

  char *str = strdup(config_string);
  for (int i=0; i<nb_tokens; i++)
  {
    jsmntok_t *tok = &(tokens[i]);
    tok->str = &str[tok->start];
    str[tok->end] = 0;
      //printf("%d %d %d %d: %s\n", tokens[i].type, tokens[i].start, tokens[i].end, tokens[i].size, tok->str);
  }



  return new js::config_object(&(tokens[0]));
}

int js::config_object::get_child_int(std::string name)
{
  js::config *config = this->get(name);
  if (config)
    return config->get_int();
  else
    return 0;
}

bool js::config_object::get_child_bool(std::string name)
{
  js::config *config = this->get(name);
  if (config)
    return config->get_bool();
  else
    return 0;
}

std::string js::config_object::get_child_str(std::string name)
{
  js::config *config = this->get(name);
  if (config)
    return config->get_str();
  else
    return "";
}
