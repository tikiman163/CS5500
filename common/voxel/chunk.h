#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"
#include "vector3.h"

class Chunk
{
public:
  Chunk(int x, int y, int z);
  ~Chunk();

  void Update(float dt);
  bool IsLoaded();
  void load();
  bool IsSetup();
  void setup();
  bool getRebuild();
  void setRebuild();// Called by player handler class when chunk contents have changed
					//flags the chunk as needing its mesh rebuilt
	void Rebuild();
	void setRenderFlags();
	bool sRender();
  Vector3* getPosition();

  /*
  ** Assuming the use of OpenGLRenderer class from JUCE  Currently commented out
  *to prevent compile errors
  ** void Render(OpenGLRenderer* pRenderer);
  ** To be added back in next sprint for rendering compatibility
  */

  static const int CHUNK_SIZE = 16;

private:
  // The blocks data
  Block*** mBlocks;
  bool loaded = false;
  bool setupd = false;
  bool rebuild = false;
  bool surrounded = false;
  bool empty = false;
  bool filled = false;
  int X; // World Chunk Position;
  int Y; // World Chunk Position;
  int Z; // World Chunk Position;
};

#endif
