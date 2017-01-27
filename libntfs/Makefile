CELL_SDK ?= /c/cell
CELL_MK_DIR ?= $(CELL_SDK)/samples/mk

include $(CELL_MK_DIR)/sdk.makedef.mk

PPU_LIB_TARGET	= libntfs_prx.a
PPU_INCDIRS = -Iinclude
PPU_SRCS = $(wildcard source/*.c)
DEFINES += -DBIGENDIAN -D__CELLOS_LV2__ -DPS3_GEKKO -DHAVE_CONFIG_H

PPU_CFLAGS := -Os -Wall -fmerge-all-constants -mcpu=cell -fno-strict-aliasing $(PPU_INCDIRS) $(DEFINES) -std=gnu99 -Wno-shadow -Wno-unused-parameter
# -ffunction-sections -fdata-sections -fno-builtin-printf -nodefaultlibs

include $(CELL_MK_DIR)/sdk.target.mk


