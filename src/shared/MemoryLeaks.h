/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MANGOSSERVER_MEMORY_H
#define MANGOSSERVER_MEMORY_H

#include "Platform/CompilerDefs.h"

#if COMPILER == COMPILER_MICROSOFT

#ifndef _WIN64
// Visual Leak Detector support enabled
//#include <vld/vld.h>
// standard Visual Studio leak check disabled,
//#  define _CRTDBG_MAP_ALLOC
//#  include <stdlib.h>
//#  include <crtdbg.h>
#else
//#  define _CRTDBG_MAP_ALLOC
//#  include <stdlib.h>
//#  include <crtdbg.h>
#endif

#endif


#include "Policies/Singleton.h"

struct MemoryManager : public MaNGOS::Singleton < MemoryManager >
{
    MemoryManager();
};
#endif
