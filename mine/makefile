CXX = g++ -std=c++17


SRCS = *.cpp

PLUGINSRCS = $(wildcard PluginSrc/*.cpp)
PLUGINOBJS = $(PLUGINSRCS:.cpp=.o)

PLUGINPATH = Plugins/
PLUGINSRCPATH = PluginSrc/

TARGET = main

all: sources objects $(PLUGINOBJS)

sources:
# -rdynamic is needed so that the .so files can reference to the correct instance singleton
	$(CXX) -rdynamic $(SRCS) -ldl -lffi -lgmsh -pthread -g -o $(TARGET)

objects:
#   Creates objects so that the .so can use them
	$(CXX) -c -fPIC -shared Core.cpp -o libCore.so
#   The following objects are created so that we can link them to the rust plguins
	$(CXX) -c -fPIC -shared gmshFacade.cpp -o libgmshFacade.so

$(PLUGINOBJS): %.o: %.cpp
	$(CXX) -c -fPIC $< -o $@
	$(CXX) -shared $@ libCore.so -o $(PLUGINPATH)$(notdir $(basename $@)).so 
#	Gives the .so and the .cpp of the plugin as arguments to metaDataCreator
#	the .cpp is given to extract last modified time
	MetaDataTest/metaDataCreator $(PLUGINPATH)$(notdir $(basename $@)).so $(PLUGINSRCPATH)$(notdir $(basename $@)).cpp
	
plugins:  $(PLUGINOBJS)

clean:
	rm -f $(TARGET)
	rm -f $(wildcard *.o)
	rm -f $(wildcard PluginSrc/*.o)
