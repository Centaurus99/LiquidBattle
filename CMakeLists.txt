cmake_minimum_required(VERSION 3.0.0)
project(LiquidBattle VERSION 0.1.0)

# Config Debug
if (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_definitions(-DDEBUG)
endif()

# The Box2D library.
add_subdirectory(liquidfun-gcc9/liquidfun/Box2D)

find_package(OpenGL REQUIRED)

# Some flags for Freeglut and GLUI.
add_definitions( -DFREEGLUT_EXPORTS -DFREEGLUT_STATIC
                 -DGLUT_DISABLE_ATEXIT_HACK -D_CRT_SECURE_NO_WARNINGS )

# Define the framework files.
set(LiquidBattle_Framework_SRCS
	src/framework/Main.cpp
	src/framework/Main.h
	src/framework/Render.cpp
	src/framework/Render.h
	src/framework/Test.cpp
	src/framework/Test.h
	src/framework/Arrow.cpp
	src/framework/Arrow.h
	src/framework/FullscreenUI.cpp
	src/framework/FullscreenUI.h
	src/framework/ParticleParameter.cpp
	src/framework/ParticleParameter.h
)

#define the test files.
set(LiquidBattle_Tests_SRCS
	src/MyDebug.h
	src/CollisionData.h
	src/Player.h
	src/Obstacle.h
	src/ColorSet.h
	src/LiquidBattle.h
	src/LiquidBattle.cpp
)

# These are used to create visual studio folders.
source_group(Framework FILES ${LiquidBattle_Framework_SRCS})
source_group(Tests FILES ${LiquidBattle_Tests_SRCS})

include_directories (
	${OPENGL_INCLUDE_DIR}
	${Box2D_SOURCE_DIR}
	/usr/include
	liquidfun-gcc9/freeglut/include
)

if(APPLE)
	# We are not using the Apple's framework version, but X11's
	include_directories( /usr/X11/include )
	link_directories( /usr/X11/lib/ )
	find_library(OPENGL_LIBRARIES NAMES GL GLU X11 HINTS /usr/X11/lib)
elseif(WIN32)
	set (ADDITIONAL_LIBRARIES winmm)
endif(APPLE)

add_executable(LiquidBattle
	${LiquidBattle_Framework_SRCS}
	${LiquidBattle_Tests_SRCS}
)

target_link_libraries (
	LiquidBattle
	Box2D
	freeglut_static
	glui
	${ADDITIONAL_LIBRARIES}
	${OPENGL_LIBRARIES}
)
