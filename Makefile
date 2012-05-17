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
NL2INCS  = -Isrc/niflib2
INCS     = $(HEADERS) $(QTINCS) $(NL2INCS)
NSOPTNS  = -DFSENGINE -DNIFSKOPE_QT -DNIFSKOPE_X -DNIFSKOPE_OGRE_GL
QT4OPTNS = -DQT_XML_LIB -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
OPTNS    = $(NSOPTNS) $(QT4OPTNS)
CXXFLAGS = -g -frtti -Wfatal-errors -Wall $(OPTNS) $(INCS)
CPPFLAGS =
CFLAGS   =
LDFLAGS  = $(LIBS)
APP      = nifskope

GLOBAL_SRC = $(shell find $(WORK_DIR)/ -type f -name '*.cpp' | grep -v -e '$(QHULLDIR)/' -e '\.moc.cpp$$' -e '\.qrc.cpp$$' -e 'test/' -e 'niflib++.cpp')
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
	find . -type f -name "*~" -delete

# DO NOT DELETE

src/gl/dds/Image.o: src/gl/dds/Color.h src/gl/dds/Image.h src/gl/dds/Common.h
src/gl/dds/DirectDrawSurface.o: src/gl/dds/DirectDrawSurface.h
src/gl/dds/DirectDrawSurface.o: src/gl/dds/Common.h src/gl/dds/Stream.h
src/gl/dds/DirectDrawSurface.o: src/gl/dds/ColorBlock.h src/gl/dds/Color.h
src/gl/dds/DirectDrawSurface.o: src/gl/dds/Image.h src/gl/dds/BlockDXT.h
src/gl/dds/DirectDrawSurface.o: src/gl/dds/PixelFormat.h
src/gl/dds/dds_api.o: src/gl/dds/dds_api.h src/gl/dds/Image.h
src/gl/dds/dds_api.o: src/gl/dds/Common.h src/gl/dds/Color.h
src/gl/dds/dds_api.o: src/gl/dds/Stream.h src/gl/dds/DirectDrawSurface.h
src/gl/dds/dds_api.o: src/gl/dds/ColorBlock.h
src/gl/dds/ColorBlock.o: src/gl/dds/ColorBlock.h src/gl/dds/Color.h
src/gl/dds/ColorBlock.o: src/gl/dds/Image.h src/gl/dds/Common.h
src/gl/dds/Stream.o: src/gl/dds/Stream.h
src/gl/dds/BlockDXT.o: src/gl/dds/Common.h src/gl/dds/Stream.h
src/gl/dds/BlockDXT.o: src/gl/dds/ColorBlock.h src/gl/dds/Color.h
src/gl/dds/BlockDXT.o: src/gl/dds/Image.h src/gl/dds/BlockDXT.h
src/gl/glmesh.o: include/ns_base.h src/gl/glscene.h src/gl/GLee.h
src/gl/glmesh.o: src/nifmodel.h include/ns_types.h include/ns_utils.h
src/gl/glmesh.o: src/basemodel.h src/message.h src/gl/glnode.h
src/gl/glmesh.o: src/gl/glcontrolable.h src/gl/glproperty.h include/ns_math.h
src/gl/glmesh.o: include/ns_opengl.h src/gl/gltex.h src/gl/gltools.h
src/gl/glmesh.o: src/gl/renderer.h src/gl/glmesh.h src/gl/glcontroller.h
src/gl/glmesh.o: src/options.h src/widgets/groupbox.h
src/gl/glmarker.o: src/gl/glmarker.h
src/gl/GLee.o: src/gl/GLee.h
src/gl/glparticles.o: include/ns_base.h src/gl/glscene.h src/gl/GLee.h
src/gl/glparticles.o: src/nifmodel.h include/ns_types.h include/ns_utils.h
src/gl/glparticles.o: src/basemodel.h src/message.h src/gl/glnode.h
src/gl/glparticles.o: src/gl/glcontrolable.h src/gl/glproperty.h
src/gl/glparticles.o: include/ns_math.h include/ns_opengl.h src/gl/gltex.h
src/gl/glparticles.o: src/gl/gltools.h src/gl/renderer.h
src/gl/glparticles.o: src/gl/glcontroller.h src/gl/glparticles.h
src/gl/glscene.o: include/ns_base.h src/gl/glscene.h src/gl/GLee.h
src/gl/glscene.o: src/nifmodel.h include/ns_types.h include/ns_utils.h
src/gl/glscene.o: src/basemodel.h src/message.h src/gl/glnode.h
src/gl/glscene.o: src/gl/glcontrolable.h src/gl/glproperty.h
src/gl/glscene.o: include/ns_math.h include/ns_opengl.h src/gl/gltex.h
src/gl/glscene.o: src/gl/gltools.h src/gl/renderer.h src/gl/glcontroller.h
src/gl/glscene.o: src/gl/glmesh.h src/gl/glparticles.h src/options.h
src/gl/glscene.o: src/widgets/groupbox.h
src/gl/gltex.o: include/ns_base.h src/gl/GLee.h src/gl/glscene.h
src/gl/gltex.o: src/nifmodel.h include/ns_types.h include/ns_utils.h
src/gl/gltex.o: src/basemodel.h src/message.h src/gl/glnode.h
src/gl/gltex.o: src/gl/glcontrolable.h src/gl/glproperty.h include/ns_math.h
src/gl/gltex.o: include/ns_opengl.h src/gl/gltex.h src/gl/gltools.h
src/gl/gltex.o: src/gl/renderer.h src/gl/gltexloaders.h src/options.h
src/gl/gltex.o: src/widgets/groupbox.h src/fsengine/fsmanager.h
src/gl/gltex.o: src/fsengine/fsengine.h
src/gl/glcontroller.o: include/ns_base.h src/gl/glcontroller.h src/nifmodel.h
src/gl/glcontroller.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/gl/glcontroller.o: src/message.h src/gl/glscene.h src/gl/GLee.h
src/gl/glcontroller.o: src/gl/glnode.h src/gl/glcontrolable.h
src/gl/glcontroller.o: src/gl/glproperty.h include/ns_math.h
src/gl/glcontroller.o: include/ns_opengl.h src/gl/gltex.h src/gl/gltools.h
src/gl/glcontroller.o: src/gl/renderer.h src/options.h src/widgets/groupbox.h
src/gl/gltools.o: include/ns_base.h src/gl/gltools.h src/nifmodel.h
src/gl/gltools.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/gl/gltools.o: src/message.h
src/gl/gltexloaders.o: include/ns_base.h include/ns_utils.h src/gl/GLee.h
src/gl/gltexloaders.o: src/gl/gltexloaders.h src/gl/dds/dds_api.h
src/gl/gltexloaders.o: src/gl/dds/Image.h src/gl/dds/Common.h
src/gl/gltexloaders.o: src/gl/dds/Color.h src/gl/dds/DirectDrawSurface.h
src/gl/gltexloaders.o: src/gl/dds/Stream.h src/gl/dds/ColorBlock.h
src/gl/gltexloaders.o: src/nifmodel.h include/ns_types.h src/basemodel.h
src/gl/gltexloaders.o: src/message.h
src/gl/glnode.o: include/ns_base.h src/gl/glscene.h src/gl/GLee.h
src/gl/glnode.o: src/nifmodel.h include/ns_types.h include/ns_utils.h
src/gl/glnode.o: src/basemodel.h src/message.h src/gl/glnode.h
src/gl/glnode.o: src/gl/glcontrolable.h src/gl/glproperty.h include/ns_math.h
src/gl/glnode.o: include/ns_opengl.h src/gl/gltex.h src/gl/gltools.h
src/gl/glnode.o: src/gl/renderer.h src/gl/glmarker.h src/gl/glcontroller.h
src/gl/glnode.o: src/options.h src/widgets/groupbox.h
src/gl/glnode.o: src/NvTriStrip/qtwrapper.h src/gl/furniture.h
src/gl/glnode.o: src/gl/constraints.h
src/gl/renderer.o: include/ns_base.h src/gl/GLee.h src/gl/renderer.h
src/gl/renderer.o: src/nifmodel.h include/ns_types.h include/ns_utils.h
src/gl/renderer.o: src/basemodel.h src/message.h src/gl/gltex.h
src/gl/renderer.o: src/gl/glmesh.h src/gl/glnode.h src/gl/glcontrolable.h
src/gl/renderer.o: src/gl/glproperty.h include/ns_math.h include/ns_opengl.h
src/gl/renderer.o: src/gl/gltools.h src/gl/glscene.h src/options.h
src/gl/renderer.o: src/widgets/groupbox.h
src/gl/glproperty.o: include/ns_base.h src/gl/glproperty.h src/gl/GLee.h
src/gl/glproperty.o: src/gl/glcontrolable.h src/nifmodel.h include/ns_types.h
src/gl/glproperty.o: include/ns_utils.h src/basemodel.h src/message.h
src/gl/glproperty.o: include/ns_math.h include/ns_opengl.h
src/gl/glproperty.o: src/gl/glcontroller.h src/gl/glscene.h src/gl/glnode.h
src/gl/glproperty.o: src/gl/gltex.h src/gl/gltools.h src/gl/renderer.h
src/gl/glproperty.o: src/options.h src/widgets/groupbox.h
src/ns_utils.o: include/ns_utils.h include/ns_base.h
src/nifmodel.o: src/nifmodel.h include/ns_types.h include/ns_base.h
src/nifmodel.o: include/ns_utils.h src/basemodel.h src/message.h
src/nifmodel.o: src/options.h src/widgets/groupbox.h config.h src/spellbook.h
src/options.o: include/ns_base.h src/options.h src/widgets/groupbox.h
src/options.o: config.h src/widgets/colorwheel.h src/widgets/fileselect.h
src/options.o: src/widgets/floatslider.h
src/ns_opengl.o: include/ns_opengl.h include/ns_math.h include/ns_types.h
src/message.o: src/message.h
src/qhull.o: src/qhull.h qhull/src/qhull_a.h qhull/src/libqhull.h
src/qhull.o: qhull/src/user.h qhull/src/stat.h qhull/src/random.h
src/qhull.o: qhull/src/mem.h qhull/src/qset.h qhull/src/geom.h
src/qhull.o: qhull/src/merge.h qhull/src/poly.h qhull/src/io.h
src/qhull.o: qhull/src/libqhull.c qhull/src/qhull_a.h qhull/src/mem.c
src/qhull.o: qhull/src/qset.c qhull/src/geom.c qhull/src/merge.c
src/qhull.o: qhull/src/poly.c qhull/src/io.c qhull/src/stat.c
src/qhull.o: qhull/src/global.c qhull/src/user.c qhull/src/poly2.c
src/qhull.o: qhull/src/geom2.c qhull/src/userprintf.c qhull/src/usermem.c
src/qhull.o: qhull/src/random.c qhull/src/rboxlib.c
src/NvTriStrip/VertexCache.o: src/NvTriStrip/VertexCache.h
src/NvTriStrip/qtwrapper.o: src/NvTriStrip/qtwrapper.h
src/NvTriStrip/qtwrapper.o: src/NvTriStrip/NvTriStrip.h
src/NvTriStrip/NvTriStripObjects.o: src/NvTriStrip/NvTriStripObjects.h
src/NvTriStrip/NvTriStripObjects.o: src/NvTriStrip/VertexCache.h
src/NvTriStrip/NvTriStrip.o: src/NvTriStrip/NvTriStripObjects.h
src/NvTriStrip/NvTriStrip.o: src/NvTriStrip/VertexCache.h
src/NvTriStrip/NvTriStrip.o: src/NvTriStrip/NvTriStrip.h
src/kfmmodel.o: src/kfmmodel.h include/ns_types.h include/ns_base.h
src/kfmmodel.o: include/ns_utils.h src/basemodel.h src/message.h
src/basemodel.o: include/ns_base.h src/basemodel.h src/message.h
src/basemodel.o: src/options.h src/widgets/groupbox.h
src/ns_math.o: include/ns_math.h include/ns_types.h
src/nifdelegate.o: src/nifmodel.h include/ns_types.h include/ns_base.h
src/nifdelegate.o: include/ns_utils.h src/basemodel.h src/message.h
src/nifdelegate.o: src/nifproxy.h src/kfmmodel.h src/spellbook.h
src/nifdelegate.o: src/widgets/valueedit.h src/widgets/nifcheckboxlist.h
src/nifdelegate.o: src/options.h src/widgets/groupbox.h
src/niflib2/Parser.o: src/niflib2/Parser.h src/niflib2/List.h
src/niflib2/Parser.o: src/niflib2/Tag.h src/niflib2/Attr.h
src/niflib2/Parser.o: src/niflib2/Buffer.h src/niflib2/niflib.h
src/niflib2/TreeNode.o: src/niflib2/TreeNode.h src/niflib2/List.h
src/niflib2/niff.o: src/niflib2/niff.h
src/niflib2/Tag.o: src/niflib2/Tag.h src/niflib2/Attr.h src/niflib2/Buffer.h
src/niflib2/Tag.o: src/niflib2/List.h src/niflib2/niflib.h
src/niflib2/Field.o: src/niflib2/Field.h src/niflib2/Buffer.h
src/niflib2/Field.o: src/niflib2/Tag.h src/niflib2/Attr.h src/niflib2/List.h
src/niflib2/Field.o: src/niflib2/niff.h src/niflib2/niflib.h
src/niflib2/Field.o: src/niflib2/Compiler.h src/niflib2/Parser.h
src/niflib2/Field.o: src/niflib2/TreeNode.h
src/niflib2/Buffer.o: src/niflib2/Buffer.h src/niflib2/niflib.h
src/niflib2/List.o: src/niflib2/List.h
src/niflib2/niflib++.o: src/niflib2/niflib.h src/niflib2/niff.h
src/niflib2/niflib++.o: src/niflib2/Parser.h src/niflib2/List.h
src/niflib2/niflib++.o: src/niflib2/Tag.h src/niflib2/Attr.h
src/niflib2/niflib++.o: src/niflib2/Buffer.h src/niflib2/Compiler.h
src/niflib2/niflib++.o: src/niflib2/Field.h src/niflib2/TreeNode.h
src/niflib2/Attr.o: src/niflib2/Attr.h src/niflib2/Buffer.h
src/niflib2/Compiler.o: src/niflib2/Compiler.h src/niflib2/niflib.h
src/niflib2/Compiler.o: src/niflib2/Parser.h src/niflib2/List.h
src/niflib2/Compiler.o: src/niflib2/Tag.h src/niflib2/Attr.h
src/niflib2/Compiler.o: src/niflib2/Buffer.h src/niflib2/niff.h
src/niflib2/Compiler.o: src/niflib2/Field.h src/niflib2/TreeNode.h
src/niflib2/niflib.o: src/niflib2/niflib.h
src/spellbook.o: src/spellbook.h src/nifmodel.h include/ns_types.h
src/spellbook.o: include/ns_base.h include/ns_utils.h src/basemodel.h
src/spellbook.o: src/message.h
src/nifproxy.o: src/nifproxy.h src/nifmodel.h include/ns_types.h
src/nifproxy.o: include/ns_base.h include/ns_utils.h src/basemodel.h
src/nifproxy.o: src/message.h
src/spells/headerstring.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/headerstring.o: include/ns_types.h include/ns_utils.h
src/spells/headerstring.o: src/basemodel.h src/message.h
src/spells/color.o: src/spellbook.h src/nifmodel.h include/ns_types.h
src/spells/color.o: include/ns_base.h include/ns_utils.h src/basemodel.h
src/spells/color.o: src/message.h src/widgets/colorwheel.h
src/spells/mesh.o: include/ns_base.h src/spells/mesh.h src/spellbook.h
src/spells/mesh.o: src/nifmodel.h include/ns_types.h include/ns_utils.h
src/spells/mesh.o: src/basemodel.h src/message.h
src/spells/transform.o: include/ns_base.h src/spells/transform.h
src/spells/transform.o: src/spellbook.h src/nifmodel.h include/ns_types.h
src/spells/transform.o: include/ns_utils.h src/basemodel.h src/message.h
src/spells/transform.o: config.h src/widgets/nifeditors.h
src/spells/moppcode.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/moppcode.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/moppcode.o: src/message.h
src/spells/skeleton.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/skeleton.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/skeleton.o: src/message.h src/spells/skeleton.h src/gl/gltools.h
src/spells/skeleton.o: src/NvTriStrip/qtwrapper.h
src/spells/normals.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/normals.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/normals.o: src/message.h src/NvTriStrip/qtwrapper.h
src/spells/fo3only.o: src/spellbook.h src/nifmodel.h include/ns_types.h
src/spells/fo3only.o: include/ns_base.h include/ns_utils.h src/basemodel.h
src/spells/fo3only.o: src/message.h
src/spells/morphctrl.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/morphctrl.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/morphctrl.o: src/message.h
src/spells/flags.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/flags.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/flags.o: src/message.h
src/spells/optimize.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/optimize.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/optimize.o: src/message.h src/spells/blocks.h src/spells/mesh.h
src/spells/optimize.o: src/spells/tangentspace.h src/spells/transform.h
src/spells/stringpalette.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/stringpalette.o: include/ns_types.h include/ns_utils.h
src/spells/stringpalette.o: src/basemodel.h src/message.h
src/spells/stringpalette.o: src/spells/stringpalette.h
src/spells/animation.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/animation.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/animation.o: src/message.h src/options.h src/widgets/groupbox.h
src/spells/bounds.o: src/spellbook.h src/nifmodel.h include/ns_types.h
src/spells/bounds.o: include/ns_base.h include/ns_utils.h src/basemodel.h
src/spells/bounds.o: src/message.h src/widgets/nifeditors.h
src/spells/strippify.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/strippify.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/strippify.o: src/message.h src/NvTriStrip/qtwrapper.h
src/spells/tangentspace.o: include/ns_base.h src/spells/tangentspace.h
src/spells/tangentspace.o: src/spellbook.h src/nifmodel.h include/ns_types.h
src/spells/tangentspace.o: include/ns_utils.h src/basemodel.h src/message.h
src/spells/tangentspace.o: src/NvTriStrip/qtwrapper.h
src/spells/material.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/material.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/material.o: src/message.h src/widgets/nifeditors.h
src/spells/misc.o: include/ns_base.h src/spells/misc.h src/spellbook.h
src/spells/misc.o: src/nifmodel.h include/ns_types.h include/ns_utils.h
src/spells/misc.o: src/basemodel.h src/message.h
src/spells/blocks.o: include/ns_base.h src/spells/blocks.h src/spellbook.h
src/spells/blocks.o: src/nifmodel.h include/ns_types.h include/ns_utils.h
src/spells/blocks.o: src/basemodel.h src/message.h config.h
src/spells/texture.o: include/ns_base.h src/spells/blocks.h src/spellbook.h
src/spells/texture.o: src/nifmodel.h include/ns_types.h include/ns_utils.h
src/spells/texture.o: src/basemodel.h src/message.h src/spells/texture.h
src/spells/texture.o: src/widgets/nifeditors.h src/gl/gltex.h config.h
src/spells/texture.o: src/widgets/fileselect.h src/widgets/uvedit.h
src/spells/texture.o: src/NvTriStrip/qtwrapper.h
src/spells/light.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/light.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/light.o: src/message.h src/widgets/nifeditors.h
src/spells/sanitize.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/sanitize.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/sanitize.o: src/message.h src/spells/misc.h
src/spells/havok.o: include/ns_base.h src/spellbook.h src/nifmodel.h
src/spells/havok.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/spells/havok.o: src/message.h src/qhull.h src/NvTriStrip/qtwrapper.h
src/spells/havok.o: src/spells/blocks.h
src/fsengine/fsmanager.o: include/ns_base.h src/fsengine/fsmanager.h
src/fsengine/fsmanager.o: src/fsengine/fsengine.h src/fsengine/bsa.h
src/fsengine/fsmanager.o: src/options.h src/widgets/groupbox.h
src/fsengine/fsengine.o: include/ns_base.h src/fsengine/fsengine.h
src/fsengine/fsengine.o: src/fsengine/bsa.h
src/fsengine/bsa.o: src/fsengine/bsa.h src/fsengine/fsengine.h
src/importex/importex.o: src/nifskope.h src/message.h src/widgets/nifview.h
src/importex/importex.o: src/nifproxy.h src/nifmodel.h include/ns_types.h
src/importex/importex.o: include/ns_base.h include/ns_utils.h src/basemodel.h
src/importex/obj.o: include/ns_base.h src/nifmodel.h include/ns_types.h
src/importex/obj.o: include/ns_utils.h src/basemodel.h src/message.h
src/importex/obj.o: src/NvTriStrip/qtwrapper.h src/gl/gltex.h
src/importex/3ds.o: include/ns_base.h src/importex/3ds.h src/spellbook.h
src/importex/3ds.o: src/nifmodel.h include/ns_types.h include/ns_utils.h
src/importex/3ds.o: src/basemodel.h src/message.h src/NvTriStrip/qtwrapper.h
src/importex/3ds.o: src/gl/gltex.h
src/widgets/refrbrowser.o: include/ns_base.h src/widgets/refrbrowser.h
src/widgets/refrbrowser.o: src/nifmodel.h include/ns_types.h
src/widgets/refrbrowser.o: include/ns_utils.h src/basemodel.h src/message.h
src/widgets/nifeditors.o: include/ns_base.h src/widgets/nifeditors.h
src/widgets/nifeditors.o: src/nifmodel.h include/ns_types.h
src/widgets/nifeditors.o: include/ns_utils.h src/basemodel.h src/message.h
src/widgets/nifeditors.o: src/widgets/colorwheel.h src/widgets/floatslider.h
src/widgets/nifeditors.o: src/widgets/valueedit.h
src/widgets/groupbox.o: src/widgets/groupbox.h
src/widgets/inspect.o: include/ns_base.h src/widgets/inspect.h src/nifmodel.h
src/widgets/inspect.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/widgets/inspect.o: src/message.h src/gl/glscene.h src/gl/GLee.h
src/widgets/inspect.o: src/gl/glnode.h src/gl/glcontrolable.h
src/widgets/inspect.o: src/gl/glproperty.h include/ns_math.h
src/widgets/inspect.o: include/ns_opengl.h src/gl/gltex.h src/gl/gltools.h
src/widgets/inspect.o: src/gl/renderer.h src/gl/glnode.h
src/widgets/nifcheckboxlist.o: src/widgets/nifcheckboxlist.h src/nifmodel.h
src/widgets/nifcheckboxlist.o: include/ns_types.h include/ns_base.h
src/widgets/nifcheckboxlist.o: include/ns_utils.h src/basemodel.h
src/widgets/nifcheckboxlist.o: src/message.h src/options.h
src/widgets/nifcheckboxlist.o: src/widgets/groupbox.h
src/widgets/colorwheel.o: src/widgets/colorwheel.h src/widgets/floatslider.h
src/widgets/xmlcheck.o: include/ns_base.h src/widgets/xmlcheck.h
src/widgets/xmlcheck.o: src/message.h src/kfmmodel.h include/ns_types.h
src/widgets/xmlcheck.o: include/ns_utils.h src/basemodel.h src/nifmodel.h
src/widgets/xmlcheck.o: config.h src/widgets/fileselect.h
src/widgets/nifview.o: src/widgets/nifview.h src/basemodel.h src/message.h
src/widgets/nifview.o: src/nifproxy.h src/spellbook.h src/nifmodel.h
src/widgets/nifview.o: include/ns_types.h include/ns_base.h
src/widgets/nifview.o: include/ns_utils.h
src/widgets/uvedit.o: include/ns_base.h src/widgets/uvedit.h src/nifmodel.h
src/widgets/uvedit.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/widgets/uvedit.o: src/message.h src/options.h src/widgets/groupbox.h
src/widgets/uvedit.o: src/gl/gltex.h src/gl/gltools.h
src/widgets/uvedit.o: src/NvTriStrip/qtwrapper.h
src/widgets/floatslider.o: src/widgets/floatslider.h src/widgets/floatedit.h
src/widgets/copyfnam.o: src/widgets/copyfnam.h
src/widgets/valueedit.o: src/widgets/valueedit.h src/widgets/floatedit.h
src/widgets/fileselect.o: src/widgets/fileselect.h config.h
src/widgets/floatedit.o: src/widgets/floatedit.h
src/nifskope.o: include/ns_base.h src/nifskope.h src/message.h config.h
src/nifskope.o: src/kfmmodel.h include/ns_types.h include/ns_utils.h
src/nifskope.o: src/basemodel.h src/nifmodel.h src/nifproxy.h
src/nifskope.o: src/widgets/nifview.h src/widgets/refrbrowser.h
src/nifskope.o: src/widgets/inspect.h src/glview.h src/gl/GLee.h
src/nifskope.o: src/widgets/floatedit.h src/widgets/floatslider.h
src/nifskope.o: src/spellbook.h src/widgets/fileselect.h
src/nifskope.o: src/widgets/copyfnam.h src/widgets/xmlcheck.h src/options.h
src/nifskope.o: src/widgets/groupbox.h
src/glview.o: include/ns_base.h src/gl/GLee.h src/glview.h src/nifmodel.h
src/glview.o: include/ns_types.h include/ns_utils.h src/basemodel.h
src/glview.o: src/message.h src/widgets/floatedit.h src/widgets/floatslider.h
src/glview.o: src/gl/glscene.h src/gl/GLee.h src/gl/glnode.h
src/glview.o: src/gl/glcontrolable.h src/gl/glproperty.h include/ns_math.h
src/glview.o: include/ns_opengl.h src/gl/gltex.h src/gl/gltools.h
src/glview.o: src/gl/renderer.h src/gl/gltex.h src/options.h
src/glview.o: src/widgets/groupbox.h src/widgets/fileselect.h
