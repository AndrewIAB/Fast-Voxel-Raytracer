#version 430 core

in vec2 i_Uv;
in vec3 i_CoordPos;
out vec4 o_Color;

layout (location = 0) uniform mat3 u_RotationX;
layout (location = 1) uniform mat3 u_RotationY;
layout (location = 2) uniform vec3 u_Position;

layout (binding = 0) uniform sampler2D u_NoiseTexture;

const int MAX_LENGTH = 100;

struct RayResult {
	vec3 hitPos;
	vec3 hitNormal;
	ivec3 hitBlock;
	float dist;
};

vec2 noiseLayerOff = vec2(0.);

vec3 GetNoise() {
	vec4 noiseSample = texture(u_NoiseTexture, i_Uv + noiseLayerOff);
	noiseLayerOff += noiseSample.xy;
	return noiseSample.xyz;
}

// ALMOST branchless ray step :3
RayResult StepRay(vec3 origin, vec3 dir) {
	vec3 floorOrigin = floor(origin);

	float offX = (dir.x < 0 && origin.x == floorOrigin.x) ? -1 : 0;
	float offY = (dir.y < 0 && origin.y == floorOrigin.y) ? -1 : 0;
	float offZ = (dir.z < 0 && origin.z == floorOrigin.z) ? -1 : 0;

	float minX = floorOrigin.x + offX;
	float minY = floorOrigin.y + offY;
	float minZ = floorOrigin.z + offZ;
	float maxX = floorOrigin.x + 1;
	float maxY = floorOrigin.y + 1;
	float maxZ = floorOrigin.z + 1;
	
	float invDirX = 1. / dir.x;
	float invDirY = 1. / dir.y;
	float invDirZ = 1. / dir.z;
	
	float tx1 = (minX - origin.x) * invDirX;
	float tx2 = (maxX - origin.x) * invDirX;
	float ty1 = (minY - origin.y) * invDirY;
	float ty2 = (maxY - origin.y) * invDirY;
	float tz1 = (minZ - origin.z) * invDirZ;
	float tz2 = (maxZ - origin.z) * invDirZ;
	
	float txm = max(tx1, tx2);
	float tym = max(ty1, ty2);
	float tzm = max(tz1, tz2);
	
	float tmin = min(min(txm, tym), tzm);

	vec3 hitPos = tmin * dir + origin;
	ivec3 hitBlock = ivec3(
		int(floor((tmin * 0.5) * dir.x + origin.x)),
		int(floor((tmin * 0.5) * dir.y + origin.y)),
		int(floor((tmin * 0.5) * dir.z + origin.z)));

	if (tmin == tx1) {
		return RayResult(vec3(minX, hitPos.y, hitPos.z), vec3(+1, 0, 0), hitBlock, tmin);
	}
	if (tmin == tx2) {
		return RayResult(vec3(maxX, hitPos.y, hitPos.z), vec3(-1, 0, 0), hitBlock, tmin);
	}
	if (tmin == ty1) {
		return RayResult(vec3(hitPos.x, minY, hitPos.z), vec3(0, +1, 0), hitBlock, tmin);
	}
	if (tmin == ty2) {
		return RayResult(vec3(hitPos.x, maxY, hitPos.z), vec3(0, -1, 0), hitBlock, tmin);
	}
	if (tmin == tz1) {
		return RayResult(vec3(hitPos.x, hitPos.y, minZ), vec3(0, 0, +1), hitBlock, tmin);
	}
	// Unnecessary if statement
	//else if (tmin == tz2) {
	return RayResult(vec3(hitPos.x, hitPos.y, maxZ), vec3(0, 0, -1), hitBlock, tmin);
	//}
}

bool IsValidBlock(ivec3 pos) {
	vec3 vect = vec3(pos);
	return (length(pos) > 10 && pos.y < 1) || (pos.x == 12 && floor(pos.z / 3) != 0 && pos.y < 4);
}

RayResult Raycast(vec3 rayOrigin, vec3 rayDir) {
	RayResult outRay = RayResult(vec3(0.), ivec3(0), vec3(0.), 0.);
	
	RayResult rayResult;
	rayResult = StepRay(rayOrigin, rayDir);
	outRay.dist += rayResult.dist;

	// Some weirdness with this because blockPos is for the block we're currently in
	for (int i = 0; i < MAX_LENGTH; i++) {
		RayResult currRayResult = StepRay(rayResult.hitPos, rayDir);
		outRay.dist += currRayResult.dist;
		
		if (IsValidBlock(currRayResult.hitBlock)) {
			outRay.hitPos = rayResult.hitPos;
			outRay.hitBlock = currRayResult.hitBlock;
			outRay.hitNormal = rayResult.hitNormal;
			break;
		}
		
		rayResult = currRayResult;
	}
	
	return outRay;
}

float GetLightCoef(vec3 pos, vec3 lightPos, vec3 normal) {
	vec3 toVec = lightPos - pos;
	float toMag = length(toVec);
	float toMagInv = 1. / toMag;

	vec3 dir = toVec * toMagInv;

	RayResult ray = Raycast(pos, dir);
	if (ray.dist < toMag) {
		return 0;
	}
	return toMagInv * toMagInv * max(0, dot(dir, normal));
	//return toMagInv * toMagInv * max(0, dot(dir, ray.hitNormal));
}

vec3 ShadeFace(RayResult rayResult) {
	// TODO
	float light1 = GetLightCoef(rayResult.hitPos, u_Position, rayResult.hitNormal) * 20.;
	float light2 = GetLightCoef(rayResult.hitPos, vec3(0., 6., 0.), rayResult.hitNormal) * 60.;
	return (light1 + light2 + 0.1) * vec3(1.);
}

void main() {
	vec3 rayOrigin = u_Position;
	vec3 rayDir = normalize(i_CoordPos);

	vec3 color = vec3(0.);
	
	RayResult ray = Raycast(rayOrigin, rayDir);
	
	color = ShadeFace(ray);
	
	o_Color = vec4(color, 1.);
}