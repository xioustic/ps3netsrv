#define SC_SYS_CONTROL_LED				(386)

#define GREEN	1
#define RED		2
#define YELLOW	2 //RED+GREEN (RED alias due green is already on)

#define OFF			0
#define ON			1
#define BLINK_FAST	2
#define BLINK_SLOW	3

static void led(u64 color, u64 mode)
{
	system_call_2(SC_SYS_CONTROL_LED, (u64)color, (u64)mode);
}
