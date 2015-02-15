#include "block.h"
#include "chunk.h"

Chunk::Chunk(int x, int y, int z)
{
  X = x;
  Y = y;
  Z = z;
  // Block array creation
  mBlocks = new Block** [CHUNK_SIZE];
  for (int i = 0; i < CHUNK_SIZE; i++)
  {
    mBlocks[i] = new Block* [CHUNK_SIZE];

    for (int j = 0; j < CHUNK_SIZE; j++)
    {
      mBlocks[i][j] = new Block[CHUNK_SIZE];
    }
  }
}

Chunk::~Chunk()
{
  // Block array deletion
  for (int i = 0; i < CHUNK_SIZE; ++i)
  {
    for (int j = 0; j < CHUNK_SIZE; ++j)
    {
      delete[] mBlocks[i][j];
    }

    delete[] mBlocks[i];
  }
  delete[] mBlocks;
}

bool Chunk::IsLoaded()
{
  return loaded;
}

void Chunk::load()
{
  // A stub for chunk loading code
  loaded = true; // Assumes successful load to advance program test
}

bool Chunk::IsSetup()
{
  return setupd;
}

void Chunk::setup()
{
  // A stub for chunk setup code
	// Chunk render lists generation
		// Generate a list of vertices
		// Generate a list of triangles
		// Generate a list of UVs for texture atlas
	//This requires terrain generation
	//After mesh generation, render flags must be set
	this->setRenderFlags();
  setupd = true; // Assumes successful setup to advance program test
}

bool Chunk::getRebuild()
{
  return rebuild;
}

void Chunk::setRebuild()
{
  rebuild = true;
}

void Chunk::Rebuild()
{
	//TODO: code to rebuild the chunk mesh
	rebuild = false;
}

void Chunk::setRenderFlags()
{
	//A stub for code that determines if the chunk is surrounded
		//Determine if all Chunks surrounding this chunk are filled
		//This requires terrain generation
	//A stub for code that determines if the chunk is empty
		//check if the generated mesh contains verts
		//This requires terrain generation
	//A stub for code that determines if the chunk is filled
		//check if generated chunk mesh is one large cube the size of the chunk
		//this requires terrain generation
		
	//Until terrain generation we assume chunk should be rendered if visible,
	//So flags are not modified
}

bool Chunk::sRender()
{
	return (!surrounded && !empty);
}

Vector3* Chunk::getPosition()
{
  return (new Vector3(X, Y, Z));
}

void Chunk::Update(float /*dt*/)
{
  // A function stub for Chunk update code to be called from the Chunk Manager
}

/*
**	See header file for comments regarding this function
**	void Chunk::Render(OpenGLRenderer* pRenderer)
**	{
**		//A function stub for the render call from the Chunk Manager
**	}
*/
