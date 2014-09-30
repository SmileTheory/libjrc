#ifndef INCLUDE_JRCVAO_H
#define INCLUDE_JRCVAO_H

#include <stdint.h>
#include <GL/gl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define JRCVAO_MAX_ATTRIBS 16
#define JRCVAO_MAX_LOCKS 25

typedef struct jrcVaoAttrib_s
{
	uint32_t enabled;
	uint32_t count;
	uint32_t type;
	uint32_t normalized;
	uint32_t stride;
	uint32_t offset;
}
jrcVaoAttrib_t;

typedef uint32_t index_t;
#define JRCVAO_INDEX_TYPE GL_UNSIGNED_INT

typedef struct dualRangeLock_s
{
	GLsync sync;
	uint32_t vStart;
	uint32_t vEnd;
	uint32_t iStart;
	uint32_t iEnd;
}
dualRangeLock_t;

typedef enum
{
	JRCVAO_STATIC             = 0x00,
	JRCVAO_STREAMING_VERTEXES = 0x01,
	JRCVAO_STREAMING_INDEXES  = 0x02,
	JRCVAO_STREAMING          = 0x03,
	JRCVAO_STREAM_TYPE        = 0x03,
}
jrcVaoFlags_t;

typedef struct jrcVao_s
{
	// public, set before init
	jrcVaoAttrib_t attribs[JRCVAO_MAX_ATTRIBS];
	int numAttribs;

	uint32_t vertexSize;
	uint32_t indexSize;

	uint32_t vertexBufferSize;
	uint32_t indexBufferSize;

	jrcVaoFlags_t flags;

	// public, use after mapping
	void *vMap;
	void *iMap;

	// private, do not touch
	uint32_t vao;
	uint32_t vbo;
	uint32_t ibo;

	uint32_t vMapStart, vMapEnd;
	uint32_t iMapStart, iMapEnd;

	void *vPersistentBuffer;
	void *iPersistentBuffer;

	dualRangeLock_t locks[JRCVAO_MAX_LOCKS];
	int numLocks;
}
jrcVao_t;

int jrcVao_Init(jrcVao_t *vao);

// use only for dynamic buffers
void jrcVao_MapBuffersRange(jrcVao_t *vao, uint32_t numVertexes, uint32_t numIndexes);
void jrcVao_UnmapBuffers(jrcVao_t *vao);
void jrcVao_DrawBuffers(jrcVao_t *vao, GLenum primitiveType, GLsizei numElements, uint32_t firstIndex);
void jrcVao_LockPrevMappedBuffers(jrcVao_t *vao);
void jrcVao_UnmapAndDrawBuffers(jrcVao_t *vao, GLenum primitiveType, GLsizei numElements);

// use only for static buffers
void jrcVao_SetBuffers(jrcVao_t *vao, void *vBuffer, void *iBuffer);

void jrcVao_Destroy(jrcVao_t *vao);
void jrcVao_Bind(jrcVao_t *vao);

#ifdef __cplusplus
}
#endif

#endif
#ifdef JRC_VAO_IMPLEMENTATION
#include <stdio.h>

#include <GL/glew.h>

#include "SDL.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static jrcVao_t *currentVao;

void jrcVao_SetAttribPointers(jrcVao_t *vao)
{
	int i;

	for (i = 0; i < JRCVAO_MAX_ATTRIBS; i++)
	{
		if (vao->attribs[i].enabled)
		{
			glVertexAttribPointer((GLuint)i,
								  (GLint)vao->attribs[i].count,
								  (GLenum)vao->attribs[i].type, 
								  (GLboolean)vao->attribs[i].normalized, 
								  (GLsizei)vao->attribs[i].stride,
								  BUFFER_OFFSET(vao->attribs[i].offset));
			glEnableVertexAttribArray(i);
		}
		else
		{
			glDisableVertexAttribArray(i);
		}
	}
}

static int jrcVao_BufferType = 0;

#define JRCVAO_BUFFER_USE_PERSISTENT 1
#define JRCVAO_BUFFER_USE_MAPPED     2
#define JRCVAO_BUFFER_USE_COPY       3

