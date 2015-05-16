/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmDefinitions.h"

#include <assert.h>

//----------------------------------------------------------------------------
cmDefinitions::Def cmDefinitions::NoDef;

//----------------------------------------------------------------------------
cmDefinitions::Def const& cmDefinitions::GetInternal(
  const std::string& key, StackIter begin, StackIter end)
{
  assert(begin != end);
  MapType::const_iterator i = begin->Map.find(key);
  if (i != begin->Map.end())
    {
    return i->second;
    }
  StackIter it = begin;
  ++it;
  if (it == end)
    {
    return cmDefinitions::NoDef;
    }
  Def const& def = cmDefinitions::GetInternal(key, it, end);
  return begin->Map.insert(MapType::value_type(key, def)).first->second;
}

//----------------------------------------------------------------------------
const char* cmDefinitions::Get(const std::string& key,
    StackIter begin, StackIter end)
{
  Def const& def = cmDefinitions::GetInternal(key, begin, end);
  return def.Exists? def.c_str() : 0;
}

//----------------------------------------------------------------------------
void cmDefinitions::Set(const std::string& key, const char* value)
{
  Def def(value);
  this->Map[key] = def;
}

void cmDefinitions::Erase(const std::string& key)
{
  this->Map.erase(key);
}

//----------------------------------------------------------------------------
std::vector<std::string> cmDefinitions::LocalKeys() const
{
  std::vector<std::string> keys;
  keys.reserve(this->Map.size());
  // Consider local definitions.
  for(MapType::const_iterator mi = this->Map.begin();
      mi != this->Map.end(); ++mi)
    {
    if (mi->second.Exists)
      {
      keys.push_back(mi->first);
      }
    }
  return keys;
}

//----------------------------------------------------------------------------
cmDefinitions cmDefinitions::MakeClosure(StackConstIter begin,
                                         StackConstIter end)
{
  cmDefinitions closure;
  std::set<std::string> undefined;
  for (StackConstIter it = begin; it != end; ++it)
    {
    // Consider local definitions.
    for(MapType::const_iterator mi = it->Map.begin();
        mi != it->Map.end(); ++mi)
      {
      // Use this key if it is not already set or unset.
      if(closure.Map.find(mi->first) == closure.Map.end() &&
         undefined.find(mi->first) == undefined.end())
        {
        if(mi->second.Exists)
          {
          closure.Map.insert(*mi);
          }
        else
          {
          undefined.insert(mi->first);
          }
        }
      }
    }
  return closure;
}

//----------------------------------------------------------------------------
std::vector<std::string>
cmDefinitions::ClosureKeys(std::set<std::string>& bound) const
{
  std::vector<std::string> defined;
  defined.reserve(this->Map.size());
  for(MapType::const_iterator mi = this->Map.begin();
      mi != this->Map.end(); ++mi)
    {
    // Use this key if it is not already set or unset.
    if(bound.insert(mi->first).second && mi->second.Exists)
      {
      defined.push_back(mi->first);
      }
    }
  return defined;
}
