#
# GNU Makefile for NifSkope 2
#
# Prerequisites:
#  - setup QTDIR to point to "Qt4" root folder
#  - GNU findutils
#  - GNU grep
#
# Targets:
#  ""      - build NifSkope
#  "test"  - display some variables - for debug purposes
#  "clean" - remove built binaries
#
# TODO: "autotools"/"cmake"
#
# Feats:
#  - short
#  - simple
#  - build_time--;
#

CC       = gcc

# Qt 4 related
MOC	     = $(QTDIR)/bin/moc
RCC      = $(QTDIR)/bin/rcc
QT4INC   = $(QTDIR)/include
QT4LIB   = -L$(QTDIR)/lib
QT4LIBS  = -lQtGui -lQtOpenGL -lQtXml -lQtNetwork -lQtCore
QTINCS   = -I$(QT4INC) -I$(QT4INC)/Qt -I$(QT4INC)/QtCore -I$(QT4INC)/QtOpenGL -I$(QT4INC)/QtGui -I$(QT4INC)/QtNetwork

# X11
X11LIBS  = -lX11

# Open GL
OGLLIBS  = -lGLU -lGL

# System
SYSLIBS  = -lpthread

# Application
LIBS     = $(QT4LIB) $(QT4LIBS) $(X11LIBS) $(OGLLIBS) $(SYSLIBS)
WORK_DIR = .
QHULLDIR = $(WORK_DIR)/qhull
RESDIR   = $(WORK_DIR)/res
HEADERS  = -Iinclude -I. -Isrc
INCS     = $(HEADERS) $(QTINCS)
NSOPTNS  = -DFSENGINE -DNIFSKOPE_QT -DNIFSKOPE_X -DNIFSKOPE_OGRE_GL
QT4OPTNS = -DQT_XML_LIB -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
OPTNS    = $(NSOPTNS) $(QT4OPTNS)
CXXFLAGS = -g -frtti -Wfatal-errors -Wall $(OPTNS) $(INCS)
CPPFLAGS =
CFLAGS   =
LDFLAGS  = $(LIBS)
APP      = nifskope

GLOBAL_SRC = $(shell find $(WORK_DIR)/ -type f -name '*.cpp' | grep -v -e '$(QHULLDIR)/' -e '\.moc.cpp$$' -e '\.qrc.cpp$$' -e 'test/')
GLOBAL_OBJ = $(patsubst %.cpp,%.o,$(GLOBAL_SRC))

MOC_MODULES = $(patsubst %.h,%.moc.cpp,$(shell grep -l -R Q_OBJECT * | grep -v -e 'qhull/' -e 'test/' | grep \.h$$))
MOC_OBJS = $(patsubst %.cpp,%.o,$(MOC_MODULES))