static void jrcVao_CreateBuffer(GLenum type, int size, void **buffer)
{
	if (jrcVao_BufferType == JRCVAO_BUFFER_USE_PERSISTENT)
	{
		GLbitfield bufferFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

		glBufferStorage(type, size, NULL, bufferFlags);
		*buffer = glMapBufferRange(type, 0, size, bufferFlags);
	}
	else
	{
		glBufferData(type, size, NULL, GL_DYNAMIC_DRAW);
	}

	if (jrcVao_BufferType == JRCVAO_BUFFER_USE_COPY)
	{
		// fallback if streaming isn't available
		// pretend we have persistent buffers
		*buffer = malloc(size);
	}
}

int jrcVao_Init(jrcVao_t *vao)
{
	if (!jrcVao_BufferType)
	{
		if (GLEW_ARB_buffer_storage && GLEW_ARB_map_buffer_range && GLEW_ARB_sync && GLEW_ARB_draw_elements_base_vertex)
			jrcVao_BufferType = JRCVAO_BUFFER_USE_PERSISTENT;
		else if (GLEW_ARB_map_buffer_range && GLEW_ARB_sync && GLEW_ARB_draw_elements_base_vertex)
			jrcVao_BufferType = JRCVAO_BUFFER_USE_MAPPED;
		else
			jrcVao_BufferType = JRCVAO_BUFFER_USE_COPY;
	}

	if (GLEW_ARB_vertex_array_object)
	{
		glGenVertexArrays(1, &vao->vao);
		glBindVertexArray(vao->vao);
	}

	glGenBuffers(1, &vao->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vao->vbo);

	glGenBuffers(1, &vao->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->ibo);

	if (vao->flags & JRCVAO_STREAMING_VERTEXES)
		jrcVao_CreateBuffer(GL_ARRAY_BUFFER, vao->vertexBufferSize, &vao->vPersistentBuffer);
	
	if (vao->flags & JRCVAO_STREAMING_INDEXES)
		jrcVao_CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->indexBufferSize, &vao->iPersistentBuffer);

	jrcVao_SetAttribPointers(vao);

	vao->numLocks = 0;

	currentVao = vao;

	return 1;
}

void jrcVao_ExpireLocks(jrcVao_t *vao, int delay, int flush)
{
	dualRangeLock_t *lock;

	// won't be any locks if there's no GL_ARB_sync, so safe to do this
	for (lock = vao->locks; (int)(lock - vao->locks) < vao->numLocks; lock++)
	{
		GLenum status;

		status = glClientWaitSync(lock->sync, flush ? GL_SYNC_FLUSH_COMMANDS_BIT : 0, delay);

		if (status == GL_WAIT_FAILED)
		{
			// FIXME: error or something
			SDL_Log("glClientWaitSync(): GL_WAIT_FAILED\n");
		}

		if (status != GL_TIMEOUT_EXPIRED)
		{
			glDeleteSync(lock->sync);
			*lock-- = vao->locks[--vao->numLocks];
		}
	}
}

