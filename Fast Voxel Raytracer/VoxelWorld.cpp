#include "VoxelWorld.hpp"

static int ChunkBlockIdx(int x, int y, int z) {
	return x + (y + z * CHUNK_SIDE_LEN) * CHUNK_SIDE_LEN;
}

static void ChunkBlockPosition(int index, int& x, int& y, int& z) {
	x = index % CHUNK_SIDE_LEN;
	y = (index / CHUNK_SIDE_LEN) % CHUNK_SIDE_LEN;
	z =	index / (CHUNK_SIDE_LEN * CHUNK_SIDE_LEN);
}

VoxelChunk* VoxelWorld::GetChunk(WorldPosition x, WorldPosition y, WorldPosition z) {
	for (VoxelChunk* c : chunks) {
		if (c->x == x && c->y == y && c->z == z) {
			return c;
		}
	}

	chunks.push_back(new VoxelChunk());
	chunks.back()->x = x;
	chunks.back()->y = x;
	chunks.back()->z = x;
	std::fill(std::begin(chunks.back()->data), std::end(chunks.back()->data), 0);
	chunks.back()->updated = false;

	// Need to add to updated chunks since it was loaded
	// Really only matters if the chunk is loaded from a file which, currently, it is not lol
	AddUpdateChunk(chunks.back());

	return chunks.back();
}

void VoxelWorld::AddUpdateChunk(VoxelChunk* chunk) {
	if (!chunk->updated) {
		chunkUpdateQueue.push(chunk);
		chunk->updated = true;
	}
}

VoxelChunk* VoxelWorld::PopUpdateChunk() {
	if (chunkUpdateQueue.empty()) {
		return nullptr;
	}
	VoxelChunk* chunk = chunkUpdateQueue.back();
	chunk->updated = false;
	chunkUpdateQueue.pop();
	return chunk;
}

unsigned char VoxelWorld::GetBlock(WorldPosition x, WorldPosition y, WorldPosition z) {
	WorldPosition cx = x / CHUNK_SIDE_LEN;
	WorldPosition cy = y / CHUNK_SIDE_LEN;
	WorldPosition cz = z / CHUNK_SIDE_LEN;

	VoxelChunk* chunk = GetChunk(cx, cy, cz);

	WorldPosition bx = x % CHUNK_SIDE_LEN;
	WorldPosition by = y % CHUNK_SIDE_LEN;
	WorldPosition bz = z % CHUNK_SIDE_LEN;

	WorldPosition idx = ChunkBlockIdx(bx, by, bz);

	return chunk->data[idx];
}

void VoxelWorld::SetBlock(unsigned char block, WorldPosition x, WorldPosition y, WorldPosition z) {
	WorldPosition cx = x / CHUNK_SIDE_LEN;
	WorldPosition cy = y / CHUNK_SIDE_LEN;
	WorldPosition cz = z / CHUNK_SIDE_LEN;

	VoxelChunk* chunk = GetChunk(cx, cy, cz);
	AddUpdateChunk(chunk);

	WorldPosition bx = x % CHUNK_SIDE_LEN;
	WorldPosition by = y % CHUNK_SIDE_LEN;
	WorldPosition bz = z % CHUNK_SIDE_LEN;

	WorldPosition idx = ChunkBlockIdx(bx, by, bz);

	chunk->data[idx] = block;
}
