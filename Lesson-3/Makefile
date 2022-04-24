TARGET_DEVICE = $(shell gcc -dumpmachine | cut -f1 -d -)
CXX:= g++ -std=c++17

SRCS:= lineoffire.cpp

INCS:= $(wildcard *.h)
LIB:=libirisgst_lineoffire.so

NVDS_VERSION:=5.1

CFLAGS+= -fPIC -DDS_VERSION=\"5.1.0\" \
	 -I /usr/local/cuda/include \
	 -I /opt/nvidia/deepstream/deepstream-$(NVDS_VERSION)/sources/includes

GST_INSTALL_DIR?=/opt/nvidia/deepstream/deepstream-$(NVDS_VERSION)/lib/gst-plugins/
LIB_INSTALL_DIR?=/opt/nvidia/deepstream/deepstream-$(NVDS_VERSION)/lib/

LIBS := -shared -Wl,-no-undefined \
		-L/usr/local/cuda/lib64/ -lcudart -ldl \
		-lnppc -lnppig -lnpps -lnppicc -lnppidei \
		-L$(LIB_INSTALL_DIR) -lnvdsgst_helper -lnvdsgst_meta -lnvds_meta -lnvbufsurface -lnvbufsurftransform \
		-Wl,-rpath,$(LIB_INSTALL_DIR)

OBJS:= $(SRCS:.cpp=.o)

PKGS:= gstreamer-1.0 gstreamer-base-1.0 gstreamer-video-1.0 opencv4

CFLAGS+=$(shell pkg-config --cflags $(PKGS))
LIBS+=$(shell pkg-config --libs $(PKGS))

all: $(LIB)

%.o: %.cpp $(INCS) Makefile
	@echo $(CFLAGS)
	$(CXX) -c -o $@ $(CFLAGS) $<

$(LIB): $(OBJS) Makefile
	@echo $(CFLAGS)
	$(CXX) -o $@ $(OBJS) $(LIBS)


install: $(LIB)
	cp -rv $(LIB) $(GST_INSTALL_DIR)

clean:
	rm -rf $(OBJS) $(LIB)