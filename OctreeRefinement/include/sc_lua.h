/*
  This file is part of the SC Library.
  The SC Library provides support for parallel scientific applications.

  Copyright (C) 2010 The University of Texas System
  Additional copyright (C) 2011 individual authors

  The SC Library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  The SC Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with the SC Library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#ifndef SC_LUA_H
#define SC_LUA_H

#include <sc.h>

#ifdef SC_HAVE_LUA

SC_EXTERN_C_BEGIN;

#ifdef SC_HAVE_LUA5_2_LUA_H
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
#else
#ifdef SC_HAVE_LUA5_1_LUA_H
#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <lua5.1/lauxlib.h>
#else
#ifdef SC_HAVE_LUA_H
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#endif
#endif
#endif

SC_EXTERN_C_END;

#else
#error "We did not find a recent lua library that provides lua_createtable."
#endif

#endif /* !SC_LUA_H */
