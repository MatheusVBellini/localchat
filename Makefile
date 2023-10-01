CXXFLAGS  += -Wall -Wextra -Wpedantic -Iinclude -std=c++11
LDFLAGS   +=

SERVER = build/server.out
CLIENT = build/client.out

CXXFILES = $(wildcard src/**/*.cpp)
OFILES   = $(patsubst src/%.cpp,build/obj/%.o, $(CXXFILES))

.PHONY: all clean run debug

all: $(SERVER) $(CLIENT)

clean:
	@rm -rf build/

run: $(SERVER) $(CLIENT)
	@./$(SERVER) $(SERVER_ARGS) 2>build/server.logs && ./$(CLIENT) $(CLIENT_ARGS) 2>./build/client.logs

debug: CXXFLAGS+=-g3 -O0 -DDEBUG
debug: clean
debug: $(SERVER) $(CLIENT)


$(SERVER): $(filter-out build/obj/client/%.o,$(OFILES))
	@mkdir -p build
	$(CXX) $(LDFLAGS) -o $@ $^

$(CLIENT): $(filter-out build/obj/server/%.o,$(OFILES))
	@mkdir -p build
	$(CXX) $(LDFLAGS) -o $@ $^

build/obj/%.o: src/%.cpp src/%.h
	@mkdir -p $(shell echo $< |sed 's|src\(.*\)/[^/]*|build/obj\1|')
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/obj/%.o: src/%.cpp
	@mkdir -p $(shell echo $< |sed 's|src\(.*\)/[^/]*|build/obj\1|')
	$(CXX) $(CXXFLAGS) -c -o $@ $<

