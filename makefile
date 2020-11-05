


main: main.o charset.o input.o sdl.o
	clang objs/main.o objs/charset.o objs/input.o objs/sdl.o -lSDL2 -lSDL2_ttf -lSDL2_image -o bin/main.game -no-pie

main.o:
	clang -c src/main.c -o objs/main.o -I/usr/include/SDL2 -Isrc/

charset.o:
	clang -c src/charset/charset.c -o objs/charset.o -I/usr/include/SDL2 -Isrc/

input.o:
	clang -c src/input/input.c -o objs/input.o -I/usr/include/SDL2 -Isrc/

sdl.o:
	clang -c src/utils/sdl.c -o objs/sdl.o -I/usr/include/SDL2 -Isrc/