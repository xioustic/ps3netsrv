CELL_MK_DIR = $(CELL_SDK)/samples/mk
include $(CELL_MK_DIR)/sdk.makedef.mk

PEXPORTPICKUP = ppu-lv2-prx-exportpickup

CRT_HEAD += $(shell ppu-lv2-gcc -print-file-name'='ecrti.o)
CRT_HEAD += $(shell ppu-lv2-gcc -print-file-name'='crtbegin.o)
CRT_TAIL += $(shell ppu-lv2-gcc -print-file-name'='crtend.o)
CRT_HEAD += $(shell ppu-lv2-gcc -print-file-name'='ecrtn.o)

#PPU_SRCS = network.c main.c
PPU_SRCS = main.c
PPU_INCDIRS	+= -I./vsh/inc
PPU_PRX_TARGET = video_rec.prx
PPU_PRX_LDFLAGS += -L./vsh/lib -Wl,--strip-unused-data
PPU_PRX_STRIP_FLAGS = -s

PPU_PRX_LDLIBS 	+= -lfs_stub -lio_stub -lrtc_stub \
                   -lvsh_export_stub \
                   -lpaf_export_stub \
                   -lvshmain_export_stub \
                   -lvshtask_export_stub \
                   -lallocator_export_stub \
                   -lstdc_export_stub \
                   -lxsetting_export_stub

PPU_CFLAGS += -Os -ffunction-sections -fdata-sections -fno-builtin-printf -nodefaultlibs -std=gnu99 -Wno-shadow -Wno-unused-parameter

all:
	$(MAKE) $(PPU_OBJS_DEPENDS)
	$(PPU_PRX_STRIP) --strip-debug --strip-section-header $(PPU_PRX_TARGET)
	$(MAKE_FSELF) $(PPU_PRX_TARGET) $(PPU_SPRX_TARGET)

include $(CELL_MK_DIR)/sdk.target.mk




