//-----------------------------------------------------------------------------
//  This is an implementation of Tom Forsyth's "Linear-Speed Vertex Cache 
//  Optimization" algorithm as described here:
//  http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html
//
//  This code was authored and released into the public domain by
//  Adrian Stone (stone@gameangst.com).
//
//  Backported to C, OptimizeVertexes() added, and made into a single header by
//  James Canete.
//
//  THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
//  SHALL ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER 
//  LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
//  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef JRC_FORSYTH_H
#define JRC_FORSYTH_H

typedef unsigned int fIndex_t;
enum { MAX_INDEX = 4294967295u};

//-----------------------------------------------------------------------------
//  OptimizeFaces
//-----------------------------------------------------------------------------
//  Parameters:
//      indexList
//          input index list
//      indexCount
//          the number of indices in the list
//      vertexCount
//          the largest index value in indexList
//      newIndexList
//          a pointer to a preallocated buffer the same size as indexList to
//          hold the optimized index list
//      lruCacheSize
//          the size of the simulated post-transform cache (max:64)
//-----------------------------------------------------------------------------
void OptimizeFaces(const fIndex_t* indexList, unsigned int indexCount, unsigned int vertexCount, fIndex_t* newIndexList, fIndex_t lruCacheSize);

//-----------------------------------------------------------------------------
//  OptimizeVertexes
//-----------------------------------------------------------------------------
//  Parameters:
//      vertexList
//          input vertex list
//      vertexCount
//          the largest index value in indexList
//      vertexSize
//          the size, in bytes, of each vertex
//      indexList
//          input index list
//      indexCount
//          the number of indices in the list
//      newVertexList
//          a pointer to a preallocated buffer the same size as vertexList to
//          hold the optimized vertex list
//      newIndexList
//          a pointer to a preallocated buffer the same size as indexList to
//          hold the modified index list
//-----------------------------------------------------------------------------
void OptimizeVertexes(const void *vertexList, unsigned int vertexCount, unsigned int vertexSize, fIndex_t *indexList, unsigned int indexCount, void *newVertexList, fIndex_t *newIndexList);

#endif

#ifdef JRC_FORSYTH_IMPLEMENTATION

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int uint;

// code for computing vertex score was taken, as much as possible
// directly from the original publication.
static float ComputeVertexCacheScore(int cachePosition, int vertexCacheSize)
{
	const float FindVertexScore_CacheDecayPower = 1.5f;
	const float FindVertexScore_LastTriScore = 0.75f;

	float score = 0.0f;
	if ( cachePosition < 0 )
	{
		// Vertex is not in FIFO cache - no score.
	}
	else
	{
		if ( cachePosition < 3 )
		{
			// This vertex was used in the last triangle,
			// so it has a fixed score, whichever of the three
			// it's in. Otherwise, you can get very different
			// answers depending on whether you add
			// the triangle 1,2,3 or 3,1,2 - which is silly.
			score = FindVertexScore_LastTriScore;
		}
		else
		{
			assert ( cachePosition < vertexCacheSize );
			// Points for being high in the cache.
			const float scaler = 1.0f / ( vertexCacheSize - 3 );
			score = 1.0f - ( cachePosition - 3 ) * scaler;
			score = powf ( score, FindVertexScore_CacheDecayPower );
		}
	}

	return score;
}

static float ComputeVertexValenceScore(uint numActiveFaces)
{
	const float FindVertexScore_ValenceBoostScale = 2.0f;
	const float FindVertexScore_ValenceBoostPower = 0.5f;

	float score = 0.f;

	// Bonus points for having a low number of tris still to
	// use the vert, so we get rid of lone verts quickly.
	float valenceBoost = powf ( (float)(numActiveFaces),
		-FindVertexScore_ValenceBoostPower );
	score += FindVertexScore_ValenceBoostScale * valenceBoost;

	return score;
}


enum {kMaxVertexCacheSize = 64};
enum {kMaxPrecomputedVertexValenceScores = 64};
static float s_vertexCacheScores[kMaxVertexCacheSize+1][kMaxVertexCacheSize];
static float s_vertexValenceScores[kMaxPrecomputedVertexValenceScores];

static char ComputeVertexScores()
{
	int cacheSize, cachePos;
	uint valence;

	for (cacheSize=0; cacheSize<=kMaxVertexCacheSize; ++cacheSize)
	{
		for (cachePos=0; cachePos<cacheSize; ++cachePos)
		{
			s_vertexCacheScores[cacheSize][cachePos] = ComputeVertexCacheScore(cachePos, cacheSize);
		}
	}

	for (valence=0; valence<kMaxPrecomputedVertexValenceScores; ++valence)
	{
		s_vertexValenceScores[valence] = ComputeVertexValenceScore(valence);
	}

	return 1;
}

