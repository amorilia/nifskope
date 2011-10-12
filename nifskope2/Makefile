CC       = g++
MOC	     = $(QTDIR)/bin/moc
QT4INC   = ${QTDIR}/include
QTINCS   = -I$(QT4INC) -I$(QT4INC)/Qt -I$(QT4INC)/QtCore
CXXFLAGS = -frtti -Wfatal-errors -Wall -DNIFSKOPE_QT $(QTINCS) -I.
CPPFLAGS =
CFLAGS   =
LDFLAGS  = -L${QTDIR}/lib -lQtGui
APP      = nifskope

WORK_DIR = .
MOC_DIR  = $(WORK_DIR)/moc

GLOBAL_SRC = $(wildcard $(WORK_DIR)/*.cpp)
GLOBAL_OBJ = $(patsubst %.cpp,%.o,$(GLOBAL_SRC))

# MOC_MODULES = $(patsubst %.h,moc_%,$(wildcard $(MOC_DIR)/*.h))
MOC_MODULES = $(patsubst %.h,%.moc.cpp,$(shell grep -l -R Q_OBJECT * | grep \.h$$))
MOC_CPP += $(MOC_MODULES)
MOC_CPP += $(wildcard $(MOC_DIR)/*.cpp)
MOC_OBJS = $(patsubst %.cpp,%.o,$(MOC_CPP))

%.moc.cpp: %.h
	$(MOC) $< -o $@

%.moc.o: %.moc.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

$(APP): $(GLOBAL_OBJ) $(MOC_OBJS)
	$(CC) $(GLOBAL_OBJ) $(MOC_OBJS) $(LDFLAGS) -o $(APP)

test:
	@echo "GLOBAL_SRC:" $(GLOBAL_SRC)
	@echo "GLOBAL_OBJ:" $(GLOBAL_OBJ)
	@echo "MOC_MODULES:" $(MOC_MODULES)
	@echo "MOC_CPP:" $(MOC_CPP)
	@echo "MOC_OBJS:" $(MOC_OBJS)

clean:
	rm -f $(APP) $(GLOBAL_OBJ) $(MOC_OBJS) $(MOC_MODULES)