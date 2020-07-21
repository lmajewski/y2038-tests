# Makefile for custom apps to include in the image
# Pseudo targets:
#
# - clean: remove all build aterfacts
#
# - all: build the init executable
#
# - run: try and cross-run init on host through QEMU
#
# Default is 'all'

.PHONY: all install clean

all: test_n2038 test_y2038
test_y2038: $(patsubst %.c,%.64.o,$(wildcard *.c))
	@echo "LD64  $^"
	$(CC) $(LDFLAGS) -o test_y2038 $^ -lrt -lpthread

test_n2038: $(patsubst %.c,%.32.o,$(wildcard *.c))
	@echo "LD32  $^"
	$(CC) $(LDFLAGS) -o test_n2038 $^ -lrt -lpthread

%.64.o: %.c %.64.d
	@echo "CC64  $<"
	@$(CC) $(CFLAGS) -c -save-temps -o $@ -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64 $<

%.64.d: %.c
	@$(CC) $(CFLAGS) -M -save-temps -o $@ -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64 $<
%.32.o: %.c %.32.d
	@echo "CC32  $<"
	@$(CC) $(CFLAGS) -c -o $@ -D_FILE_OFFSET_BITS=64 $<

%.32.d: %.c
	@$(CC) $(CFLAGS) -M -o $@ $<

install: all
	install -m 755 test_n2038 $(INSTALL_DIR)
	install -m 755 test_y2038 $(INSTALL_DIR)

clean:
	rm -f test_[ny]2038 *.o *.d *.i *.s