void jrcVao_MapBuffersRange(jrcVao_t *vao, uint32_t numVertexes, uint32_t numIndexes)
{
	uint32_t vRange = 0, iRange = 0;
	dualRangeLock_t *lock;

	// can't be used on static VAOs
	if ((vao->flags & JRCVAO_STREAM_TYPE) == JRCVAO_STATIC)
	{
		SDL_Log("jrcVao_MapBuffersRange(): VAO is static! %d\n", vao->flags);
		return;
	}

	vao->vMap = NULL;
	vao->iMap = NULL;

	if (vao->flags & JRCVAO_STREAMING_VERTEXES)
	{
		vRange = numVertexes * vao->vertexSize;

		if (vRange > vao->vertexBufferSize)
		{
			SDL_Log("jrcVao_MapBuffersRange(): too many vertexes (%d, %d > %d)", numVertexes, vRange, vao->vertexBufferSize);
			return;
		}

		if (vao->vMapStart + vRange > vao->vertexBufferSize)
			vao->vMapStart = 0;

		vao->vMapEnd = vao->vMapStart + vRange;
	}

	if (vao->flags & JRCVAO_STREAMING_INDEXES)
	{
		iRange = numIndexes * vao->indexSize;

		if (iRange > vao->indexBufferSize)
		{
			SDL_Log("jrcVao_MapBuffersRange(): too many indexes (%d, %d > %d)", numIndexes, iRange, vao->indexBufferSize);
			return;
		}

		if (vao->iMapStart + iRange > vao->indexBufferSize)
			vao->iMapStart = 0;

		vao->iMapEnd = vao->iMapStart + iRange;
	}


	// won't be any locks if there's no GL_ARB_sync, so safe to do this
	jrcVao_ExpireLocks(vao, 0, 0);
	for (lock = vao->locks; (int)(lock - vao->locks) < vao->numLocks; lock++)
	{
		int vCollision = (vao->flags & JRCVAO_STREAMING_VERTEXES) && !((vao->vMapEnd <= lock->vStart) || (vao->vMapStart >= lock->vEnd));
		int iCollision = (vao->flags & JRCVAO_STREAMING_INDEXES)  && !((vao->iMapEnd <= lock->iStart) || (vao->iMapStart >= lock->iEnd));

		if (vCollision || iCollision)
		{
			int currentTick = SDL_GetTicks();
			GLenum status;

			while ((status = glClientWaitSync(lock->sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000) == GL_TIMEOUT_EXPIRED))
				;

			SDL_Log("stalled for %dms!\n", SDL_GetTicks() - currentTick);

			if (status == GL_WAIT_FAILED)
			{
				// FIXME: error or something
				SDL_Log("glClientWaitSync(): GL_WAIT_FAILED\n");
			}

			glDeleteSync(lock->sync);
			*lock-- = vao->locks[--vao->numLocks];
		}
	}

	jrcVao_Bind(NULL);

	if (vao->flags & JRCVAO_STREAMING_VERTEXES)
	{
		if (vao->vPersistentBuffer)
			vao->vMap = (uint8_t *)vao->vPersistentBuffer + vao->vMapStart;
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, vao->vbo);
			vao->vMap = glMapBufferRange(GL_ARRAY_BUFFER,         vao->vMapStart, vRange, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		}
	}
	
	if (vao->flags & JRCVAO_STREAMING_INDEXES)
	{
		if (vao->iPersistentBuffer)
			vao->iMap = (uint8_t *)vao->iPersistentBuffer + vao->iMapStart;
		else
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->ibo);
			vao->iMap = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, vao->iMapStart, iRange, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		}
	}
}

void jrcVao_UnmapBuffers(jrcVao_t *vao)
{
	if (!vao->vPersistentBuffer && (vao->flags & JRCVAO_STREAMING_VERTEXES))
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (!vao->iPersistentBuffer && (vao->flags & JRCVAO_STREAMING_INDEXES))
	{
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	if (jrcVao_BufferType == JRCVAO_BUFFER_USE_COPY)
	{
		if (!GLEW_ARB_draw_elements_base_vertex  && (vao->flags & JRCVAO_STREAMING_INDEXES))
		{
			// if no base vertex extension, increment all the indexes by hand
			uint32_t i;
			index_t *index = (index_t *)(vao->iPersistentBuffer + vao->iMapStart);

			for (i = vao->iMapStart; i < vao->iMapEnd; i += vao->indexSize)
			{
				*index += vao->vMapStart / vao->vertexSize;
				index++;
			}
		}

		// Do an old-fashioned copy
		if (vao->flags & JRCVAO_STREAMING_VERTEXES)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vao->vbo);
			glBufferSubData(GL_ARRAY_BUFFER,         vao->vMapStart, vao->vMapEnd - vao->vMapStart, (uint8_t *)vao->vPersistentBuffer + vao->vMapStart);
		}

		if (vao->flags & JRCVAO_STREAMING_INDEXES)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->ibo);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, vao->iMapStart, vao->iMapEnd - vao->iMapStart, (uint8_t *)vao->iPersistentBuffer + vao->iMapStart);
		}
	}
}

