APP:= no_entry

CC:= g++

NVDS_VERSION:= 5.1

SRCS:= $(wildcard *.cpp)

INCS:= $(wildcard *.hpp)

PKGS:= gstreamer-1.0 gstreamer-base-1.0 gstreamer-video-1.0

CFLAGS+= $(shell pkg-config --cflags $(PKGS))

LIBS+= $(shell pkg-config --libs $(PKGS))

OBJS:= $(SRCS:.cpp=.o)

all: $(APP)

%.o: %.cpp $(INCS) 
	$(CC) -c $(CFLAGS) $< -o $@

$(APP): $(OBJS) Makefile
	$(CC) $(OBJS) $(LIBS) -o $(APP)

clean:
	rm -rf $(OBJS) $(LIB)
	rm $(APP)
