PROJECT           := flexlog
LIBRARY           := lib$(PROJECT).so

DESTDIR           ?=
PREFIX            := /usr
BIN_DIR           := $(DESTDIR)$(PREFIX)/bin
HDR_DIR           := $(DESTDIR)$(PREFIX)/include/$(PROJECT)
LIB_DIR           := $(DESTDIR)$(PREFIX)/lib

## -- Compiler Settings
CROSS             ?=
CC                := $(CROSS)clang
STD               := -std=gnu99

override CPPFLAGS += -I.
override CFLAGS   += $(STD) -Os -Wall -Werror -Wextra -MP -MMD
override CFLAGS   += -D_GNU_SOURCE
override LDFLAGS  += -L.
override LDLIBS   += -lpthread

SOURCES           := $(shell find . -name "*.c" -print)
OBJECTS            = $(addsuffix .o,$(basename $(SOURCES)))
DEPENDS            = $(addsuffix .d,$(basename $(OBJECTS)))


## -- Compilation
.PHONY: all debug lib
.DELETE_ON_ERROR:

all: CFLAGS       += -DNDEBUG
all: $(OBJECTS)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJECTS) -o $(PROJECT)

debug: CFLAGS     += -g
debug: $(OBJECTS)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJECTS) -o $(PROJECT)

lib: CFLAGS       += -fPIC -DNDEBUG
lib: LDLIBS       += -lc
lib: $(OBJECTS)
	$(CC) $(LDFLAGS) $(LDLIBS) -shared $(OBJECTS) -o $(LIBRARY)

-include $(DEPENDS)


.PHONY: clean distclean install run uninstall
.ONESHELL:

clean:
	@find . -type f -executable | xargs $(RM)
	@find . -name "*.[ado]" | xargs $(RM)

distclean: uninstall clean

install: uninstall
ifneq ("$(wildcard $(PROJECT))","")
	@cp $(PROJECT) $(BIN_DIR)
else ifneq ("$(wildcard $(LIBRARY))","")
	@mkdir -p $(HDR_DIR)
	@find . -type f -name "*.h" | xargs -I {} cp --parents {} $(HDR_DIR)
	@cp $(LIBRARY) $(LIB_DIR)
endif

run:
ifneq ("$(wildcard $(PROJECT))","")
	@./$(PROJECT)
endif

uninstall:
ifneq ("$(wildcard $(PROJECT))","")
	@$(RM) -rf $(BIN_DIR)/$(PROJECT)
else ifneq ("$(wildcard $(LIBRARY))","")
	@$(RM) -rf $(HDR_DIR)
	@$(RM) -rf $(LIB_DIR)/$(LIBRARY)
endif


.PHONY: cppcheck cpplint style valgrind
.ONESHELL:

cppcheck:
	@cppcheck --enable=all --std=c99 --std=posix \
		--suppress=missingIncludeSystem \
		-f .

cpplint:
	@cpplint --linelength=80 --recursive \
		--filter=-build,-readability/casting \
		.

style:
	@astyle -Qnrv --style=google --indent=spaces=4 --lineend=linux \
		--convert-tabs --indent-switches \
		"*.c" "*.h"

valgrind: clean debug
	@valgrind --leak-check=full --show-leak-kinds=all ./$(PROJECT)
