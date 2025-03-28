/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef FITD_HQR_H
#define FITD_HQR_H

#include "fitd/common.h"
#include "fitd/hqr.h"
#include "fitd/vars.h"
#include "fitd/gfx.h"
#include "common/array.h"

namespace Fitd {

Common::Array<sBody *> vBodies;

static sBody *createBodyFromPtr(void *ptr) {
	uint8 *bodyBuffer = (uint8 *)ptr;

	sBody *newBody = new sBody;

	newBody->m_raw = ptr;
	newBody->m_flags = READ_LE_U16(bodyBuffer);
	bodyBuffer += 2;

	newBody->m_zv.ZVX1 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_zv.ZVX2 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_zv.ZVY1 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_zv.ZVY2 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_zv.ZVZ1 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_zv.ZVZ2 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;

	uint16 scratchBufferSize = READ_LE_U16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_scratchBuffer.resize(scratchBufferSize);
	for (int i = 0; i < scratchBufferSize; i++) {
		newBody->m_scratchBuffer[i] = READ_LE_U8(bodyBuffer);
		bodyBuffer += 1;
	}

	uint16 numVertices = READ_LE_U16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_vertices.resize(numVertices);
	for (int i = 0; i < numVertices; i++) {
		newBody->m_vertices[i].x = READ_LE_S16(bodyBuffer);
		bodyBuffer += 2;
		newBody->m_vertices[i].y = READ_LE_S16(bodyBuffer);
		bodyBuffer += 2;
		newBody->m_vertices[i].z = READ_LE_S16(bodyBuffer);
		bodyBuffer += 2;
	}

	if (newBody->m_flags & INFO_TORTUE) {
		assert(0); // never used
	}

	if (newBody->m_flags & INFO_ANIM) {
		uint16 numGroups = READ_LE_U16(bodyBuffer);
		bodyBuffer += 2;
		newBody->m_groupOrder.reserve(numGroups);
		newBody->m_groups.resize(numGroups);

		if (newBody->m_flags & INFO_OPTIMISE) // AITD2+
		{
			for (int i = 0; i < numGroups; i++) {
				uint16 offset = READ_LE_U16(bodyBuffer);
				assert(offset % 0x18 == 0);
				newBody->m_groupOrder.push_back(offset / 0x18);
				bodyBuffer += 2;
			}

			for (int i = 0; i < numGroups; i++) {
				newBody->m_groups[i].m_start = READ_LE_S16(bodyBuffer) / 6;
				bodyBuffer += 2;
				newBody->m_groups[i].m_numVertices = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_baseVertices = READ_LE_S16(bodyBuffer) / 6;
				bodyBuffer += 2;
				newBody->m_groups[i].m_orgGroup = READ_LE_S8(bodyBuffer);
				bodyBuffer += 1;
				newBody->m_groups[i].m_numGroup = READ_LE_S8(bodyBuffer);
				bodyBuffer += 1;
				newBody->m_groups[i].m_state.m_type = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[0] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[1] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[2] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_rotateDelta[0] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_rotateDelta[1] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_rotateDelta[2] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				bodyBuffer += 2; // padding?
			}
		} else {
			for (int i = 0; i < numGroups; i++) {
				uint16 offset = READ_LE_U16(bodyBuffer);
				assert(offset % 0x10 == 0);
				newBody->m_groupOrder.push_back(offset / 0x10);
				bodyBuffer += 2;
			}

			for (int i = 0; i < numGroups; i++) {
				newBody->m_groups[i].m_start = READ_LE_S16(bodyBuffer) / 6;
				bodyBuffer += 2;
				newBody->m_groups[i].m_numVertices = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_baseVertices = READ_LE_S16(bodyBuffer) / 6;
				bodyBuffer += 2;
				newBody->m_groups[i].m_orgGroup = READ_LE_S8(bodyBuffer);
				bodyBuffer += 1;
				newBody->m_groups[i].m_numGroup = READ_LE_S8(bodyBuffer);
				bodyBuffer += 1;
				newBody->m_groups[i].m_state.m_type = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[0] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[1] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[2] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
			}
		}
	}

	uint16 numPrimitives = READ_LE_U16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_primitives.resize(numPrimitives);
	for (int i = 0; i < numPrimitives; i++) {
		newBody->m_primitives[i].m_type = (primTypeEnum)READ_LE_U8(bodyBuffer);
		bodyBuffer += 1;

		switch (newBody->m_primitives[i].m_type) {
		case primTypeEnum_Line:
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_even = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_points.resize(2);
			for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			break;
		case primTypeEnum_Poly:
			newBody->m_primitives[i].m_points.resize(READ_LE_U8(bodyBuffer));
			bodyBuffer += 1;
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			break;
		case primTypeEnum_Point:
		case primTypeEnum_BigPoint:
		case primTypeEnum_Zixel:
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_even = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_points.resize(1);
			for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			break;
		case primTypeEnum_Sphere:
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_even = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_size = READ_LE_U16(bodyBuffer);
			bodyBuffer += 2;
			newBody->m_primitives[i].m_points.resize(1);
			for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			break;
		case processPrim_PolyTexture8:
			newBody->m_primitives[i].m_points.resize(READ_LE_U8(bodyBuffer));
			bodyBuffer += 1;
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			break;
		case processPrim_PolyTexture9:
		case processPrim_PolyTexture10:
			newBody->m_primitives[i].m_points.resize(READ_LE_U8(bodyBuffer));
			bodyBuffer += 1;
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			// load UVS?
			for (int j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				READ_LE_U8(bodyBuffer);
				bodyBuffer += 1;
				READ_LE_U8(bodyBuffer);
				bodyBuffer += 1;
			}
			break;
		default:
			assert(0);
			break;
		}
	}

	vBodies.push_back(newBody);
	return newBody;
}

sBody *getBodyFromPtr(void *ptr) {
	for (int i = 0; i < vBodies.size(); i++) {
		if (vBodies[i]->m_raw == ptr) {
			return vBodies[i];
		}
	}

	return createBodyFromPtr(ptr);
}

} // namespace Fitd

#endif
