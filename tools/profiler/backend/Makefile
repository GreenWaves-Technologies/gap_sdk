BUILD_DIR =build
TARGET_LIB = $(BUILD_DIR)/libprofiling.a
SRC_DIR =src
OBJ_DIR =$(BUILD_DIR)/obj
DEP_DIR =$(BUILD_DIR)/dep
INCLUDE_DIR = include $(DUMPER_DIR)

CXX = g++
#CXXFLAGS = -fPIC -Wall -Wextra -Werror -O2 -DDEBUG -g --std=c++14
CXXFLAGS = -g -fPIC -Wall -Wextra -Werror --std=c++14
CXXFLAGS += $(foreach dir, $(INCLUDE_DIR), -I $(dir))
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d
RM = rm -f

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
DEPS = $(OBJS:$(OBJ_DIR)/%.o=$(DEP_DIR)/%.d)

.PHONY: all clean

all: $(BUILD_DIR) $(OBJ_DIR) $(DEP_DIR) $(TARGET_LIB)

$(BUILD_DIR) $(OBJ_DIR) $(DEP_DIR): ; @mkdir -p $@

$(TARGET_LIB): $(OBJS)
	ar rvs $@ $^

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(DEP_DIR)/%.d | $(DEP_DIR)
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) -o $@ -c $<

clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(DEPS)
	-$(RM) -r $(BUILD_DIR)

$(DEPS):

-include $(wildcard $(DEPS))
