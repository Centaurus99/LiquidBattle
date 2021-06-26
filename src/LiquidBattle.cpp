/*
 * @Author: Tong Haixuan
 * @Date: 2021-06-26 17:28:50
 * @LastEditTime: 2021-06-26 21:11:00
 * @LastEditors: Tong Haixuan
 * @Description: Generate Entries
 */
/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
* Copyright (c) 2013 Google, Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "framework/Test.h"
#include "framework/Render.h"
#include "framework/Main.h"
#include "framework/ParticleParameter.h"

#if defined(__APPLE__) && !defined(__IOS__)
	#include <GLUT/glut.h>
#else
	#include "GL/freeglut.h"
#endif

#include "LiquidBattle.h"

TestEntry g_testEntries[] =
{
	{"LiquidBattle", LiquidBattle::Create},
	{NULL, NULL}
};
