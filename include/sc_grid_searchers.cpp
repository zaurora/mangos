/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"

//return closest GO in grid, with range from pSource
GameObject* GetClosestGameObjectWithEntry(WorldObject* pSource, uint32 uiEntry, float fRange)
{
    GameObject* pGo = NULL;

    CellPair pair(MaNGOS::ComputeCellPair(pSource->GetPositionX(), pSource->GetPositionY()));
    Cell cell(pair);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    MaNGOS::NearestGameObjectEntryInObjectRangeCheck go_check(*pSource, uiEntry, fRange);
    MaNGOS::GameObjectLastSearcher<MaNGOS::NearestGameObjectEntryInObjectRangeCheck> searcher(pSource, pGo, go_check);

    TypeContainerVisitor<MaNGOS::GameObjectLastSearcher<MaNGOS::NearestGameObjectEntryInObjectRangeCheck>, GridTypeMapContainer> go_searcher(searcher);

    CellLock<GridReadGuard> cell_lock(cell, pair);
    cell_lock->Visit(cell_lock, go_searcher,*(pSource->GetMap()));

    return pGo;
}

//return closest creature alive in grid, with range from pSource
Creature* GetClosestCreatureWithEntry(WorldObject* pSource, uint32 uiEntry, float fRange)
{
    Creature* pCreature = NULL;

    CellPair pair(MaNGOS::ComputeCellPair(pSource->GetPositionX(), pSource->GetPositionY()));
    Cell cell(pair);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck creature_check(*pSource, uiEntry, true, fRange);
    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pSource, pCreature, creature_check);

    TypeContainerVisitor<MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck>, GridTypeMapContainer> creature_searcher(searcher);

    CellLock<GridReadGuard> cell_lock(cell, pair);
    cell_lock->Visit(cell_lock, creature_searcher,*(pSource->GetMap()));

    return pCreature;
}
