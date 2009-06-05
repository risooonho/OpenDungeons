#include "stdio.h"
#include <string>
using namespace std;

#include "Defines.h"
#include "Socket.h"
#include "ExampleFrameListener.h"
#include "Network.h"
#include "ChatMessage.h"

/*! \brief A thread function which runs on the client to handle communications with the server.
 *
 * A single instance of this thread is spawned by the client when it connects
 * to a server.  The socket connection itself is established before this thread
 * executes and the CSPStruct is used to pass this spawned socket instance, as
 * well as a pointer to the instance of the ExampleFrameListener being used by
 * the game.
 */
void *clientSocketProcessor(void *p)
{
	string tempString;
	string serverCommand, arguments;
	Socket *sock = ((CSPStruct*)p)->nSocket;
	ExampleFrameListener *frameListener = ((CSPStruct*)p)->nFrameListener;


	// Send a hello request to start the conversation with the server
	sem_wait(&sock->semaphore);
	sock->send(formatCommand("hello", (string)"OpenDungeons V " + VERSION));
	sem_post(&sock->semaphore);
	while(sock->is_valid())
	{
		string commandFromServer = "";
		bool packetComplete;

		// Loop until we get to a place that ends in a '>' symbol
		// indicating that we have 1 or more FULL messages so we
		// don't break in the middle of a message.
		packetComplete = false;
		while(!packetComplete)
		{
			int charsRead = sock->recv(tempString);
			// If the server closed the connection
			if(charsRead <= 0)
			{
				return NULL;
			}

			commandFromServer += tempString;
			if(commandFromServer[commandFromServer.length()-1] == '>')
			{
				packetComplete = true;
			}
		}

		bool parseReturnValue = parseCommand(commandFromServer, serverCommand, arguments);
		do
		{

			if(serverCommand.compare("picknick") == 0)
			{
				sem_wait(&sock->semaphore);
				sock->send(formatCommand("setnick", me->nick));
				sem_post(&sock->semaphore);
			}

			else if(serverCommand.compare("chat") == 0)
			{
				ChatMessage *newMessage = processChatMessage(arguments);
				frameListener->chatMessages.push_back(newMessage);
			}

			else if(serverCommand.compare("newmap") == 0)
			{
				gameMap.clearAll();
			}

			else if(serverCommand.compare("turnsPerSecond") == 0)
			{
				turnsPerSecond = atof(arguments.c_str());
			}

			else if(serverCommand.compare("addtile") == 0)
			{
				stringstream tempSS(arguments);
				Tile *newTile = new Tile;
				tempSS >> newTile;
				gameMap.addTile(newTile);
				newTile->createMesh();
				sem_wait(&sock->semaphore);
				sock->send(formatCommand("ok", "addtile"));
				sem_post(&sock->semaphore);

				// Loop over the tile's neighbors to force them to recheck
				// their mesh to see if they can use an optimized one
				vector<Tile*> neighbors = gameMap.neighborTiles(newTile->x, newTile->y);
				for(unsigned int i = 0; i < neighbors.size(); i++)
				{
					neighbors[i]->setFullness(neighbors[i]->getFullness());
				}
			}

			else if(serverCommand.compare("addclass") == 0)
			{
				//NOTE: This code is duplicated in readGameMapFromFile defined in src/Functions.cpp
				// Changes to this code should be reflected in that code as well
				double tempX, tempY, tempZ, tempSightRadius, tempDigRate;
				int tempHP, tempMana;
				stringstream tempSS;
				string tempString2;

				tempSS.str(arguments);

				tempSS >> tempString >> tempString2 >> tempX >> tempY >> tempZ;
				tempSS >> tempHP >> tempMana;
				tempSS >> tempSightRadius >> tempDigRate;

				Creature *p = new Creature(tempString, tempString2, Ogre::Vector3(tempX, tempY, tempZ), tempHP, tempMana, tempSightRadius, tempDigRate);
				gameMap.addClassDescription(p);
				sem_wait(&sock->semaphore);
				sock->send(formatCommand("ok", "addclass"));
				sem_post(&sock->semaphore);
			}

			else if(serverCommand.compare("addcreature") == 0)
			{
				//NOTE: This code is duplicated in readGameMapFromFile defined in src/Functions.cpp
				// Changes to this code should be reflected in that code as well
				Creature *newCreature = new Creature;

				stringstream tempSS;
				tempSS.str(arguments);
				tempSS >> newCreature;

				gameMap.addCreature(newCreature);
				newCreature->createMesh();
				sem_wait(&sock->semaphore);
				sock->send(formatCommand("ok", "addcreature"));
				sem_post(&sock->semaphore);
			}

			else if(serverCommand.compare("newturn") == 0)
			{
				stringstream tempSS;
				tempSS.str(arguments);
				tempSS >> turnNumber;
			}

			else if(serverCommand.compare("creatureAddDestination") == 0)
			{
				char array[255];

				stringstream tempSS;
				tempSS.str(arguments);

				tempSS.getline(array, sizeof(array), ':');
				//tempString = array;
				Creature *tempCreature = gameMap.getCreature(array);

				double tempX, tempY, tempZ;
				tempSS.getline(array, sizeof(array), ':');
				tempX = atof(array);
				tempSS.getline(array, sizeof(array), ':');
				tempY = atof(array);
				tempSS.getline(array, sizeof(array));
				tempZ = atof(array);

				Ogre::Vector3 tempVector(tempX, tempY, tempZ);

				if(tempCreature != NULL)
				{
					cout << endl << tempCreature->name << tempX << ",  " << tempY << ",  " << tempZ << endl;

					tempCreature->addDestination(tempVector.x, tempVector.y);
				}
			}

			else if(serverCommand.compare("tileFullnessChange") == 0)
			{
				char array[255];
				double tempFullness, tempX, tempY;
				stringstream tempSS;

				tempSS.str(arguments);
				tempSS.getline(array, sizeof(array), ':');
				tempFullness = atof(array);
				tempSS.getline(array, sizeof(array), ':');
				tempX = atof(array);
				tempSS.getline(array, sizeof(array));
				tempY = atof(array);

				Tile *tempTile = gameMap.getTile(tempX, tempY);
				if(tempTile != NULL)
				{
					cout << "\nSetting tile fullness for tile " << tempX << ", " << tempY << " to " << tempFullness << "\n";
					tempTile->setFullness(tempFullness);
				}
				else
				{
					cout << "\nERROR:  Server told us to set the fullness for a nonexistent tile.\n";
				}
			}

			else
			{
				cout << "\n\n\nERROR:  Unknown server command!\nCommand:";
				cout << serverCommand << "\nArguments:" << arguments << "\n\n";
			}

			parseReturnValue = parseCommand(commandFromServer, serverCommand, arguments);
		}while(parseReturnValue);
	}

	// Return something to make the compiler happy
	return NULL;
}

