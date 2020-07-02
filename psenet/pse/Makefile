CXXFLAGS = -I include  -std=c++11 -O3 $(shell python3-config --cflags)
LDFLAGS = $(shell python3-config --ldflags)

DEPS = $(shell find include -xtype f)
CXX_SOURCES = pse.cpp

LIB_SO = pse.so

$(LIB_SO): $(CXX_SOURCES) $(DEPS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(CXX_SOURCES) --shared -fPIC

clean:
	rm -rf $(LIB_SO)
