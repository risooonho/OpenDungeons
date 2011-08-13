#ifndef RENDERREQUEST_H
#define RENDERREQUEST_H

#include <string>
#include <OgreVector3.h>
#include <OgreQuaternion.h>

/*! \brief A data structure to be used for requesting that the OGRE rendering thread perform certain tasks.
 *
 *  This data structure is used filled out with a request and then placed in
 *  the global renderQueue.  The requests are taken out of the queue and
 *  processed by the frameStarted event in the ExampleFrameListener class. 
 */
class RenderRequest
{
    public:
        enum RequestType
        {
            createTile,
            refreshTile,
            destroyTile,
            deleteTile,
            createCreature,
            destroyCreature,
            deleteCreature,
            setObjectAnimationState,
            createCreatureVisualDebug,
            destroyCreatureVisualDebug,
            createWeapon,
            destroyWeapon,
            deleteWeapon,
            pickUpCreature,
            dropCreature,
            rotateCreaturesInHand,
            createRoom,
            destroyRoom,
            deleteRoom,
            createRoomObject,
            destroyRoomObject,
            deleteRoomObject,
            createTrap,
            destroyTrap,
            deleteTrap,
            createTreasuryIndicator,
            destroyTreasuryIndicator,
            createMapLight,
            updateMapLight,
            destroyMapLight,
            destroyMapLightVisualIndicator,
            deleteMapLight,
            createField,
            refreshField,
            destroyField,
            moveSceneNode,
            orientSceneNodeToward,
            reorientSceneNode,
            scaleSceneNode,
            createMissileObject,
            destroyMissileObject,
            deleteMissileObject, //setMissileObjectAnimationState,
            noRequest
        };

        RenderRequest();

        long int turnNumber;
        RequestType type;
        //TODO - can we use a union here instead?
        void *p;
        void *p2;
        void *p3;
        std::string str;
        Ogre::Vector3 vec;
        Ogre::Quaternion quaternion;
        bool b;
        //TODO:  Add a pointer called destroyMe which is used to pass a void pointer which should be deleted after it is used, this can replace the need for str and vec.
};

#endif
