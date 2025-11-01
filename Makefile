CC      = clang
CFLAGS  = -std=c99 -Wall -pedantic
SRC_DIR = .
BIN_DIR = bin

TARGETS = $(BIN_DIR)/preorder \
          $(BIN_DIR)/inorder \
          $(BIN_DIR)/postorder \
          $(BIN_DIR)/calculate \
          $(BIN_DIR)/heap

all: $(BIN_DIR) $(TARGETS)

$(BIN_DIR)/preorder: $(SRC_DIR)/preorder.c
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_DIR)/inorder: $(SRC_DIR)/inorder.c
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_DIR)/postorder: $(SRC_DIR)/postorder.c
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_DIR)/calculate: $(SRC_DIR)/calculate.c
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_DIR)/heap: $(SRC_DIR)/heap.c
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -f $(TARGETS)
