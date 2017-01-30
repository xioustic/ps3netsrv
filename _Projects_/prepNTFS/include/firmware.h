#define SC_PEEK_LV2 	(6)

static float c_firmware=0.0f;
static u8 dex_mode=0;

static inline uint64_t peekq(uint64_t addr) //lv2
{
	lv2syscall1(SC_PEEK_LV2, addr);
	return (uint64_t) p1;
}

static void detect_firmware(void)
{
	const uint64_t CEX = 0x4345580000000000ULL;
	const uint64_t DEX = 0x4445580000000000ULL;
	const uint64_t DEH = 0x4445480000000000ULL;

	dex_mode = 0;

	if(peekq(0x80000000002ED808ULL)==CEX) {c_firmware=4.80f;}				else
	if(peekq(0x80000000002ED818ULL)==CEX) {c_firmware=(peekq(0x80000000002FCB68ULL)==0x323031352F31322FULL)?4.78f:(peekq(0x80000000002FCB68ULL)==0x323031352F30382FULL)?4.76f:4.75f;} else
	if(peekq(0x80000000002ED778ULL)==CEX) {c_firmware=4.70f;}				else
	if(peekq(0x80000000002ED860ULL)==CEX) {c_firmware=(peekq(0x80000000002FC938ULL)==0x323031342F31312FULL)?4.66f:4.65f;} else
	if(peekq(0x80000000002ED850ULL)==CEX) {c_firmware=4.60f;}				else
	if(peekq(0x80000000002EC5E0ULL)==CEX) {c_firmware=4.55f;}				else
	if(peekq(0x80000000002E9D70ULL)==CEX) {c_firmware=4.53f;}				else
	if(peekq(0x80000000002E9BE0ULL)==CEX) {c_firmware=4.50f;}				else
	if(peekq(0x80000000002EA9B8ULL)==CEX) {c_firmware=4.46f;}				else
	if(peekq(0x80000000002EA498ULL)==CEX) {c_firmware=4.41f;}				else
	if(peekq(0x80000000002EA488ULL)==CEX) {c_firmware=4.40f;}				else
	if(peekq(0x80000000002E9F18ULL)==CEX) {c_firmware=4.31f;}				else
	if(peekq(0x80000000002E9F08ULL)==CEX) {c_firmware=4.30f;}				else
	if(peekq(0x80000000002E8610ULL)==CEX) {c_firmware=4.21f;}				else
	if(peekq(0x80000000002D83D0ULL)==CEX) {c_firmware=3.55f;}				else

	if(peekq(0x800000000030F3A0ULL)==DEX) {c_firmware=4.80f; dex_mode=2;}	else
	if(peekq(0x800000000030F2D0ULL)==DEX) {c_firmware=(peekq(0x800000000031EF48ULL)==0x323031352F31322FULL)?4.78f:(peekq(0x800000000031EF48ULL)==0x323031352F30382FULL)?4.76f:4.75f; dex_mode=2;}	else
	if(peekq(0x800000000030F240ULL)==DEX) {c_firmware=4.70f; dex_mode=2;}	else
	if(peekq(0x800000000030F1A8ULL)==DEX) {c_firmware=(peekq(0x800000000031EBA8ULL)==0x323031342F31312FULL)?4.66f:4.65f; dex_mode=2;} else
	if(peekq(0x800000000030F198ULL)==DEX) {c_firmware=4.60f; dex_mode=2;}	else
	if(peekq(0x800000000030D6A8ULL)==DEX) {c_firmware=4.55f; dex_mode=2;}	else
	if(peekq(0x800000000030AEA8ULL)==DEX) {c_firmware=4.53f; dex_mode=2;}	else
	if(peekq(0x8000000000309698ULL)==DEX) {c_firmware=4.50f; dex_mode=2;}	else
	if(peekq(0x8000000000305410ULL)==DEX) {c_firmware=4.46f; dex_mode=2;}	else
	if(peekq(0x8000000000304EF0ULL)==DEX) {c_firmware=4.41f; dex_mode=2;}	else
	if(peekq(0x8000000000304EE0ULL)==DEX) {c_firmware=4.40f; dex_mode=2;}	else
	if(peekq(0x8000000000304640ULL)==DEX) {c_firmware=4.31f; dex_mode=2;}	else
	if(peekq(0x8000000000304630ULL)==DEX) {c_firmware=4.30f; dex_mode=2;}	else
	if(peekq(0x8000000000302D88ULL)==DEX) {c_firmware=4.21f; dex_mode=2;}	else
	if(peekq(0x80000000002EFE20ULL)==DEX) {c_firmware=3.55f; dex_mode=2;}	else

	if(peekq(0x8000000000319F78ULL)==DEH) {c_firmware=3.55f; dex_mode=1;}	else
	if(peekq(0x800000000032EDC8ULL)==DEH) {c_firmware=4.60f; dex_mode=1;}	else
	if(peekq(0x800000000032EB60ULL)==DEH) {c_firmware=(peekq(0x8000000000344B70ULL)==0x323031352F31322FULL)?4.78f:(peekq(0x8000000000344B70ULL)==0x323031352F30382FULL)?4.76f:4.75f; dex_mode=1;}	else
	if(peekq(0x800000000032EB60ULL)==DEH) {c_firmware=4.80f; dex_mode=1;}	else

	if(peekq(0x80000000002CFF98ULL)==CEX) {c_firmware=3.41f;}
	//if(peekq(0x80000000002E79C8ULL)==DEX) {c_firmware=3.41f; dex_mode=2;}	else
}
