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
cmDefinitions::cmDefinitions(cmDefinitions* parent)
  : Up(parent)
{
}

//----------------------------------------------------------------------------
std::pair<const char*, bool> cmDefinitions::Get(const std::string& key)
{
  MapType::const_iterator i = this->Map.find(key);
  std::pair<const char*, bool> result(0, false);
  if(i != this->Map.end())
    {
    result = std::make_pair(i->second.Exists ? i->second.c_str() : 0, true);
    }
  return result;
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
std::set<std::string> cmDefinitions::LocalKeys() const
{
  std::set<std::string> keys;
  // Consider local definitions.
  for(MapType::const_iterator mi = this->Map.begin();
      mi != this->Map.end(); ++mi)
    {
    if (mi->second.Exists)
      {
      keys.insert(mi->first);
      }
    }
  return keys;
}

//----------------------------------------------------------------------------
cmDefinitions cmDefinitions::MakeClosure() const
{
  std::set<std::string> undefined;
  cmDefinitions closure;
  closure.MakeClosure(undefined, this);
  return closure;
}

//----------------------------------------------------------------------------
void cmDefinitions::MakeClosure(std::set<std::string>& undefined,
                                cmDefinitions const* defs)
{
  std::vector<cmDefinitions*> ups;
  while(defs)
    {
    ups.push_back(defs);
    defs = defs->Up;
    }
  for (std::vector<cmDefinitions*>::const_iterator it = ups.begin();
       it != ups.end(); ++it)
    {
    // Consider local definitions.
    for(MapType::const_iterator mi = (*it)->Map.begin();
        mi != (*it)->Map.end(); ++mi)
      {
      // Use this key if it is not already set or unset.
      if(this->Map.find(mi->first) == this->Map.end() &&
         undefined.find(mi->first) == undefined.end())
        {
        if(mi->second.Exists)
          {
          this->Map.insert(*mi);
          }
        else
          {
          undefined.insert(mi->first);
          }
        }
      }
    }
}

//----------------------------------------------------------------------------
std::vector<std::string>
cmDefinitions::Keys(std::vector<std::string>& undefined) const
{
  std::vector<std::string> defined;
  defined.reserve(this->Map.size());
  for(MapType::const_iterator mi = this->Map.begin();
      mi != this->Map.end(); ++mi)
    {
    std::vector<std::string>& m = mi->second.Exists? defined : undefined;
    m.push_back(mi->first);
    }
  undefined.erase(std::unique(undefined.begin(), undefined.end()),
                  undefined.end());
  return defined;
}