static char s_vertexScoresComputed = 0;

//        inline float FindVertexCacheScore(uint cachePosition, uint maxSizeVertexCache)
//        {
//            return s_vertexCacheScores[maxSizeVertexCache][cachePosition];
//        }

//        inline float FindVertexValenceScore(uint numActiveTris)
//        {
//            return s_vertexValenceScores[numActiveTris];
//        }

float FindVertexScore(uint numActiveFaces, uint cachePosition, uint vertexCacheSize)
{
	if (!s_vertexScoresComputed)
		s_vertexScoresComputed = ComputeVertexScores();

	if ( numActiveFaces == 0 )
	{
		// No tri needs this vertex!
		return -1.0f;
	}

	float score = 0.f;
	if (cachePosition < vertexCacheSize)
	{
		score += s_vertexCacheScores[vertexCacheSize][cachePosition];
	}

	if (numActiveFaces < kMaxPrecomputedVertexValenceScores)
	{
		score += s_vertexValenceScores[numActiveFaces];
	}
	else
	{
		score += ComputeVertexValenceScore(numActiveFaces);
	}

	return score;
}

typedef struct
{
	float   score;
	uint    activeFaceListStart;
	uint    activeFaceListSize;
	fIndex_t  cachePos0;
	fIndex_t  cachePos1;
}
OptimizeVertexData;

