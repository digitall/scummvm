/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include <assert.h>

#include "cruise/cruise_main.h"
#include "stack.h"

namespace Cruise {

// 4 type bigger than the old one, but much safer/cleaner
StackElement scriptStack[SIZE_STACK];

// VAR

StackElement::StackElement(int16 var) {
	_data._shortVar = var;
	_type = STACK_SHORT;
}
void Stack::pushVar(int16 var) {
	StackElement temp(var);
	push(temp);
}

int16 Stack::popVar() {
	if (!empty()) {
		StackElement temp = pop();
		if (temp._type == STACK_SHORT)
			return temp._data._shortVar;
		else
			assert(0);
	} else {
		return 0;
	}
}

//// PTR

StackElement::StackElement(void *ptr) {
	_data._ptrVar = ptr;
	_type = STACK_PTR;
}

void Stack::pushPtr(void *ptr) {
	StackElement temp(ptr);
	push(temp);
}

void *Stack::popPtr() {
	if (!empty()) {
		StackElement temp = pop();
		if (temp._type == STACK_PTR)
			return temp._data._ptrVar;
		else
			assert(0);
	} else {
		return 0;
	}
}

//// MISC

void Stack::reset() {
	clear();
}
} // End of namespace Cruise
