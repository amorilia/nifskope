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

GLOBAL_SRC = $(shell find $(WORK_DIR)/ -type f -name '*.cpp' | grep -v -e '$(QHULLDIR)/' -e '\.moc.cpp$$' -e '\.qrc.cpp$$')
GLOBAL_OBJ = $(patsubst %.cpp,%.o,$(GLOBAL_SRC))

MOC_MODULES = $(patsubst %.h,%.moc.cpp,$(shell grep -l -R Q_OBJECT * | grep -v 'qhull/' | grep \.h$$))
MOC_OBJS = $(patsubst %.cpp,%.o,$(MOC_MODULES))

RCC_QRC = $(patsubst %.qrc,%.qrc.cpp,$(wildcard $(RESDIR)/*.qrc))
RCC_OBJ = $(patsubst %.cpp,%.o,$(RCC_QRC))

.PHONY: all test $(APP) clean

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

clean:
	rm -f $(APP) $(GLOBAL_OBJ) $(MOC_OBJS) $(RCC_OBJ)

# Auto-generated header dependency list
# includes config.h generation when needed
src/gl/dds/Image.o: Color.h Image.h stdio.h
src/gl/dds/DirectDrawSurface.o: DirectDrawSurface.h BlockDXT.h PixelFormat.h stdio.h math.h
src/gl/dds/dds_api.o: dds_api.h Stream.h DirectDrawSurface.h stdio.h
src/gl/dds/ColorBlock.o: ColorBlock.h Image.h Common.h
src/gl/dds/Stream.o: Stream.h stdio.h string.h
src/gl/dds/BlockDXT.o: Common.h Stream.h ColorBlock.h BlockDXT.h
src/gl/glmesh.o: ns_base.h glscene.h glmesh.h glcontroller.h gltools.h options.h GL/glext.h
src/gl/glmarker.o: glmarker.h
src/gl/GLee.o: stdio.h stdlib.h string.h GLee.h Carbon/Carbon.h
src/gl/glparticles.o: glscene.h glcontroller.h glparticles.h math.h
src/gl/glscene.o: glscene.h glcontroller.h glnode.h glmesh.h glparticles.h gltex.h options.h
src/gl/gltex.o: GLee.h glscene.h gltex.h gltexloaders.h options.h fsengine/fsmanager.h fsengine/fsengine.h
src/gl/glcontroller.o: glcontroller.h glscene.h options.h
src/gl/gltools.o: gltools.h nifmodel.h
src/gl/gltexloaders.o: GLee.h gltexloaders.h dds/dds_api.h dds/DirectDrawSurface.h nifmodel.h
src/gl/glnode.o: glscene.h glmarker.h glnode.h glcontroller.h options.h NvTriStrip/qtwrapper.h furniture.h constraints.h
src/gl/renderer.o: GLee.h renderer.h gltex.h glmesh.h glscene.h glproperty.h options.h
src/gl/glproperty.o: ns_base.h glproperty.h glcontroller.h glscene.h options.h
src/nifmodel.o: ns_base.h nifmodel.h niftypes.h options.h config.h spellbook.h
src/options.o: options.h config.h widgets/colorwheel.h widgets/fileselect.h widgets/floatslider.h widgets/groupbox.h
src/message.o: message.h
src/qhull.o: qhull.h math.h qhull/src/qhull_a.h
src/NvTriStrip/VertexCache.o: VertexCache.h
src/NvTriStrip/qtwrapper.o: qtwrapper.h NvTriStrip.h
src/NvTriStrip/NvTriStripObjects.o: assert.h string.h stdio.h NvTriStripObjects.h VertexCache.h
src/NvTriStrip/NvTriStrip.o: NvTriStripObjects.h NvTriStrip.h string.h
src/kfmmodel.o: kfmmodel.h
src/nifvalue.o: nifvalue.h nifmodel.h config.h options.h
src/basemodel.o: basemodel.h niftypes.h options.h
src/kfmxml.o: kfmmodel.h
src/nifdelegate.o: nifmodel.h nifproxy.h kfmmodel.h spellbook.h widgets/valueedit.h widgets/nifcheckboxlist.h options.h
src/nifexpr.o: nifexpr.h basemodel.h
src/spellbook.o: spellbook.h
src/nifproxy.o: nifproxy.h nifmodel.h
src/nifxml.o: nifmodel.h niftypes.h
src/spells/headerstring.o: spellbook.h
src/spells/color.o: spellbook.h widgets/colorwheel.h
src/spells/mesh.o: mesh.h
src/spells/transform.o: transform.h config.h widgets/nifeditors.h
src/spells/moppcode.o: ns_base.h spellbook.h
src/spells/skeleton.o: spellbook.h skeleton.h gl/gltools.h NvTriStrip/qtwrapper.h
src/spells/normals.o: spellbook.h NvTriStrip/qtwrapper.h
src/spells/fo3only.o: spellbook.h
src/spells/morphctrl.o: spellbook.h
src/spells/flags.o: ns_base.h spellbook.h
src/spells/optimize.o: spellbook.h blocks.h mesh.h tangentspace.h transform.h
src/spells/stringpalette.o: ns_base.h spellbook.h stringpalette.h
src/spells/animation.o: spellbook.h options.h
src/spells/bounds.o: spellbook.h widgets/nifeditors.h
src/spells/strippify.o: spellbook.h NvTriStrip/qtwrapper.h
src/spells/tangentspace.o: ns_base.h tangentspace.h NvTriStrip/qtwrapper.h
src/spells/material.o: spellbook.h widgets/nifeditors.h
src/spells/misc.o: misc.h
src/spells/blocks.o: blocks.h config.h
src/spells/texture.o: ns_base.h blocks.h texture.h spellbook.h gl/gltex.h config.h widgets/fileselect.h widgets/uvedit.h NvTriStrip/qtwrapper.h
src/spells/light.o: spellbook.h widgets/nifeditors.h
src/spells/sanitize.o: ns_base.h spellbook.h misc.h
src/spells/havok.o: spellbook.h qhull.h NvTriStrip/qtwrapper.h blocks.h
src/fsengine/fsmanager.o: fsmanager.h fsengine.h bsa.h options.h
src/fsengine/fsengine.o: fsengine.h bsa.h
src/fsengine/bsa.o: bsa.h
src/importex/importex.o: nifskope.h widgets/nifview.h nifproxy.h nifmodel.h
src/importex/obj.o: ns_base.h nifmodel.h NvTriStrip/qtwrapper.h gl/gltex.h
src/importex/3ds.o: 3ds.h spellbook.h NvTriStrip/qtwrapper.h gl/gltex.h
src/widgets/refrbrowser.o: refrbrowser.h nifmodel.h
src/widgets/nifeditors.o: nifeditors.h nifmodel.h colorwheel.h floatslider.h valueedit.h
src/widgets/groupbox.o: groupbox.h
src/widgets/inspect.o: inspect.h nifmodel.h gl/glscene.h gl/glnode.h
src/widgets/nifcheckboxlist.o: nifcheckboxlist.h options.h
src/widgets/colorwheel.o: colorwheel.h floatslider.h niftypes.h math.h
src/widgets/xmlcheck.o: xmlcheck.h kfmmodel.h nifmodel.h config.h fileselect.h
src/widgets/nifview.o: nifview.h basemodel.h nifproxy.h spellbook.h
src/widgets/uvedit.o: uvedit.h nifmodel.h niftypes.h options.h gl/gltex.h gl/gltools.h NvTriStrip/qtwrapper.h math.h GL/glext.h
src/widgets/floatslider.o: floatslider.h floatedit.h
src/widgets/copyfnam.o: copyfnam.h
src/widgets/valueedit.o: valueedit.h floatedit.h
src/widgets/fileselect.o: fileselect.h config.h
src/widgets/floatedit.o: floatedit.h nifvalue.h
src/nifskope.o: nifskope.h config.h kfmmodel.h nifmodel.h nifproxy.h widgets/nifview.h widgets/refrbrowser.h widgets/inspect.h glview.h spellbook.h widgets/fileselect.h widgets/copyfnam.h widgets/xmlcheck.h options.h fsengine/fsmanager.h
src/niftypes.o: niftypes.h nifmodel.h
src/glview.o: gl/GLee.h glview.h math.h nifmodel.h gl/glscene.h gl/gltex.h options.h widgets/fileselect.h widgets/floatedit.h widgets/floatslider.h
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
src/nifmodel.h: basemodel.h
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
src/kfmmodel.h: basemodel.h
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
%.h:
	
