rd -r build

mkdir build

cd build

cl /MD /O2 /c /DLUA_BUILD_AS_DLL ../src/*.c

ren lua.obj lua.o

link /DLL /IMPLIB:lua54.lib /OUT:lua54.dll *.obj
