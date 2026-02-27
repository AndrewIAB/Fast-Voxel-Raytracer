#pragma once

#define CHUNK_SIDE_LEN 8

#include <vector>
#include <queue>

typedef unsigned int WorldPosition;

struct VoxelChunk {
	bool updated;
	WorldPosition x, y, z;
	unsigned char data[CHUNK_SIDE_LEN * CHUNK_SIDE_LEN * CHUNK_SIDE_LEN];
};

struct VoxelWorld {
private:
	std::vector<VoxelChunk*> chunks;
	std::queue<VoxelChunk*> chunkUpdateQueue;

	void AddUpdateChunk(VoxelChunk* chunk);
public:
	VoxelChunk* GetChunk(WorldPosition x, WorldPosition y, WorldPosition z);
	VoxelChunk* PopUpdateChunk();

	unsigned char GetBlock(WorldPosition x, WorldPosition y, WorldPosition z);
	void SetBlock(unsigned char block, WorldPosition x, WorldPosition y, WorldPosition z);
};