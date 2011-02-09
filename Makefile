# BUILD SETTINGS ###########################################

TARGET := ComboShooter

############################################################

STRIP := strip

OBJS := $(shell find src/ -name *.cpp | sed "s/^src\///")
OBJS := $(foreach obj, $(OBJS:.cpp=.o), obj/$(obj))

# FLAGS ####################################################

ifneq ($(MAKECMDGOALS), release)
    EXTRA_CXXFLAGS += -g3 -O0 -Werror
else
    EXTRA_CXXFLAGS += -g0 -O2 -DNDEBUG
endif
EXTRA_CXXFLAGS += -pedantic -Wall -Wextra

ALL_CXXFLAGS += -std=c++0x -MMD -I./src -DTARGET_$(PLATFORM) $(EXTRA_CXXFLAGS) $(CXXFLAGS)
ALL_LDFLAGS += $(LDFLAGS)
LDLIBS += -lm -lglfw -lGL -lpng

# RULES ####################################################

.PHONY : all release clean

all : $(TARGET)

release : all
	$(STRIP) $(TARGET)

clean :
	rm -rf obj/*
	rm -f $(TARGET)

ifneq ($(MAKECMDGOALS), clean)
    -include $(OBJS:.o=.d)
endif

$(TARGET) : $(OBJS)
	$(CXX) -o $@ $(ALL_LDFLAGS) $^ $(LDLIBS)

obj/%.o : src/%.cpp
	@mkdir -p "$(dir $@)"
	$(CXX) -c -o $@ $(ALL_CXXFLAGS) $<

