ROMNAME = catadventure
ROMTITLE = "CAT ADVENTURE 64"
FINAL = 0
DEBUG = 1

BUILD_DIR = build
ASSETS_DIR = assets
FILESYSTEM_DIR = filesystem
SRC_DIR = code

SRC = $(SRC_DIR)/main.c

# Toolchain paths
include $(N64_INST)/include/n64.mk
include $(N64_INST)/include/t3d.mk

N64_ROM_SAVETYPE = eeprom4k

ifeq ($(FINAL),1)
  N64_ROM_ELFCOMPRESS = 3
  MKSPRITE_FLAGS = --compress 2
endif

ifeq ($(DEBUG), 1)
  N64_CFLAGS += -g -DDEBUG=$(DEBUG)
  N64_LDFLAGS += -g
endif

# Asset conversion rules
$(FILESYSTEM_DIR)/%.sprite: $(ASSETS_DIR)/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/%.font64: $(ASSETS_DIR)/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/%.t3dm: $(ASSETS_DIR)/%.glb
	@mkdir -p $(dir $@)
	@echo "    [3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) $(T3DM_FLAGS) "$<" $@

$(FILESYSTEM_DIR)/%.wav64: $(ASSETS_DIR)/%.wav
	@mkdir -p $(dir $@)
	@echo "    [AUDIO-WAV] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/%.wav64: $(ASSETS_DIR)/%.mp3
	@mkdir -p $(dir $@)
	@echo "    [AUDIO-MP3] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/%.xm64: $(ASSETS_DIR)/%.xm
	@mkdir -p $(dir $@)
	@echo "    [MUSIC] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/%.desc: $(ASSETS_DIR)/%.txt
	@mkdir -p $(dir $@)
	@echo "    [DESCRIPTION] $@"
	cp "$<" $@

# Build rules
all: $(ROMNAME).z64

$(BUILD_DIR)/$(ROMNAME).elf: $(SRC:%.c=$(BUILD_DIR)/%.o)
$(ROMNAME).z64: N64_ROM_TITLE=$(ROMTITLE)
$(ROMNAME).z64: $(BUILD_DIR)/$(ROMNAME).dfs $(BUILD_DIR)/$(ROMNAME).msym

clean:
	rm -rf $(BUILD_DIR) $(FILESYSTEM_DIR)/* $(ROMNAME).z64 

# Include dependency files
-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
