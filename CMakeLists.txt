set(project "SpaceInvaders")
project(${project})
cmake_minimum_required(VERSION 3.0)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

ADD_EXECUTABLE(${project} 
	source/main.cpp	
	source/headers/GameWorld.h
	source/headers/GameWorld.cpp
	source/headers/GameObject.h
	source/headers/GameObject.cpp
	source/headers/Enemy.h
	source/headers/Enemy.cpp
	source/headers/Player.h
	source/headers/Player.cpp
	source/headers/Bullet.h
	source/headers/Bullet.cpp
	source/headers/EnemyBullet.h
	source/headers/EnemyBullet.cpp
	source/headers/ScoreDigit.h
	source/headers/ScoreDigit.cpp
	source/headers/Boundry.h
	source/headers/Boundry.cpp

)
TARGET_LINK_LIBRARIES(${project} ${CONAN_LIBS})

set (CMAKE_CXX_STANDARD 11)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${project})

FILE(COPY source/assets DESTINATION bin/)
