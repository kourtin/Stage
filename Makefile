# Makefile
# Stage surpuissant 2000
# J. Aceituno

# lib: opencv
opencv_cppflags ?= `pkg-config opencv --cflags`
opencv_ldflags ?= `pkg-config opencv --libs`

# lib: boost
boost_dir ?= /usr/local
boost_cppflags ?= -I$(boost_dir)/include
boost_ldflags ?= -L$(boost_dir)

# lib: cinder
cinder_dir ?= /Users/join/src/cinder_0.8.2_mac
cinderopencv_dir ?= $(cinder_dir)/blocks/opencv
cinder_cppflags ?= -I$(cinder_dir)/include -I$(cinderopencv_dir)/include -Wno-reorder
cinder_ldflags ?= $(cinder_dir)/lib/libcinder_d.a -framework Accelerate -framework AudioToolbox -framework AudioUnit -framework CoreAudio -framework CoreVideo -framework OpenGL -framework QTKit -framework QuickTime -framework Cocoa

# lib: freenect
freenect_dir ?= /usr/local
freenect_cppflags ?= -I$(freenect_dir)/include/libfreenect
freenect_ldflags ?= -L$(freenect_dir)/lib -lfreenect

# lib: figtree
figtree_dir ?= /Users/join/src/figtree-0.9.3
figtree_cppflags ?= -I$(figtree_dir)/include
figtree_ldflags ?= -L$(figtree_dir)/lib -lfigtree -lann_figtree_version

# lib: tuio
tuio_dir = libTUIO
tuio_lib = $(tuio_dir)/libTUIO.a
tuio_cppflags = -I$(tuio_dir)/TUIO
tuio_ldflags = $(tuio_lib)

# lib: oscpack
oscpack_dir = liboscpack
oscpack_lib = $(oscpack_dir)/liboscpack.a
oscpack_cppflags = -I$(oscpack_dir)
oscpack_ldflags = $(oscpack_lib)

# lib: cvblob
cvblob_cppflags ?= -Ilibcvblob

# lib: boost
boost_dir = /usr/local
boost_cppflags = -I$(boost_dir)
boost_ldflags = -L$(boost_dir) -lboost_thread-mt

# lib: reactivision
reactivision_dir = libreactivision
reactivision_lib = $(reactivision_dir)/libreactivision.a
reactivision_cppflags = -I$(reactivision_dir)/ext/libfidtrack -I$(reactivision_dir)/ext/libdtouch -I$(reactivision_dir)/ext/tinyxml -I$(reactivision_dir)/common
reactivision_ldflags = $(reactivision_lib)

# lib: artoolkitplus
artoolkitplus_dir = libartoolkitplus
artoolkitplus_lib = $(artoolkitplus_dir)/libartoolkitplus.a
artoolkitplus_cppflags = -I$(artoolkitplus_dir)/include
artoolkitplus_ldflags = $(artoolkitplus_lib)

# lib: cmtf
ctmf_dir = libctmf
ctmf_lib = $(ctmf_dir)/ctmf.o
ctmf_cppflags = -I$(ctmf_dir)
ctmf_ldflags = $(ctmf_lib)

# lib: openni
openni_cppflags = -I/usr/include/ni
openni_ldflags = -lOpenNI

# lib: stage
stage_dir = libstage
stage_lib = $(stage_dir)/libstage.a
stage_cppflags = -I$(stage_dir)
stage_ldflags = $(stage_lib)

# libs:
librairies = opencv cinder freenect cvblob tuio oscpack boost reactivision artoolkitplus ctmf openni stage

# Concaténation de toutes les choses nécessaires aux librairies
librairies_cppflags = $(foreach lib,$(librairies),$($(addsuffix _cppflags,$(lib))))
librairies_ldflags = $(foreach lib,$(librairies),$($(addsuffix _ldflags,$(lib))))

# Options à passer au compilateur
CPPFLAGS ?= -g -Wall -Wno-unknown-pragmas
CPPFLAGS += $(addprefix -I,$(chercher_dans))
CPPFLAGS += $(librairies_cppflags)
CPPFLAGS += -arch i386

# Options à passer à l'éditeur de liens
LDFLAGS ?= -lm
LDFLAGS += $(addprefix -L,$(chercher_dans))
LDFLAGS += $(librairies_ldflags)
LDFLAGS += -arch i386

