CC       = g++
MOC	     = $(QTDIR)/bin/moc
QT4INC   = ${QTDIR}/include
QTINCS   = -I$(QT4INC) -I$(QT4INC)/Qt -I$(QT4INC)/QtCore -I$(QT4INC)/QtOpenGL
OGREINC  = -I$(OGRE_ROOT)/include/OGRE
WORK_DIR = .
MOC_DIR  = $(WORK_DIR)/moc
NIFLIB_DIR = $(WORK_DIR)/niflib
NIFLIB_INC = -I$(NIFLIB_DIR)
OPTNS    = -DNIFSKOPE_QT -DNIFSKOPE_X -DNIFSKOPE_OGRE_GL
CXXFLAGS = -g -frtti -Wfatal-errors -Wall $(OPTNS) $(QTINCS) $(OGREINC) $(NIFLIB_INC) -I.
CPPFLAGS =
CFLAGS   =
LDFLAGS  = -L${QTDIR}/lib -L${OGRE_ROOT}/lib -lQtGui -lQtOpenGL -lX11 -lOgreMain
APP      = nifskope

GLOBAL_SRC = $(wildcard $(WORK_DIR)/*.cpp)
GLOBAL_OBJ = $(patsubst %.cpp,%.o,$(GLOBAL_SRC))

# MOC_MODULES = $(patsubst %.h,moc_%,$(wildcard $(MOC_DIR)/*.h))
MOC_MODULES = $(patsubst %.h,%.moc.cpp,$(shell grep -l -R Q_OBJECT * | grep \.h$$))
MOC_CPP += $(MOC_MODULES)
MOC_CPP += $(wildcard $(MOC_DIR)/*.cpp)
MOC_OBJS = $(patsubst %.cpp,%.o,$(MOC_CPP))

NIFLIB_CPP = $(wildcard $(NIFLIB_DIR)/*.cpp)
NIFLIB_CPP := $(patsubst $(NIFLIB_DIR)/niflib++.cpp,,$(NIFLIB_CPP)) 
NIFLIB_OBJ = $(patsubst %.cpp,%.o,$(NIFLIB_CPP))

%.moc.cpp: %.h
	$(MOC) $< -o $@

%.moc.o: %.moc.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

$(APP): $(GLOBAL_OBJ) $(MOC_OBJS) $(NIFLIB_OBJ)
	$(CC) $(GLOBAL_OBJ) $(MOC_OBJS) $(NIFLIB_OBJ) $(LDFLAGS) -o $(APP)

test:
	@echo "GLOBAL_SRC:" $(GLOBAL_SRC)
	@echo "GLOBAL_OBJ:" $(GLOBAL_OBJ)
	@echo "MOC_MODULES:" $(MOC_MODULES)
	@echo "MOC_CPP:" $(MOC_CPP)
	@echo "MOC_OBJS:" $(MOC_OBJS)
	@echo "NIFLIB_CPP:" $(NIFLIB_CPP)
	@echo "NIFLIB_OBJ:" $(NIFLIB_OBJ)

clean:
	rm -f $(APP) $(GLOBAL_OBJ) $(MOC_OBJS) $(MOC_MODULES)