CC       = g++
#CXXFLAGS = -g -Wfatal-errors -Wall -std=gnu++0x
CXXFLAGS = -g -Wfatal-errors -Wall
CPPFLAGS =
CFLAGS   =
LDFLAGS  =
APP      = niflib++

WORK_DIR = .

GLOBAL_SRC = $(wildcard $(WORK_DIR)/*.cpp)
GLOBAL_OBJ = $(patsubst %.cpp,%.o,$(GLOBAL_SRC))

$(APP): $(GLOBAL_OBJ)
	$(CC) $(GLOBAL_OBJ) $(LDFLAGS) -o $(APP)

test:
	@echo "GLOBAL_SRC:" $(GLOBAL_SRC)
	@echo "GLOBAL_OBJ:" $(GLOBAL_OBJ)

clean:
	rm -f $(APP) $(GLOBAL_OBJ)