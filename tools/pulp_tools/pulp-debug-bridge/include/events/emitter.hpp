// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#ifndef __EMITTER_H
#define __EMITTER_H

#include <list>
#include <algorithm>
#include <functional>

class EmitterRef {
public:
    virtual ~EmitterRef() {};
};

#define SMART_EMITTER(__type, __func)                                   \
template<typename ...paramTs>                                           \
class __type##Emitter {                                                 \
public:                                                                 \
    using EmmiterFunc = std::function<void(paramTs...)>;                \
    class EmitterRec : public EmitterRef {                              \
        friend __type##Emitter<paramTs...>;                             \
    public:                                                             \
        EmitterRec(EmmiterFunc func, bool once) :                       \
            m_func(func), m_once(once), m_deleted(false) {}             \
        bool operator==(const EmitterRec& a) const {                    \
            return &a == this;                                          \
        }                                                               \
        void set_deleted() { m_deleted = true; }                        \
                                                                        \
    private:                                                            \
        EmmiterFunc m_func;                                             \
        bool m_once;                                                    \
        bool m_deleted;                                                 \
    };                                                                  \
                                                                        \
    virtual ~__type##Emitter() {}                                       \
                                                                        \
    void emit_##__func(paramTs ...params) {                             \
        auto l=m_listeners.begin();                                     \
        while (l != m_listeners.end()) {                                \
            if (l->m_deleted) {                                         \
                l = m_listeners.erase(l);                               \
            } else {                                                    \
                if (l->m_once) {                                        \
                    l->m_deleted = true;                                \
                }                                                       \
                l->m_func(params...);                                   \
                l++;                                                    \
            }                                                           \
        }                                                               \
    }                                                                   \
                                                                        \
    void clear_##__func(const EmitterRef &rec) {                        \
        auto el = std::find(m_listeners.begin(),                        \
            m_listeners.end(), dynamic_cast<const EmitterRec&>(rec));   \
        if (el != m_listeners.end()) {                                  \
            el->set_deleted();                                          \
        }                                                               \
    }                                                                   \
                                                                        \
    void clearall_##__func() {                                          \
        auto l=m_listeners.begin();                                     \
        while (l != m_listeners.end()) {                                \
            l++->set_deleted();                                         \
        }                                                               \
    }                                                                   \
                                                                        \
    EmitterRef& on_##__func(const EmmiterFunc &func) {                  \
        m_listeners.emplace_back(std::move(func), false);               \
        return static_cast<EmitterRef&>(m_listeners.back());            \
    }                                                                   \
                                                                        \
    EmitterRef& once_##__func(const EmmiterFunc &func) {                \
        m_listeners.emplace_back(std::move(func), true);                \
        return static_cast<EmitterRef&>(m_listeners.back());            \
    }                                                                   \
                                                                        \
    int __func##_listener_count() {                                     \
        auto l=m_listeners.begin();                                     \
        int count = 0;                                                  \
        while (l != m_listeners.end()) {                                \
            if (!l->m_deleted) {                                        \
                count++;                                                \
            }                                                           \
            l++;                                                        \
        }                                                               \
        return count;                                                   \
    }                                                                   \
                                                                        \
private:                                                                \
    std::list<EmitterRec> m_listeners;                                  \
};                                                                      \

#endif