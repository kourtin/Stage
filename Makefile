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

# lib: cvblob
cvblob_cppflags ?= -Ilibcvblob

# libs:
librairies = opencv cinder freenect cvblob

# Concaténation de toutes les choses nécessaires aux librairies
librairies_cppflags = $(foreach lib,$(librairies),$($(addsuffix _cppflags,$(lib))))
librairies_ldflags = $(foreach lib,$(librairies),$($(addsuffix _ldflags,$(lib))))

# Options à passer au compilateur
CPPFLAGS ?= -g -Wall
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

# Fait des variables avec la liste des objets à créer à chaque fois, en fonction des sources
$(foreach programme, $(programmes), $(eval $(programme)_objs = $(cvblob_objs) $(patsubst %.cpp,%.o,$(wildcard $(programme).src/*.cpp))))

###########################

.PHONY: all
.SECONDARY:

all: $(programmes)

%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $^

.PHONY: clean
clean:
	rm -rf *.o *.d $(programmes) $(addsuffix .dSYM,$(programmes)) $(addsuffix .src/*.o,$(programmes))

.SECONDEXPANSION:
$(programmes): $$($$@_objs)
	$(LINK.cpp) $(OUTPUT_OPTION) $^
