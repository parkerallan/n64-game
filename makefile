ROMNAME = catadventure
ROMTITLE = "CAT ADVENTURE 64"
FINAL = 0
DEBUG = 1

BUILD_DIR = build
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
assets_png = $(wildcard assets/*.png)
assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite)))

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o $(dir $@) "$<"

filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -o $(dir $@) "$<"

filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) $(T3DM_FLAGS) "$<" $@

filesystem/%.wav64: assets/%.wav
	@mkdir -p $(dir $@)
	@echo "    [AUDIO-WAV] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

filesystem/%.wav64: assets/%.mp3
	@mkdir -p $(dir $@)
	@echo "    [AUDIO-MP3] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

filesystem/%.xm64: assets/%.xm
	@mkdir -p $(dir $@)
	@echo "    [MUSIC] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

filesystem/%.desc: assets/%.txt
	@mkdir -p $(dir $@)
	@echo "    [DESCRIPTION] $@"
	cp "$<" $@

# Build rules
all: $(ROMNAME).z64

$(BUILD_DIR)/$(ROMNAME).dfs: $(assets_conv)
$(BUILD_DIR)/$(ROMNAME).elf: $(SRC:%.c=$(BUILD_DIR)/%.o)

$(ROMNAME).z64: N64_ROM_TITLE=$(ROMTITLE)
$(ROMNAME).z64: $(BUILD_DIR)/$(ROMNAME).dfs $(BUILD_DIR)/$(ROMNAME).msym

clean:
	rm -rf $(BUILD_DIR) filesystem/* $(ROMNAME).z64 

# Include dependency files
-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