void OptimizeFaces(const fIndex_t* indexList, unsigned int indexCount, unsigned int vertexCount, fIndex_t* newIndexList, fIndex_t lruCacheSize)
{
	OptimizeVertexData *vertexDataList;
	uint i, j, k, v, c0, c1;
	
	vertexDataList = calloc(vertexCount, sizeof(*vertexDataList));

	// compute face count per vertex
	for (i=0; i<indexCount; ++i)
	{
		fIndex_t index = indexList[i];
		assert(index < vertexCount);
		vertexDataList[index].activeFaceListSize++;
	}

	uint *activeFaceList;

	const fIndex_t kEvictedCacheIndex = MAX_INDEX;

	{
		// allocate face list per vertex
		uint curActiveFaceListPos = 0;
		for (i=0; i<vertexCount; ++i)
		{
			OptimizeVertexData* vertexData = &vertexDataList[i];
			vertexData->cachePos0 = kEvictedCacheIndex;
			vertexData->cachePos1 = kEvictedCacheIndex;
			vertexData->activeFaceListStart = curActiveFaceListPos;
			curActiveFaceListPos += vertexData->activeFaceListSize;
			vertexData->score = FindVertexScore(vertexData->activeFaceListSize, vertexData->cachePos0, lruCacheSize);
			vertexData->activeFaceListSize = 0;
		}
		activeFaceList = malloc(curActiveFaceListPos * sizeof(*activeFaceList));
	}

	// fill out face list per vertex
	for (i=0; i<indexCount; i+=3)
	{
		for (j=0; j<3; ++j)
		{
			fIndex_t index = indexList[i+j];
			OptimizeVertexData* vertexData = &vertexDataList[index];
			activeFaceList[vertexData->activeFaceListStart + vertexData->activeFaceListSize] = i;
			vertexData->activeFaceListSize++;
		}
	}

	char *processedFaceList;
	processedFaceList = calloc(indexCount, sizeof(*processedFaceList));

	fIndex_t vertexCacheBuffer[(kMaxVertexCacheSize+3)*2];
	fIndex_t* cache0 = vertexCacheBuffer;
	fIndex_t* cache1 = vertexCacheBuffer+(kMaxVertexCacheSize+3);
	fIndex_t entriesInCache0 = 0;

	uint bestFace = 0;
	float bestScore = -1.f;

	const float maxValenceScore = FindVertexScore(1, kEvictedCacheIndex, lruCacheSize) * 3.f;

	for (i = 0; i < indexCount; i += 3)
	{
		if (bestScore < 0.f)
		{
			// no verts in the cache are used by any unprocessed faces so
			// search all unprocessed faces for a new starting point
			for (j = 0; j < indexCount; j += 3)
			{
				if (processedFaceList[j] == 0)
				{
					uint face = j;
					float faceScore = 0.f;
					for (k=0; k<3; ++k)
					{
						fIndex_t index = indexList[face+k];
						OptimizeVertexData* vertexData = &vertexDataList[index];
						assert(vertexData->activeFaceListSize > 0);
						assert(vertexData->cachePos0 >= lruCacheSize);
						faceScore += vertexData->score;
					}

					if (faceScore > bestScore)
					{
						bestScore = faceScore;
						bestFace = face;

						assert(bestScore <= maxValenceScore);
						if (bestScore >= maxValenceScore)
						{
							break;
						}
					}
				}
			}
			assert(bestScore >= 0.f);
		}

		processedFaceList[bestFace] = 1;
		fIndex_t entriesInCache1 = 0;

		// add bestFace to LRU cache and to newIndexList
		for (v = 0; v < 3; ++v)
		{
			fIndex_t index = indexList[bestFace+v];
			newIndexList[i+v] = index;

			OptimizeVertexData* vertexData = &vertexDataList[index];

			if (vertexData->cachePos1 >= entriesInCache1)
			{
				vertexData->cachePos1 = entriesInCache1;
				cache1[entriesInCache1++] = index;

				if (vertexData->activeFaceListSize == 1)
				{
					--vertexData->activeFaceListSize;
					continue;
				}
			}

			assert(vertexData->activeFaceListSize > 0);
			uint* begin = &activeFaceList[vertexData->activeFaceListStart];
			uint* end = &activeFaceList[vertexData->activeFaceListStart + vertexData->activeFaceListSize];
			//uint* it = std::find(begin, end, bestFace);
			uint* it;
			for (it = begin; *it != bestFace && it != end; it++);
			assert(it != end);
			//std::swap(*it, *(end-1));
			{
				uint tmp = *it;
				*it = *(end-1);
				*(end-1)=tmp;
			}
			--vertexData->activeFaceListSize;
			vertexData->score = FindVertexScore(vertexData->activeFaceListSize, vertexData->cachePos1, lruCacheSize);

		}

		// move the rest of the old verts in the cache down and compute their new scores
		for (c0 = 0; c0 < entriesInCache0; ++c0)
		{
			fIndex_t index = cache0[c0];
			OptimizeVertexData* vertexData = &vertexDataList[index];

			if (vertexData->cachePos1 >= entriesInCache1)
			{
				vertexData->cachePos1 = entriesInCache1;
				cache1[entriesInCache1++] = index;
				vertexData->score = FindVertexScore(vertexData->activeFaceListSize, vertexData->cachePos1, lruCacheSize);
			}
		}

		// find the best scoring triangle in the current cache (including up to 3 that were just evicted)
		bestScore = -1.f;
		for (c1 = 0; c1 < entriesInCache1; ++c1)
		{
			fIndex_t index = cache1[c1];
			OptimizeVertexData* vertexData = &vertexDataList[index];
			vertexData->cachePos0 = vertexData->cachePos1;
			vertexData->cachePos1 = kEvictedCacheIndex;
			for (j=0; j<vertexData->activeFaceListSize; ++j)
			{
				uint face = activeFaceList[vertexData->activeFaceListStart+j];
				float faceScore = 0.f;
				for (v=0; v<3; v++)
				{
					fIndex_t faceIndex = indexList[face+v];
					faceScore += vertexDataList[faceIndex].score;
				}
				if (faceScore > bestScore)
				{
					bestScore = faceScore;
					bestFace = face;
				}
			}
		}

		//std::swap(cache0, cache1);
		{
			fIndex_t* tmp = cache0;
			cache0 = cache1;
			cache1 = tmp;
		}
		//entriesInCache0 = std::min(entriesInCache1, lruCacheSize);
		entriesInCache0 = entriesInCache1 < lruCacheSize ? entriesInCache1 : lruCacheSize;
	}
	
	free(vertexDataList);
	free(processedFaceList);
	free(activeFaceList);
}


void OptimizeVertexes(const void *vertexList, unsigned int vertexCount, unsigned int vertexSize, fIndex_t *indexList, unsigned int indexCount, void *newVertexList, fIndex_t *newIndexList)
{
	int i;
	fIndex_t lastVertex = 0;
	fIndex_t *vertexRemap = calloc(vertexCount, sizeof(fIndex_t));

	for (i = 0; i < indexCount; i++)
		if (vertexRemap[indexList[i]] == 0)
			vertexRemap[indexList[i]] = ++lastVertex;

	for (i = 0; i < indexCount; i++)
		newIndexList[i] = vertexRemap[indexList[i]] - 1;

	for (i = 0; i < vertexCount; i++)
		memcpy((char *)(newVertexList) + vertexSize * (vertexRemap[i] - 1), (char *)(vertexList) + vertexSize * i, vertexSize);

	free(vertexRemap);
}

#endif