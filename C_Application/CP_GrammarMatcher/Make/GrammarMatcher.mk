COMPONENT := CP_GrammarMatcher

SOURCES += $(shell find $(SRC)/$(COMPONENT)/Source -type f -name '*.cpp')
FLAGS += -I$(SRC)/$(COMPONENT)/Include