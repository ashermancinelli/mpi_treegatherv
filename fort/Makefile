
FC = mpifort

SRC = $(wildcard *.f03)
OBJ = $(SRC:.o=.f03)
FFLAGS = -ffree-form --warn-all
BIN_NAME = treegather.bin

.PHONY: Makefile clean all run

all: $(OBJ)
	$(FC) $(FFLAGS) -o $(BIN_NAME) $(OBJ)

%.o: %.f90
	$(FC) $(CFLAGS) -c $< -o $@

clean:
	if [ -f $(BIN_NAME) ]; then rm $(BIN_NAME); fi
