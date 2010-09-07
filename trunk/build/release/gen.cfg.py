sourcePaths = ["../../src/Math/", "../../src/Util/Tokenizer/", "../../src/Misc/", "../../src/", "../../src/Renderer/", "../../src/Scene/", "../../src/Ui/", "../../src/Resources/", "../../src/Util/", "../../src/Scene/Controllers/", "../../src/Physics/", "../../src/Renderer/BufferObjects/", "../../src/Resources/Helpers/", "../../src/Resources/Core/", "../../src/Core/", "../../src/Scripting/", "../../src/Scripting/Math", "../../src/Scripting/Util", "../../src/Scripting/Core", "../../src/Scripting/Scene"]

includePaths = []
includePaths.append("./")
includePaths.extend(list(sourcePaths))
includePaths.extend(["../../extern/include", "../../extern/include/bullet", "/usr/include/python2.6"])

executableName = "anki"

compiler = "g++"

compilerFlags = "-DDEBUG_ENABLED=0 -DPLATFORM_LINUX -c -pedantic-errors -pedantic -ansi -Wall -Wextra -W -Wno-long-long -pipe -s -msse4 -O3 -mtune=core2 -ffast-math -fsingle-precision-constant"

linkerFlags = "-rdynamic -L../../extern/lib-x86-64-linux -Wl,-Bstatic -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lGLEW -lGLU -lboost_system -lboost_python -lboost_filesystem -Wl,-Bdynamic -lGL -ljpeg -lSDL -lpng -lpython2.6"
