#include "chunkmanager.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range2d.h"

ChunkManager::ChunkManager()
{
  tbb::task_scheduler_init init; // Initializing the the tbb task scheduler
                                 // using the automatic thread count
  pCameraPosition = new Vector3(-(16 * (MAX_CHUNKS_INDEX_X / 2)) - 1,
                                -(16 * (MAX_CHUNKS_INDEX_Y / 2)) - 1,
                                -(16 * (MAX_CHUNKS_INDEX_Z / 2)) - 1);
                                    // Initializing the camera to an invalid
                                    // position to guarantee the Visibility
                                    // list is generated on entering the world
  pCameraRotation = new Vector3();
}

void ChunkManager::Update(float /*deltaTime*/,
                          Vector3 cameraPosition,
                          Vector3 cameraRotation)
{
  // Code to designate which chunks should be processed below.
    int numChunksLoaded = 0;
  
	ChunkList::accessor aChunk;

    Vector3 cChunk = translateCameraPositionToChunk(
		cameraPosition); // Getting which chunk the camera is currently in
	
	tbb::parallel_for(tbb::blocked_range2d<int,int>(cChunk.x - MAX_VIEW_DISTANCE, cChunk.x + MAX_VIEW_DISTANCE, cChunk.y - MAX_VIEW_DISTANCE, cChunk.y + MAX_VIEW_DISTANCE),
		[&](const tbb::blocked_range2d<int,int>& r)
		{
			//i = x, j = y, k = z
			for(int i = r.rows().begin(); i != r.rows().end(); i++)
			{
				for(int j = r.cols().begin(); j != r.cols().end(); j++)
				{
					for(int k = cChunk.z - MAX_VIEW_DISTANCE; k < cChunk.z + MAX_VIEW_DISTANCE + 1; k++)
					{
						
						Vector3 tpos = wrapAround(i, j, k);
							
						std::string tKey = generateKey(tpos.getX(), tpos.getY(), tpos.getZ());//generating a string for use as a standardized key for the chunk
						
						//test if chunk is in previous visibility list
						//if in previous visibility list, get previous chunk and add it to the new visibility set
						//test if chunk is setup and loaded
						//if not then add the chunk to the chunk processing list
						//if not in previous visibility list then add new chunk to new visibility list and chunk processing list
						if(pChunkVisibilityList.find(aChunk, tKey))
						{
							Chunk* tChunk = aChunk->second;
							aChunk.release();
							nChunkVisibilityList.insert(aChunk, tKey);
							aChunk->second = tChunk;
							aChunk.release();
							if (!tChunk->IsLoaded() || !tChunk->IsSetup())
							{
								chunkProcessingList.insert(aChunk, tKey);
								aChunk->second = tChunk;
							}
						}
						else{
							Chunk* tChunk = new Chunk(i, j, k);
							nChunkVisibilityList.insert(aChunk, tKey);
							aChunk->second = tChunk;
							aChunk.release();
							chunkProcessingList.insert(aChunk, tKey);
							aChunk->second = tChunk;
						}
						aChunk.release();//accessor must be released after use to release concurrency locks
					}
				}
			}
	});
	
	//generating the list of chunks that should be removed from memory
	tbb::parallel_for(pChunkVisibilityList.range(),
		[&](ChunkList::range_type& r) {
		for(ChunkList::iterator i = r.begin(); i != r.end(); i++)
		{
			Chunk* pChunk = i->second;//retrieving the chunk
			std::string pKey = i->first;//retrieving the chunk key
			if(!nChunkVisibilityList.find(aChunk, pKey))//test if chunk is in the new list of potentially visible chunks
			{
				chunkUnloadList.insert(aChunk, pKey);//if not, insert the current chunk into the list of chunks to be unloaded and removed from memory
				aChunk->second = pChunk;
				aChunk.release();
			}
		}
	});

	// A parallel for loop representing a Map of
	// Sequences to be performed on Chunks within
	// the range of the players that can be safe
	// done concurrently
	tbb::parallel_for(chunkProcessingList.range(), 
    [&](ChunkList::range_type& r)
    { // A labmda function with pass by reference, this requires compiling under
        // c++11 standard
        for (ChunkList::iterator i = r.begin(); i != r.end(); i++)
        { // The parallel for loop for a concurrent vector of Chunks
            // add chunk processing code below
			Chunk* pChunk = i->second;
			std::string pKey = i->first;

		    // Update Load List Sequence
		    if (pChunk->IsLoaded() == false)
		    {
				if (numChunksLoaded < 20)
				{
					pChunk->load();

					numChunksLoaded++;

					forceVisibilityUpdate = true;
				}
		    }

		    // Update Setup List Sequence
		    if (pChunk->IsLoaded() && pChunk->IsSetup() == false)
		    {
				pChunk->setup();

				if (pChunk->IsSetup())
				{
					forceVisibilityUpdate = true;
				}
		    }

		    // Update Rebuild List
		    if (pChunk->IsLoaded() && pChunk->IsSetup() && pChunk->getRebuild())
		    {
				chunkRebuildList.insert(aChunk, pKey);
			    aChunk->second = pChunk;
			    aChunk.release();
				
				// Functional Reduce code for setting the UpdateFlags of
				// neighbouring chunks
				// TODO: Reduce pattern
				
				// This can only be done once terrain generation is implemented
				// It requires testing to see if the given chunk is completely surrounded or empty
				// This results in setting flags to either render the chunk, or not render the chunk
				
				// End functional reduce code for mesh building
		    }
		}
    });
    chunkProcessingList.clear(); // Clearing the list for reuse on the next update

    // Update Flags List
    // TODO: Update Flags are part the reduce pattern
    tbb::parallel_for(chunkRebuildList.range(),
		[&](ChunkList::range_type& r)
		{
			for(ChunkList::iterator i = r.begin(); i != r.end(); i++)
			{
				if (numChunksLoaded < 20)
				{
					Chunk* pChunk = i->second;
					std::string pKey = i->first;
					Vector3 vKey = keyToVector(pKey);
					pChunk->Rebuild();
					chunkUpdateFlagsList.insert(aChunk, pKey);
					aChunk->second = pChunk;
					aChunk.release();
					
					//surrounding chunks must have their render flags updated as well
						//This requires that if a chunks is not currently loaded it must be loaded from memory
						//This requires persistence
						//Until persistence is implemented we will only update the flags of chunks already in memory
					
					//determine neighboring chunk locations
					Vector3 leftChunk = wrapAround(vKey.getX() - 1, vKey.getY(), vKey.getZ());
					Vector3 rightChunk = wrapAround(vKey.getX() + 1, vKey.getY(), vKey.getZ());
					Vector3 topChunk = wrapAround(vKey.getX(), vKey.getY() + 1, vKey.getZ());
					Vector3 botChunk = wrapAround(vKey.getX(), vKey.getY() - 1, vKey.getZ());
					Vector3 frontChunk = wrapAround(vKey.getX(), vKey.getY(), vKey.getZ() + 1);
					Vector3 backChunk = wrapAround(vKey.getX(), vKey.getY(), vKey.getZ() - 1);
					//generate neighbours keys
					std::string leftkey = generateKey(leftChunk.getX(), leftChunk.getY(), leftChunk.getZ());
					std::string rightkey = generateKey(rightChunk.getX(), rightChunk.getY(), rightChunk.getZ());
					std::string topkey = generateKey(topChunk.getX(), topChunk.getY(), topChunk.getZ());
					std::string bottkey = generateKey(botChunk.getX(), botChunk.getY(), botChunk.getZ());
					std::string frontkey = generateKey(frontChunk.getX(), frontChunk.getY(), frontChunk.getZ());
					std::string backkey = generateKey(backChunk.getX(), backChunk.getY(), backChunk.getZ());
					
					
					//Check for Chunk existence and only update flags for chunks that exist
					if(nChunkVisibilityList.find(aChunk, leftkey))
					{
						Chunk* leChunk = aChunk->second;
						aChunk.release();
						chunkUpdateFlagsList.insert(aChunk, leftkey);
						aChunk->second = leChunk;
						aChunk.release();
					}
					if(nChunkVisibilityList.find(aChunk, rightkey))
					{
						Chunk* riChunk = aChunk->second;
						aChunk.release();
						chunkUpdateFlagsList.insert(aChunk, rightkey);
						aChunk->second = riChunk;
						aChunk.release();
					}
					if(nChunkVisibilityList.find(aChunk, topkey))
					{
						Chunk* toChunk = aChunk->second;
						aChunk.release();
						chunkUpdateFlagsList.insert(aChunk, topkey);
						aChunk->second = toChunk;
						aChunk.release();
					}
					if(nChunkVisibilityList.find(aChunk, bottkey))
					{
						Chunk* boChunk = aChunk->second;
						aChunk.release();
						chunkUpdateFlagsList.insert(aChunk, bottkey);
						aChunk->second = boChunk;
						aChunk.release();
					}
					if(nChunkVisibilityList.find(aChunk, frontkey))
					{
						Chunk* frChunk = aChunk->second;
						aChunk.release();
						chunkUpdateFlagsList.insert(aChunk, frontkey);
						aChunk->second = frChunk;
						aChunk.release();
					}
					if(nChunkVisibilityList.find(aChunk, backkey))
					{
						Chunk* baChunk = aChunk->second;
						aChunk.release();
						chunkUpdateFlagsList.insert(aChunk, backkey);
						aChunk->second = baChunk;
						aChunk.release();
					}

					numChunksLoaded++;
					forceVisibilityUpdate = true;
				}
			}
	
    });
    chunkRebuildList.clear(); // Clearing the list for reuse on the next update
	
	tbb::parallel_for(chunkUpdateFlagsList.range(),
		[&](ChunkList::range_type& r)
		{
			for(ChunkList::iterator i = r.begin(); i != r.end(); i++)
			{
				Chunk* pChunk = i->second;
				pChunk->setRenderFlags();
			}
	});

    // Update Unload List
    tbb::parallel_for(chunkUnloadList.range(),
		[&](ChunkList::range_type& r)
		{
			for(ChunkList::iterator i = r.begin(); i != r.end(); i++)
			{ // A for loop that removes and unloads chunks
				if (i->second != NULL)
				{
					Chunk* pChunk = i->second;
					delete pChunk;
						// Deletes the chunk to simulate the unload function
                        // which will be written after world persistence is
                        // implemented
				}
			}
    });
    chunkUnloadList.clear(); // clearing the list for reuse on the next update

    // Update RenderList if camera position or rotation has changed since last
    // update
	if (*pCameraPosition != cameraPosition || pCameraPosition != pCameraRotation || forceVisibilityUpdate)			//Current Work
	{
		chunkRenderList.clear();//Clearing the Render list before rebuilding it
		tbb::parallel_for(nChunkVisibilityList.range(),
			[&](ChunkList::range_type& r)
			{
				for(ChunkList::iterator i = r.begin(); i != r.end(); i++)// A reduce pattern to determine which chunks should be loaded
				{
					Chunk* pChunk = i->second;
					std::string pKey = i->first;
					if(pChunk->IsLoaded() && pChunk->IsSetup())//A simple check if the chunk is set-up and loaded before potential culling
					{
						if(pChunk->sRender())
						{
							// TODO: Frustrum Culling code
							// Frustrum Culling requires terrain generation
							// Assume if the above is true, add the chunk to the Render list
							chunkRenderList.insert(aChunk, pKey);
							aChunk->second = pChunk;
							aChunk.release();
						}
					}
				}
		});
		chunkRenderList = nChunkVisibilityList;
		
		forceVisibilityUpdate = false;
	}

    pChunkVisibilityList = nChunkVisibilityList;
    nChunkVisibilityList.clear();

    *pCameraPosition = cameraPosition; // setting the previous camera position to
                                     // the current camera position for the next
                                     // updates calculation purposes
    *pCameraRotation = cameraRotation; // setting the previous camera rotation to
                                     // the current camera rotation for the next
                                     // updates calculation purposes
}
/*  Use of index position of Chunks is being deprecated in favour of the use of hash maps to store chunks in memory
int ChunkManager::translatePositionToIndex(
  Vector3 chunkPosition) // Accepts the X,Y,Z coordinates of a chunk and
                         // translates it to and one dimensional index position
                         // based upon the size limits of the world
{
  float x = chunkPosition.x;
  float y = chunkPosition.y;
  float z = chunkPosition.z;
  return (x + (MAX_CHUNKS_INDEX_X / 2)) +
         ((y + (MAX_CHUNKS_INDEX_Y / 2)) * MAX_CHUNKS_INDEX_X) +
         ((z + (MAX_CHUNKS_INDEX_Z / 2)) * MAX_CHUNKS_INDEX_X *
          MAX_CHUNKS_INDEX_Y); // This assumes a zero based index and that
  // the negative range and the non-negative range(this includes 0 as
  // non-negative) are of equal size
}

Vector3* ChunkManager::translateIndexToPosition(
  int chunkIndex) // Accepts the one dimensional index position of a Chunk and
                  // returns the Vector3 coordinates in the world space.
{
  // Some of the math bellow may require require casting the values to doubles
  // or floats to avoid truncation
  //	float cIndex = (double) chunkIndex;
  float ara[3]; // This may need to be cast to a constant float
  ara[0] = chunkIndex % MAX_CHUNKS_INDEX_X -
           (MAX_CHUNKS_INDEX_X /
            2); // Calculating the X position of the chunk in the world
  ara[1] = floor((chunkIndex % (MAX_CHUNKS_INDEX_X * MAX_CHUNKS_INDEX_Y)) /
                 MAX_CHUNKS_INDEX_X) -
           (MAX_CHUNKS_INDEX_Y /
            2); // Calculating the Y position of the Chunk in the World
  ara[2] = floor(chunkIndex / (MAX_CHUNKS_INDEX_X * MAX_CHUNKS_INDEX_Y)) -
           (MAX_CHUNKS_INDEX_Z /
            2); // Calculating the Z position of the Chunk in the World
  return new Vector3(ara[0], ara[1], ara[2]);
}
*/