RCC_QRC = $(patsubst %.qrc,%.qrc.cpp,$(wildcard $(RESDIR)/*.qrc))
RCC_OBJ = $(patsubst %.cpp,%.o,$(RCC_QRC))

.PHONY: all test $(APP) clean unittest

all: $(APP)

%.qrc::
%.qrc.cpp: %.qrc
	$(RCC) -name $(APP) $< -o $@

%.moc.cpp: %.h
	$(MOC) $< -o $@

config.h: config.h.in
	$(shell $(WORKDIR)./makeconfig.sh)

$(APP): $(GLOBAL_OBJ) $(MOC_OBJS) $(RCC_OBJ)
	$(CC) $(GLOBAL_OBJ) $(MOC_OBJS) $(RCC_OBJ) $(LDFLAGS) -o $(APP)

test: $(GLOBAL_SRC)
	@echo "GLOBAL_SRC:" $(GLOBAL_SRC)
	@echo "GLOBAL_OBJ:" $(GLOBAL_OBJ)
	@echo "MOC_MODULES:" $(MOC_MODULES)
	@echo "MOC_OBJS:" $(MOC_OBJS)
	@echo "RCC_QRC:" $(RCC_QRC)
	@echo "RCC_OBJ:" $(RCC_OBJ)


TEST_OBJS = src/ns_utils.o test/test_main.o
TEST_APP = test_main
unittest: $(TEST_APP)
$(TEST_APP): $(TEST_OBJS)
	$(CC) $^ $(LDFLAGS) -o $@
#	$(CC) $(TEST_OBJS) $(LDFLAGS) -o $(TEST_APP)

clean:
	rm -f $(APP) $(GLOBAL_OBJ) $(MOC_OBJS) $(RCC_OBJ)

# Auto-generated header dependency list
# includes config.h generation when needed
src/gl/dds/Image.o:  src/gl/dds/Color.h src/gl/dds/Image.h 
src/gl/dds/DirectDrawSurface.o:  src/gl/dds/DirectDrawSurface.h src/gl/dds/BlockDXT.h src/gl/dds/PixelFormat.h  
src/gl/dds/dds_api.o:  src/gl/dds/dds_api.h src/gl/dds/Stream.h src/gl/dds/DirectDrawSurface.h 
src/gl/dds/ColorBlock.o:  src/gl/dds/ColorBlock.h src/gl/dds/Image.h src/gl/dds/Common.h
src/gl/dds/Stream.o:  src/gl/dds/Stream.h  
src/gl/dds/BlockDXT.o:  src/gl/dds/Common.h src/gl/dds/Stream.h src/gl/dds/ColorBlock.h src/gl/dds/BlockDXT.h
src/gl/glmesh.o:  include/ns_base.h src/gl/glscene.h src/gl/glmesh.h src/gl/glcontroller.h src/gl/gltools.h src/options.h 
src/gl/glmarker.o:  src/gl/glmarker.h
src/gl/GLee.o:     src/gl/GLee.h 
src/gl/glparticles.o:  include/ns_base.h src/gl/glscene.h src/gl/glcontroller.h src/gl/glparticles.h 
src/gl/glscene.o:  include/ns_base.h src/gl/glscene.h src/gl/glcontroller.h src/gl/glnode.h src/gl/glmesh.h src/gl/glparticles.h src/gl/gltex.h src/options.h
src/gl/gltex.o:  include/ns_base.h src/gl/GLee.h src/gl/glscene.h src/gl/gltex.h src/gl/gltexloaders.h src/options.h src/fsengine/fsmanager.h src/fsengine/fsengine.h
src/gl/glcontroller.o:  include/ns_base.h src/gl/glcontroller.h src/gl/glscene.h src/options.h
src/gl/gltools.o:  include/ns_base.h src/gl/gltools.h src/nifmodel.h
src/gl/gltexloaders.o:  include/ns_base.h include/ns_utils.h src/gl/GLee.h src/gl/gltexloaders.h src/gl/dds/dds_api.h src/gl/dds/DirectDrawSurface.h src/nifmodel.h
src/gl/glnode.o:  include/ns_base.h src/gl/glscene.h src/gl/glmarker.h src/gl/glnode.h src/gl/glcontroller.h src/options.h src/NvTriStrip/qtwrapper.h src/gl/furniture.h src/gl/constraints.h
src/gl/renderer.o:  include/ns_base.h src/gl/GLee.h src/gl/renderer.h src/gl/gltex.h src/gl/glmesh.h src/gl/glscene.h src/gl/glproperty.h src/options.h
src/gl/glproperty.o:  include/ns_base.h src/gl/glproperty.h src/gl/glcontroller.h src/gl/glscene.h src/options.h
src/ns_utils.o:  include/ns_utils.h include/ns_base.h
src/nifmodel.o:  src/nifmodel.h src/niftypes.h src/options.h config.h src/spellbook.h
src/options.o:  include/ns_base.h src/options.h config.h src/widgets/colorwheel.h src/widgets/fileselect.h src/widgets/floatslider.h src/widgets/groupbox.h
src/message.o:  src/message.h
src/qhull.o:  src/qhull.h qhull/src/qhull.h qhull/src/libqhull.h  qhull/src/qhull_a.h
src/NvTriStrip/VertexCache.o:  src/NvTriStrip/VertexCache.h
src/NvTriStrip/qtwrapper.o:  src/NvTriStrip/qtwrapper.h src/NvTriStrip/NvTriStrip.h
src/NvTriStrip/NvTriStripObjects.o:     src/NvTriStrip/NvTriStripObjects.h src/NvTriStrip/VertexCache.h
src/NvTriStrip/NvTriStrip.o:  src/NvTriStrip/NvTriStripObjects.h src/NvTriStrip/NvTriStrip.h 
src/kfmmodel.o:  src/kfmmodel.h include/ns_base.h
src/nifvalue.o:  include/ns_base.h include/ns_utils.h src/nifvalue.h src/nifmodel.h config.h src/options.h
src/basemodel.o:  include/ns_base.h src/basemodel.h src/niftypes.h src/options.h
src/kfmxml.o:  include/ns_base.h src/kfmmodel.h
src/nifdelegate.o:  src/nifmodel.h src/nifproxy.h src/kfmmodel.h src/spellbook.h src/widgets/valueedit.h src/widgets/nifcheckboxlist.h src/options.h
src/nifexpr.o:  include/ns_base.h include/ns_utils.h src/nifexpr.h src/basemodel.h
src/spellbook.o:  src/spellbook.h
src/nifproxy.o:  src/nifproxy.h src/nifmodel.h
src/nifxml.o:  include/ns_base.h src/nifmodel.h src/niftypes.h
src/spells/headerstring.o:  include/ns_base.h src/spellbook.h
src/spells/color.o:  src/spellbook.h src/widgets/colorwheel.h
src/spells/mesh.o:  include/ns_base.h src/gl/glmesh.h src/spells/mesh.h
src/spells/transform.o:  include/ns_base.h src/spells/transform.h config.h src/widgets/nifeditors.h
src/spells/moppcode.o:  include/ns_base.h src/spellbook.h
src/spells/skeleton.o:  include/ns_base.h src/spellbook.h src/spells/skeleton.h src/gl/gltools.h src/NvTriStrip/qtwrapper.h
src/spells/normals.o:  include/ns_base.h src/spellbook.h src/NvTriStrip/qtwrapper.h
src/spells/fo3only.o:  src/spellbook.h
src/spells/morphctrl.o:  include/ns_base.h src/spellbook.h
src/spells/flags.o:  include/ns_base.h src/spellbook.h
src/spells/optimize.o:  include/ns_base.h src/spellbook.h src/spells/blocks.h src/gl/glmesh.h src/spells/mesh.h src/spells/tangentspace.h src/spells/transform.h
src/spells/stringpalette.o:  include/ns_base.h src/spellbook.h src/spells/stringpalette.h
src/spells/animation.o:  include/ns_base.h src/spellbook.h src/options.h
src/spells/bounds.o:  src/spellbook.h src/widgets/nifeditors.h
src/spells/strippify.o:  include/ns_base.h src/spellbook.h src/NvTriStrip/qtwrapper.h
src/spells/tangentspace.o:  include/ns_base.h src/spells/tangentspace.h src/NvTriStrip/qtwrapper.h
src/spells/material.o:  include/ns_base.h src/spellbook.h src/widgets/nifeditors.h
src/spells/misc.o:  include/ns_base.h src/spells/misc.h
src/spells/blocks.o:  include/ns_base.h src/spells/blocks.h config.h
src/spells/texture.o:  include/ns_base.h src/spells/blocks.h src/spells/texture.h src/spellbook.h src/gl/gltex.h config.h src/widgets/fileselect.h src/widgets/uvedit.h src/NvTriStrip/qtwrapper.h
src/spells/light.o:  include/ns_base.h src/spellbook.h src/widgets/nifeditors.h
src/spells/sanitize.o:  include/ns_base.h src/spellbook.h src/spells/misc.h
src/spells/havok.o:  include/ns_base.h src/spellbook.h src/qhull.h qhull/src/qhull.h qhull/src/libqhull.h src/NvTriStrip/qtwrapper.h src/spells/blocks.h
src/fsengine/fsmanager.o:  include/ns_base.h src/fsengine/fsmanager.h src/fsengine/fsengine.h src/fsengine/bsa.h src/options.h
src/fsengine/fsengine.o:  include/ns_base.h src/fsengine/fsengine.h src/fsengine/bsa.h
src/fsengine/bsa.o:  src/fsengine/bsa.h
src/importex/importex.o:  src/nifskope.h src/widgets/nifview.h src/nifproxy.h src/nifmodel.h
src/importex/obj.o:  include/ns_base.h src/nifmodel.h src/NvTriStrip/qtwrapper.h src/gl/gltex.h
src/importex/3ds.o:  include/ns_base.h src/importex/3ds.h src/spellbook.h src/NvTriStrip/qtwrapper.h src/gl/gltex.h
src/widgets/refrbrowser.o:  include/ns_base.h src/widgets/refrbrowser.h src/nifmodel.h
src/widgets/nifeditors.o:  include/ns_base.h src/widgets/nifeditors.h src/nifmodel.h src/widgets/colorwheel.h src/widgets/floatslider.h src/widgets/valueedit.h
src/widgets/groupbox.o:  src/widgets/groupbox.h
src/widgets/inspect.o:  include/ns_base.h src/widgets/inspect.h src/nifmodel.h src/gl/glscene.h src/gl/glnode.h
src/widgets/nifcheckboxlist.o:  src/widgets/nifcheckboxlist.h src/options.h
src/widgets/colorwheel.o:  src/widgets/colorwheel.h src/widgets/floatslider.h src/niftypes.h 
src/widgets/xmlcheck.o:  include/ns_base.h src/widgets/xmlcheck.h src/kfmmodel.h src/nifmodel.h config.h src/widgets/fileselect.h
src/widgets/nifview.o:  src/widgets/nifview.h src/basemodel.h src/nifproxy.h src/spellbook.h
src/widgets/uvedit.o:  include/ns_base.h src/widgets/uvedit.h src/nifmodel.h src/niftypes.h src/options.h src/gl/gltex.h src/gl/gltools.h src/NvTriStrip/qtwrapper.h  
src/widgets/floatslider.o:  src/widgets/floatslider.h src/widgets/floatedit.h
src/widgets/copyfnam.o:  src/widgets/copyfnam.h
src/widgets/valueedit.o:  src/widgets/valueedit.h src/widgets/floatedit.h
src/widgets/fileselect.o:  src/widgets/fileselect.h config.h
src/widgets/floatedit.o:  src/widgets/floatedit.h src/nifvalue.h
src/nifskope.o:  include/ns_base.h src/nifskope.h config.h src/kfmmodel.h src/nifmodel.h src/nifproxy.h src/widgets/nifview.h src/widgets/refrbrowser.h src/widgets/inspect.h src/glview.h src/spellbook.h src/widgets/fileselect.h src/widgets/copyfnam.h src/widgets/xmlcheck.h src/options.h src/fsengine/fsmanager.h
src/niftypes.o:  src/niftypes.h src/nifmodel.h
src/glview.o:  include/ns_base.h src/gl/GLee.h src/glview.h  src/nifmodel.h src/gl/glscene.h src/gl/gltex.h src/options.h src/widgets/fileselect.h src/widgets/floatedit.h src/widgets/floatslider.h
src/gl/dds/PixelFormat.h: Common.h
src/gl/dds/Image.h: Common.h Color.h
src/gl/dds/ColorBlock.h: Color.h Image.h
src/gl/dds/BlockDXT.h: Common.h Color.h ColorBlock.h Stream.h
src/gl/dds/DirectDrawSurface.h: Common.h Stream.h ColorBlock.h Image.h
src/gl/dds/dds_api.h: Image.h
src/gl/GLee.h: windows.h GL/gl.h OpenGL/gl.h GL/gl.h GL/glx.h stddef.h
src/gl/renderer.h: GLee.h nifmodel.h
src/gl/glcontroller.h: nifmodel.h
src/gl/glparticles.h: glnode.h
src/gl/glcontrolable.h: nifmodel.h
src/gl/glscene.h: GLee.h nifmodel.h glnode.h glproperty.h gltex.h gltools.h renderer.h
src/gl/glmesh.h: glnode.h gltools.h
src/gl/glnode.h: glcontrolable.h glproperty.h
src/gl/gltex.h: niftypes.h
src/gl/gltools.h: niftypes.h
src/gl/glproperty.h: GLee.h glcontrolable.h
src/gl/furniture.h: glmarker.h
src/gl/constraints.h: glmarker.h
src/qhull.h: niftypes.h
src/nifmodel.h: ns_base.h ns_utils.h basemodel.h
src/nifvalue.h: niftypes.h
src/NvTriStrip/qtwrapper.h: niftypes.h
src/NvTriStrip/NvTriStripObjects.h: assert.h VertexCache.h
src/basemodel.h: niftypes.h nifitem.h message.h
src/spellbook.h: nifmodel.h
src/glview.h: gl/GLee.h nifmodel.h widgets/floatedit.h widgets/floatslider.h
src/options.h: widgets/groupbox.h
src/spells/tangentspace.h: spellbook.h
src/spells/blocks.h: spellbook.h
src/spells/mesh.h: spellbook.h
src/spells/misc.h: spellbook.h
src/spells/texture.h: widgets/nifeditors.h
src/spells/transform.h: spellbook.h
src/fsengine/bsa.h: fsengine.h
src/kfmmodel.h: ns_base.h ns_utils.h basemodel.h
src/widgets/nifcheckboxlist.h: nifvalue.h nifmodel.h
src/widgets/valueedit.h: nifvalue.h
src/widgets/nifeditors.h: nifmodel.h
src/widgets/xmlcheck.h: message.h
src/nifskope.h: message.h
PixelFormat.h: src/gl/dds/PixelFormat.h
Common.h: src/gl/dds/Common.h
Image.h: src/gl/dds/Image.h
ColorBlock.h: src/gl/dds/ColorBlock.h
Color.h: src/gl/dds/Color.h
BlockDXT.h: src/gl/dds/BlockDXT.h
Stream.h: src/gl/dds/Stream.h
DirectDrawSurface.h: src/gl/dds/DirectDrawSurface.h
dds_api.h: src/gl/dds/dds_api.h
GLee.h: src/gl/GLee.h
gltexloaders.h: src/gl/gltexloaders.h
renderer.h: src/gl/renderer.h
glcontroller.h: src/gl/glcontroller.h
glparticles.h: src/gl/glparticles.h
glcontrolable.h: src/gl/glcontrolable.h
glscene.h: src/gl/glscene.h
glmesh.h: src/gl/glmesh.h
glnode.h: src/gl/glnode.h
glmarker.h: src/gl/glmarker.h
gltex.h: src/gl/gltex.h
gltools.h: src/gl/gltools.h
glproperty.h: src/gl/glproperty.h
furniture.h: src/gl/furniture.h
constraints.h: src/gl/constraints.h
qhull.h: src/qhull.h
nifmodel.h: src/nifmodel.h
nifvalue.h: src/nifvalue.h
VertexCache.h: src/NvTriStrip/VertexCache.h
NvTriStrip.h: src/NvTriStrip/NvTriStrip.h
qtwrapper.h: src/NvTriStrip/qtwrapper.h
NvTriStripObjects.h: src/NvTriStrip/NvTriStripObjects.h
niftypes.h: src/niftypes.h
basemodel.h: src/basemodel.h
spellbook.h: src/spellbook.h
glview.h: src/glview.h
nifproxy.h: src/nifproxy.h
options.h: src/options.h
tangentspace.h: src/spells/tangentspace.h
blocks.h: src/spells/blocks.h
mesh.h: src/spells/mesh.h
misc.h: src/spells/misc.h
texture.h: src/spells/texture.h
skeleton.h: src/spells/skeleton.h
transform.h: src/spells/transform.h
stringpalette.h: src/spells/stringpalette.h
nifexpr.h: src/nifexpr.h
fsmanager.h: src/fsengine/fsmanager.h
bsa.h: src/fsengine/bsa.h
fsengine.h: src/fsengine/fsengine.h
kfmmodel.h: src/kfmmodel.h
3ds.h: src/importex/3ds.h
message.h: src/message.h
refrbrowser.h: src/widgets/refrbrowser.h
groupbox.h: src/widgets/groupbox.h
nifcheckboxlist.h: src/widgets/nifcheckboxlist.h
uvedit.h: src/widgets/uvedit.h
floatedit.h: src/widgets/floatedit.h
valueedit.h: src/widgets/valueedit.h
copyfnam.h: src/widgets/copyfnam.h
floatslider.h: src/widgets/floatslider.h
colorwheel.h: src/widgets/colorwheel.h
fileselect.h: src/widgets/fileselect.h
nifeditors.h: src/widgets/nifeditors.h
inspect.h: src/widgets/inspect.h
nifview.h: src/widgets/nifview.h
xmlcheck.h: src/widgets/xmlcheck.h
nifskope.h: src/nifskope.h
include/nifitem.h: nifvalue.h nifexpr.h
hacking.h: include/hacking.h
ns_base.h: include/ns_base.h
nifitem.h: include/nifitem.h
ns_utils.h: include/ns_utils.h
%.h:
	