void jrcVao_DrawBuffers(jrcVao_t *vao, GLenum primitiveType, GLsizei numElements, uint32_t firstIndex)
{
	char *offset = BUFFER_OFFSET(vao->iMapStart + vao->indexSize * firstIndex);
	jrcVao_Bind(vao);

	if (GLEW_ARB_draw_elements_base_vertex)
		glDrawElementsBaseVertex(primitiveType, numElements, JRCVAO_INDEX_TYPE, offset, vao->vMapStart / vao->vertexSize);
	else
		glDrawElements(primitiveType, numElements, JRCVAO_INDEX_TYPE, offset);
}

void jrcVao_LockPrevMappedBuffers(jrcVao_t *vao)
{
	if (GLEW_ARB_sync)
	{
		dualRangeLock_t *newLock;

		jrcVao_ExpireLocks(vao, 0, 0);

		// if no locks have already expired, busy wait for one
		// FIXME: THIS IS HORRIBLE
		while (vao->numLocks == JRCVAO_MAX_LOCKS)
			jrcVao_ExpireLocks(vao, 1000000, 1);

		newLock = &vao->locks[vao->numLocks++];
		newLock->sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		newLock->vStart = vao->vMapStart;
		newLock->vEnd   = vao->vMapEnd;
		newLock->iStart = vao->iMapStart;
		newLock->iEnd   = vao->iMapEnd;
	}

	vao->vMapStart = vao->vMapEnd;
	vao->iMapStart = vao->iMapEnd;
}

void jrcVao_UnmapAndDrawBuffers(jrcVao_t *vao, GLenum primitiveType, GLsizei numElements)
{
	// can't be used on static VAOs
	if ((vao->flags & JRCVAO_STREAM_TYPE) == JRCVAO_STATIC) return;

	jrcVao_UnmapBuffers(vao);
	jrcVao_DrawBuffers(vao, primitiveType, numElements, 0);
	jrcVao_LockPrevMappedBuffers(vao);
}

void jrcVao_SetBuffers(jrcVao_t *vao, void *vBuffer, void *iBuffer)
{
	jrcVao_Bind(NULL);

	if (vBuffer && !(vao->flags & JRCVAO_STREAMING_VERTEXES))
	{
		glBindBuffer(GL_ARRAY_BUFFER, vao->vbo);
		glBufferData(GL_ARRAY_BUFFER, vao->vertexBufferSize, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER,         0, vao->vertexBufferSize, vBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	if (iBuffer && !(vao->flags & JRCVAO_STREAMING_INDEXES))
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vao->indexBufferSize, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, vao->indexBufferSize,  iBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void jrcVao_Destroy(jrcVao_t *vao)
{
	if (!vao) return;

	if (currentVao == vao) jrcVao_Bind(NULL);

	if (jrcVao_BufferType == JRCVAO_BUFFER_USE_COPY)
	{
		if (vao->flags & JRCVAO_STREAMING_VERTEXES) free(vao->vPersistentBuffer);
		if (vao->flags & JRCVAO_STREAMING_INDEXES)  free(vao->iPersistentBuffer);
	}

	if (GLEW_ARB_vertex_array_object) glDeleteVertexArrays(1, &vao->vao);

	glDeleteBuffers(1, &vao->vbo);
	glDeleteBuffers(1, &vao->ibo);
}

void jrcVao_Bind(jrcVao_t *vao)
{
	if (vao == currentVao) return;

	currentVao = vao;

	if (GLEW_ARB_vertex_array_object)
	{
		glBindVertexArray(vao ? vao->vao : 0);

		// why you no save GL_ELEMENT_ARRAY_BUFFER binding, Intel?
		if (1) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao ? vao->ibo : 0);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER,         vao ? vao->vbo : 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao ? vao->ibo : 0);
		if (vao) jrcVao_SetAttribPointers(vao);
	}
}

#endif