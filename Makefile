CC=musl-gcc
BIN=scb

all: $(BIN)

$(BIN): %: %.c
	$(CC) -o $@ $< -static

parallel:
	$(CC) -o parallel_scb parallel_scb.c -static

strip:
	strip -S \
		--strip-unneeded \
		--remove-section=.note.gnu.gold-version \
		--remove-section=.comment \
		--remove-section=.note \
		--remove-section=.note.gnu.build-id \
		--remove-section=.note.ABI-tag $(BIN)

install:
	cp $(BIN) /usr/bin/$(BIN)
	cp $(BIN).1 /usr/share/man/man1/

clean:
	rm $(BIN) parallel_scb
