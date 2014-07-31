env = Environment()

env.AppendUnique( CCFLAGS=["-Wall","-Wextra"] )
env.AppendUnique( CCFLAGS=["-Wno-sign-compare", "-Wno-parentheses", "-Wno-switch"] )
env.AppendUnique( CCFLAGS=["-Wno-unused-parameter"] )
env.AppendUnique( CCFLAGS="-g" )

Export('env')
env.SConscript(dirs="src", variant_dir="out", duplicate=False)