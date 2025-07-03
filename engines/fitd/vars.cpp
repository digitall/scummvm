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

#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/vars.h"

namespace Fitd {

void Body::sync() {
	const uint8 *ptr = (uint8 *)m_raw;

	ptr += 2;                        // skip the flag
	ptr += 12;                       // skip the ZV
	ptr += READ_LE_S16(ptr) + 2;     // skip scratch buffer
	ptr += READ_LE_S16(ptr) * 6 + 2; // skip vertices
	const uint16 numGroups = READ_LE_U16(ptr);
	ptr += numGroups * 2 + 2; // skip group order

	assert(numGroups == m_groups.size());

	for (int i = 0; i < numGroups; i++) {
		m_groups[i].m_state.m_type = READ_LE_S16(ptr + 8);
		m_groups[i].m_state.m_delta[0] = READ_LE_S16(ptr + 10);
		m_groups[i].m_state.m_delta[1] = READ_LE_S16(ptr + 12);
		m_groups[i].m_state.m_delta[2] = READ_LE_S16(ptr + 14);
		ptr += 16;
		if (m_flags & INFO_OPTIMISE) {
			m_groups[i].m_state.m_rotateDelta[0] = READ_LE_S16(ptr + 0);
			m_groups[i].m_state.m_rotateDelta[1] = READ_LE_S16(ptr + 2);
			m_groups[i].m_state.m_rotateDelta[2] = READ_LE_S16(ptr + 4);
			ptr += 8;
		}
	}
}

} // namespace Fitd
