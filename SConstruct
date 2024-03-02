#!/usr/bin/env python
import os
import sys
import SCons

env = SConscript("../godot-cpp/SConstruct")

box2d_folder = "box2d/"

box2d_include = [
	"include/",
	"src/"
]

def GlobRecursive(pattern, node='.'):
    results = []
    for f in Glob(str(node) + '/*', source=True):
        if type(f) is SCons.Node.FS.Dir:
            results += GlobRecursive(pattern, f)
    results += Glob(str(node) + '/' + pattern, source=True)
    return results

env.Prepend(CPPPATH=[box2d_folder + folder for folder in box2d_include])

# For the reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# Make Box2D include "b2_user_settings.h"
env.Append(CPPDEFINES="B2_USER_SETTINGS")

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/"])
#sources = [Glob("src/*.cpp"),Glob("src/bodies/*.cpp"),Glob("src/joints/*.cpp"),Glob("src/servers/*.cpp"),Glob("src/shapes/*.cpp"),Glob("src/spaces/*.cpp")]
sources = Glob("src/*.cpp")
#sources.extend([box2d_folder + 'src/' + box2d_src_file for box2d_src_file in box2d_src])
sources += GlobRecursive("*.cpp",box2d_folder + 'src/')

if env["platform"] == "macos":
	library = env.SharedLibrary(
		"../../bin/libphysics_server_box2d.{}.{}.framework/libphysics_server_box2d.{}.{}".format(
			env["platform"], env["target"], env["platform"], env["target"]
		),
		source=sources,
	)
else:
	library = env.SharedLibrary(
		"../../bin/libphysics_server_box2d{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
		source=sources,
	)

Default(library)
