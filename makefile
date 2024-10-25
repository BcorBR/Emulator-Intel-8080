all:
	gcc src/main.c src/processor/processor.c src/IO/IO.c src/interrupt/interrupt.c src/graphics/render.c src/FileHandle/file.c Disassembler/disDebug.c -lSDL2 -o main

debug:
	gcc src/main.c src/processor/processor.c src/IO/IO.c src/interrupt/interrupt.c src/graphics/render.c src/FileHandle/file.c Disassembler/disDebug.c -lSDL2 -o main -g