# Tous les programmes sont pourvus de l'extension .src
programmes = $(patsubst %.src,%,$(wildcard *.src))

cvblob_objs = $(patsubst %.cpp,%.o,$(wildcard libcvblob/*.cpp))
oscpack_srcs = ./ip/IpEndpointName.cpp ./ip/posix/NetworkingUtils.cpp ./ip/posix/UdpSocket.cpp ./osc/OscOutboundPacketStream.cpp ./osc/OscPrintReceivedElements.cpp ./osc/OscReceivedElements.cpp ./osc/OscTypes.cpp
oscpack_objs = $(addprefix $(oscpack_dir), $(patsubst %.cpp,%.o,$(oscpack_srcs))
reactivision_objs = $(patsubst %.c,%.o,$(wildcard $(reactivision_dir)/ext/libfidtrack/*.c)) $(patsubst %.c,%.o,$(wildcard $(reactivision_dir)/common/*.c)) $(patsubst %.cpp,%.o,$(wildcard $(reactivision_dir)/common/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard $(reactivision_dir)/ext/libfidtrack/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard $(reactivision_dir)/ext/libdtouch/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard $(reactivision_dir)/ext/tinyxml/*.cpp))
stage_objs = $(patsubst %.cpp,%.o,$(wildcard $(stage_dir)/*.cpp))
artoolkitplus_objs = $(patsubst %.cpp,%.o,$(wildcard $(artoolkitplus_dir)/src/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard $(artoolkitplus_dir)/src/extra/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard $(artoolkitplus_dir)/src/librpp/*.cpp))
# $(patsubst %.cxx,%.o,$(wildcard $(artoolkitplus_dir)/src/*.cxx)) $(patsubst %.cxx,%.o,$(wildcard $(artoolkitplus_dir)/src/core/*.cxx)) $(patsubst %.cxx,%.o,$(wildcard $(artoolkitplus_dir)/src/extra/*.cxx))

# Fait des variables avec la liste des objets à créer à chaque fois, en fonction des sources
$(foreach programme, $(programmes), $(eval $(programme)_objs = $(cvblob_objs) $(patsubst %.cpp,%.o,$(wildcard $(programme).src/*.cpp))))

###########################

.PHONY: all
.SECONDARY:

all: $(ctmf_lib) $(oscpack_lib) $(reactivision_lib) $(artoolkitplus_lib) $(tuio_lib) $(stage_lib) $(addsuffix .app,$(programmes))

$(oscpack_lib):
	CC="gcc -m32" CXX="g++ -m32" make liboscpack.a --directory=$(oscpack_dir)

$(reactivision_lib): $(reactivision_objs)
	ar rcs $@ $^

$(ctmf_lib):
	make --directory=$(ctmf_dir)

$(stage_lib): $(stage_objs) $(reactivision_lib) $(artoolkitplus_lib)
	ar rcs $@ $(stage_objs)

$(artoolkitplus_lib): $(artoolkitplus_objs)
	ar rcs $@ $^

$(tuio_lib):
	CC="gcc -m32" CXX="g++ -m32" make --directory=$(tuio_dir)

%.app: %
	mkdir -p $@/Contents/MacOS $@/Contents/Resources
	mv $^ $@/Contents/MacOS
	cat Info.plist | sed s/Stage/$^/g > $@/Contents/Info.plist
	cp PkgInfo $@/Contents
	cp $(reactivision_dir)/macosx/camera.xml $@/Contents/Resources
	cp $(reactivision_dir)/macosx/reacTIVision.xml $@/Contents/Resources
	cp $(reactivision_dir)/symbols/amoeba/*.trees $@/Contents/Resources

%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $^


%.o: %.cxx
	$(COMPILE.cpp) $(OUTPUT_OPTION) $^

%.o: %.c
	$(COMPILE.c) $(OUTPUT_OPTION) $^

.PHONY: clean
clean:
	rm -rf *.o *.d $(programmes) $(addsuffix .dSYM,$(programmes)) $(addsuffix .app,$(programmes)) $(addsuffix .src/*.o,$(programmes)) $(stage_objs)
	# $(reactivision_lib) $(reactivision_objs) $(artoolkitplus_lib) $(artoolkitplus_objs)

.SECONDEXPANSION:
$(programmes): $$($$@_objs)
	$(LINK.cpp) $(OUTPUT_OPTION) $^
