EXE=test
CC=gcc
CFLAGS=-MMD -O3
DEFS=
COMPILE=$(CC) $(CFLAGS) $(DEFS)
LDFLAGS=
LDLIBS=

SRC_DIR:=src
OBJ_DIR:=.obj
$(shell mkdir -p $(OBJ_DIR))

C_SRC:=test.c util.c
CC_SRC:=
OBJS:=$(C_SRC:%.c=$(OBJ_DIR)/%.o) $(CC_SRC:%.cc=$(OBJ_DIR)/%.o)
DEPS:=$(C_SRC:%.c=$(OBJ_DIR)/%.d) $(CC_SRC:%.cc=$(OBJ_DIR)/%.d)

all: $(EXE)

EXTS=c cc
define make_rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$1
	$$(COMPILE) -o $$@ -c $$<
endef
$(foreach EXT,$(EXTS),$(eval $(call make_rule,$(EXT))))

$(EXE): $(OBJS)
	$(COMPILE) $(LDFLAGS) $^ $(LDLIBS) -o $@

-include $(DEPS)

fresh: clean all

PHONY: clean

clean:
	rm -rf $(EXE) $(OBJ_DIR)/* *.dSYM *.gch