cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER_WORKS FALSE)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(PotetreVoxels)

include_directories(
    "include/"
    "3rdparty/"
    "3rdparty/pl_synth/"
    "3rdparty/sts_mixer/"
    "src/"
)

set(COMMON_SRC
    "3rdparty/pl_synth/pl_synth.c"
    "3rdparty/sts_mixer/sts_mixer.c"
    "src/engine/audio.c"
    "src/engine/camera.c"
    "src/engine/perlin.c"
    "src/engine/lut.c"
    "src/engine/light.c"
    "src/engine/world.c"
    "src/engine/render_draw.c"
    "src/engine/render_core.c"
    "src/engine/sprite.c"
    "src/engine/text.c"
    "src/engine/vec3.c"
    "src/engine/utils.c"
    "src/engine/actor_core.c"
    "src/engine/actor_physics.c"
    "src/engine/collision.c"
    "src/engine/aabb.c"
    "src/engine/geometry.c"
    "src/level/gen.c"
    "src/level/level.c"
    "src/level/spawn.c"
    "src/level/bsp.c"
    "src/enemies/basic.c"
    "src/game.c"
    "src/game_new.c"
    "src/game_lose.c"
    "src/game_end.c"
    "src/player/player.c"
    "src/player/hud.c"
    "src/player/stats.c"
    "src/player/weapons.c"
    "src/textures.c"
    "src/menu.c"
    "src/wormhole_actor.c"
    "src/wormhole_state.c"
    "src/tunnel.c"
    "src/objects/grenade.c"
    "src/upgrades.c"
    "src/palettes.c"
    "src/sound_atlas.c"
    "src/sprite_atlas.c"
)

################################################################################
# Backend

# @TODO: Split audio engine to own "backend"
if(BACKEND STREQUAL "X11")
    set(BACKEND_SRC
        "src/engine/platform/linux/platform_x11.c"
        "src/engine/platform/linux/platform_posix.c"
        "src/engine/platform/linux/platform_audio_alsa.c"
    )
    set(BACKEND_LIB
        X11
        GLX
        GL
        asound
    )
elseif(BACKEND STREQUAL "Null")
    set(BACKEND_SRC
        "src/engine/platform/other/platform_null_audio.c"
        "src/engine/platform/other/platform_null_thread.c"
        "src/engine/platform/other/platform_null_render.c"
    )
    set(BACKEND_LIB
    )
elseif(BACKEND STREQUAL "Raylib")
    include(FetchContent)
    set(RAYLIB_VERSION 5.5)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        FIND_PACKAGE_ARGS ${RAYLIB_VERSION} EXACT
    )
    set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(raylib)

    # Add backend
    set(BACKEND_SRC
        "src/engine/platform/generic/platform_raylib_desktop.c"
    )

    # Check system sanity
    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(BACKEND_LIB
            GL
            X11
            rt
            dl
            raylib
        )
    elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
        # MinGW only
        set(BACKEND_SRC
            "${BACKEND_SRC}"
            "src/engine/platform/linux/platform_posix.c"
            "src/engine/platform/windows/platform_audio_winmm.c"
        )
        set(CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive \
            -lssp -lwinpthread  -Wl,-Bdynamic,--no-whole-archive \
            "
        )
        set(BACKEND_LIB
            opengl32
            gdi32
            winmm
            raylib
        )
    elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set(BACKEND_LIB
            raylib
        )
        set(BACKEND_SRC
            "${BACKEND_SRC}"
            "src/engine/platform/other/platform_null_thread.c"
            "src/engine/platform/other/platform_null_audio.c"
        )
        set(CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_EXE_LINKER_FLAGS} -framework IOKit -framework Cocoa \
            -framework OpenGL"
        )
    else()
        message(FATAL_ERROR
            "Raylib backend: OS Not supported (yet)"
        )
    endif()
else()
    message(FATAL_ERROR
        "Unknown Backend! Supported: X11, Raylib, Null"
    )
endif()

################################################################################
# Make exe

add_executable(PotetreVoxels
    "${COMMON_SRC}"
    "${BACKEND_SRC}"
)

################################################################################
# Build

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Aggressive size-optimized target
    set(CMAKE_C_FLAGS_RELEASE "")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -DNDEBUG")

    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(
            CMAKE_C_FLAGS
            "${CMAKE_C_FLAGS} \
            -Oz -DNDEBUG \
            -flto -fuse-linker-plugin  -ffast-math -fipa-icf \
            -fno-plt -fno-stack-protector -fno-math-errno -fno-unwind-tables \
            -fno-asynchronous-unwind-tables -fomit-frame-pointer \
            -fno-PIE -fno-PIC \
            -ffunction-sections -fdata-sections \
            -fno-stack-clash-protection -mpreferred-stack-boundary=4 \
            -fmerge-all-constants -fsingle-precision-constant -fno-common \
            -fno-unroll-loops -fno-jump-tables -fwhole-program"
        )

        set_target_properties(
            PotetreVoxels PROPERTIES LINK_FLAGS
            "-Wl,--build-id=none -Wl,-z,norelro -Wl,-z,noseparate-code \
            -Wl,--no-eh-frame-hdr \
            -Wl,--no-ld-generated-unwind-info -Wl,--hash-style=sysv \
            -mpreferred-stack-boundary=4 \
            -Wl,-z,nodynamic-undefined-weak \
            -Wl,--gc-sections \
            -Wl,--strip-all -Wl,--discard-all \
            -fno-exceptions -fno-rtti -fno-threadsafe-statics \
            -flto -no-pie \
            -nostartfiles -e _start"
        )
    else()
        # Other systems, normal build
        set(
            CMAKE_C_FLAGS
            "${CMAKE_C_FLAGS} \
            -Oz -DNDEBUG \
            -DUSE_MAIN"
        )

        set_target_properties(
            PotetreVoxels PROPERTIES LINK_FLAGS
            "-DUSE_MAIN \
            -flto \
            "
        )
    endif()

elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")

    set(CMAKE_C_FLAGS_DEBUG "")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
        -g3 -O0 -DDEBUG -DLOGGING \
        -DUSE_MAIN \
    ")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
        -g3 -DDEBUG -DLOGGING \
        -DUSE_MAIN \
    ")

elseif(CMAKE_BUILD_TYPE STREQUAL "Profiling")

    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
        -g3 -O1 -pg -fprofile-arcs -ftest-coverage \
        -DDEBUG -DLOGGING -DUSE_MAIN \
    ")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
        -g3 -DDEBUG -pg -fprofile-arcs -ftest-coverage \
        -DUSE_MAIN \
    ")

elseif(CMAKE_BUILD_TYPE STREQUAL "Optimized")

    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
        -O2 -DUSE_MAIN -DNDEBUG -flto \
        -ffunction-sections -fdata-sections \
    ")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
        -DNDEBUG -DUSE_MAIN -flto -Wl,--gc-sections -s \
    ")
else()
    message(FATAL_ERROR
        "Unknown target! Supported: Debug, Release, Optimized and Profiling"
    )
endif()


################################################################################
# Libs link

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(
        PotetreVoxels
        ${BACKEND_LIB}
    )
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(
        PotetreVoxels
        m
        ${BACKEND_LIB}
    )
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    target_link_libraries(
        PotetreVoxels
        m
        ${BACKEND_LIB}
    )
endif()
