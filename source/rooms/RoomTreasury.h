/*
 *  Copyright (C) 2011-2014  OpenDungeons Team
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ROOMTREASURY_H
#define ROOMTREASURY_H

#include "rooms/Room.h"

class RoomTreasury: public Room
{
    friend class ODClient;
public:
    RoomTreasury(GameMap* gameMap);

    virtual RoomType getType() const
    { return RoomType::treasury; }

    // Functions overriding virtual functions in the Room base class.
    void absorbRoom(Room *r);
    void addCoveredTile(Tile* t, double nHP, bool isRoomAbsorb);
    bool removeCoveredTile(Tile* t, bool isRoomAbsorb);

    // Functions specific to this class.
    int getTotalGold();
    int emptyStorageSpace();
    int depositGold(int gold, Tile *tile);
    int withdrawGold(int gold);
    virtual void doUpkeep();

protected:
    // Because treasury do not use active spots, we don't want the default
    // behaviour (removing the active spot tile) as it could result in removing an
    // unwanted treasury
    void notifyActiveSpotRemoved(ActiveSpotPlace place, Tile* tile)
    {}

private:
    //! \brief Tells which mesh is used to show how much the tile is full of gold.
    enum TreasuryTileFullness
    {
        noGold = 0, quarter, half, threeQuarters, fullOfGold
    };

    static TreasuryTileFullness getTreasuryTileFullness(int gold);
    const char* getMeshNameForTreasuryTileFullness(TreasuryTileFullness fullness);

    void updateMeshesForTile(Tile *t);

    std::map<Tile*, int> mGoldInTile;
    std::map<Tile*, TreasuryTileFullness> mFullnessOfTile;
    bool mGoldChanged;
};

#endif // ROOMTREASURY_H