std::string ChunkManager::generateKey(int sx, int sy, int sz)//translates a chunk position value, to a standardized hash map key
{
	std::stringstream ssKey;				//creating a stringstream object to translate the integers to a string
	ssKey << sx << "," << sy << "," << sz;	//standard format for the hash map keys is 'x,y,z'
	return ssKey.str();						//returning the formatted string
}

Vector3 ChunkManager::keyToVector(std::string key)//translates a hash table key to a Vector3 for reduce operations
{
	std::vector<int> result;
	std::stringstream data(key);
	std::string line;
	while(std::getline(data,line,','))
	{
		result.push_back(std::stoi(line));
	}
	float x_pos = result[0];
	float y_pos = result[1];
	float z_pos = result[2];
	
	Vector3 chunk(x_pos, y_pos, z_pos);
	return chunk;
}

Vector3 ChunkManager::wrapAround(int x, int y, int z)
{
	int ti, tj, tk;
						
	if (x >= (MAX_CHUNKS_INDEX_X / 2))//enforcing world positional wrap on the x axis
	{
		ti = x - MAX_CHUNKS_INDEX_X;
	}
	else if(x < (MAX_CHUNKS_INDEX_X/2))
	{
		ti = x + MAX_CHUNKS_INDEX_X;
	}
	else
		ti = x;
	
	if(y >= (MAX_CHUNKS_INDEX_Y/2))//enforcing world positional wrap on the y axis
	{
		tj = y - MAX_CHUNKS_INDEX_Y;
	}
	else if(y < (MAX_CHUNKS_INDEX_Y/2))
	{
		tj = y + MAX_CHUNKS_INDEX_Y;
	}
	else
		tj = y;
	
	if(z >= (MAX_CHUNKS_INDEX_Z/2))//enforcing world positional wrap on the z axis
	{
		tk = z - MAX_CHUNKS_INDEX_Z;
	}
	else if(z < (MAX_CHUNKS_INDEX_Z/2))
	{
		tk = z + MAX_CHUNKS_INDEX_Z;
	}
	else
		tk = z;
		
	Vector3 result = Vector3(ti, tj, tk);
	return result;
}

Vector3 ChunkManager::translateCameraPositionToChunk(Vector3 cameraPosition)
{
  float x_position = 
	floor(cameraPosition.x / 16);
  float y_position = 
	floor(cameraPosition.y / 16);
  float z_posiiton = 
	floor(cameraPosition.z / 16);
  Vector3 chunk(x_position, y_position, z_posiiton);
  return chunk;
}
