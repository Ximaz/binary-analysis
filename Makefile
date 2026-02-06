CC              := cc
AR              := ar rcs
STRIP_BIN       := strip

NAME            := binary-analysis
SRC_DIR         := src
INC_DIR         := include
BUILD_DIR       := build

SRCS            := $(shell find $(SRC_DIR) -name '*.c')
OBJS            := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS            := $(OBJS:.o=.d)

UNAME_S         := $(shell uname -s)

PROFILE         ?= DEBUG

CSTD            := -ansi -pedantic-errors

WARNINGS        := -Wall -Wextra -Werror \
                   -Wstrict-prototypes \
                   -Wmissing-prototypes \
                   -Wshadow \
                   -Wpointer-arith \
                   -Wcast-align \
                   -Wwrite-strings \
                   -Wcast-qual \
                   -Wundef \
                   -Wformat=2 \
                   -Wformat-security \
                   -Wconversion \
                   -Wsign-conversion \
                   -Wnested-externs \
                   -Wbad-function-cast \
                   -Wold-style-definition \
                   -Wstrict-overflow=5 \
                   -Winit-self \
                   -Wmissing-declarations \
                   -Wunreachable-code \
                   -Wfloat-equal \
                   -Wredundant-decls

HARDEN_CFLAGS   := -fno-common \
                   -fno-strict-aliasing \
                   -fno-strict-overflow \
                   -fno-delete-null-pointer-checks \
                   -fstack-protector-all

LDFLAGS_BASE    :=

# --- LINUX (GNU) ---
ifneq ($(UNAME_S),Darwin)
    HARDEN_CFLAGS  += -fstack-clash-protection \
                      -fcf-protection=full \
                      -fPIE

    HARDEN_LDFLAGS := -Wl,-z,relro \
                      -Wl,-z,now \
                      -Wl,-z,noexecstack \
                      -pie
endif

CPPFLAGS        := -I$(INC_DIR) -MMD -MP

OPTIMIZATION	:=	-O0

ifeq ($(PROFILE),DEBUG)
    CFLAGS_PROFILE := -g3
    LDFLAGS_BASE   += -g3
else
    CFLAGS_PROFILE := -DNDEBUG
    ifeq ($(UNAME_S),Linux)
        LDFLAGS_BASE += -s
    endif
endif

CFLAGS          := $(CSTD) $(WARNINGS) $(HARDEN_CFLAGS) $(OPTIMIZATION) $(CFLAGS_PROFILE)
LDFLAGS         := $(LDFLAGS_BASE) $(HARDEN_LDFLAGS)

.PHONY: all clean fclean re info

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
ifeq ($(UNAME_S),Darwin)
ifeq ($(PROFILE),RELEASE)
	$(STRIP_BIN) $@
endif
endif
	@echo "Build Complete. Profile: $(PROFILE)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

info:
	@echo "OS Detected:     $(UNAME_S)"
	@echo "Current Profile: $(PROFILE)"
	@echo "CFLAGS:          $(CFLAGS)"
	@echo "LDFLAGS:         $(LDFLAGS)"

-include $(DEPS)
