// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#include "tests/framework/Framework.h"
#include "anki/script/ScriptManager.h"
#include "anki/Math.h"

static const char* script = R"(
b = Vec2.new(3.0, 4.0)
v2:copy(v2 * b)

v3:setZ(0.1)
)";

ANKI_TEST(Script, LuaBinder)
{
	HeapAllocator<U8> alloc(HeapMemoryPool(allocAligned, nullptr));
	ScriptManager sm(alloc);
	Vec2 v2(2.0, 3.0);
	Vec3 v3(1.1, 2.2, 3.3);

	sm.exposeVariable("v2", &v2);
	sm.exposeVariable("v3", &v3);

	sm.evalString(script);

	ANKI_TEST_EXPECT_EQ(v2, Vec2(6, 12));
	ANKI_TEST_EXPECT_EQ(v3, Vec3(1.1, 2.2, 0.1));
}
