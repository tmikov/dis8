env = Environment()

env.AppendUnique( CCFLAGS=["-Wall","-Wextra"] )
env.AppendUnique( CCFLAGS=["-Wno-sign-compare", "-Wno-parentheses", "-Wno-switch"] )

Export('env')
env.SConscript(dirs="src", variant_dir="out", duplicate=False)