#define SC_STORAGE_OPEN 				(600)
#define SC_STORAGE_CLOSE 				(601)
#define SC_STORAGE_INSERT_EJECT			(616)

#define ATA_HDD				0x101000000000007ULL
#define BDVD_DRIVE			0x101000000000006ULL
#define PATA0_HDD_DRIVE		0x101000000000008ULL
#define PATA0_BDVD_DRIVE	BDVD_DRIVE
#define PATA1_HDD_DRIVE		ATA_HDD
#define PATA1_BDVD_DRIVE	0x101000000000009ULL
#define BUILTIN_FLASH		0x100000000000001ULL
#define MEMORY_STICK		0x103000000000010ULL
#define SD_CARD				0x103000100000010ULL
#define COMPACT_FLASH		0x103000200000010ULL

#define USB_MASS_STORAGE_1(n)	(0x10300000000000AULL+n) /* For 0-5 */
#define USB_MASS_STORAGE_2(n)	(0x10300000000001FULL+(n-6)) /* For 6-127 */
#define USB_MASS_STORAGE(n)		(((n) < 6) ? USB_MASS_STORAGE_1(n) : USB_MASS_STORAGE_2(n))

#define	HDD_PARTITION(n)	(ATA_HDD | ((uint64_t)n<<32))
#define FLASH_PARTITION(n)	(BUILTIN_FLASH | ((uint64_t)n<<32))

#define DEVICE_TYPE_PS3_DVD	0xFF70
#define DEVICE_TYPE_PS3_BD	0xFF71
#define DEVICE_TYPE_PS2_CD	0xFF60
#define DEVICE_TYPE_PS2_DVD	0xFF61
#define DEVICE_TYPE_PSX_CD	0xFF50
#define DEVICE_TYPE_BDROM	0x40
#define DEVICE_TYPE_BDMR_SR	0x41 /* Sequential record */
#define DEVICE_TYPE_BDMR_RR 0x42 /* Random record */
#define DEVICE_TYPE_BDMRE	0x43
#define DEVICE_TYPE_DVD		0x10 /* DVD-ROM, DVD+-R, DVD+-RW etc, they are differenced by booktype field in some scsi command */
#define DEVICE_TYPE_CD		0x08 /* CD-ROM, CD-DA, CD-R, CD-RW, etc, they are differenced somehow with scsi commands */
#define DEVICE_TYPE_USB		0x00

static void eject_insert(u8 eject, u8 insert);

#ifdef COBRA_ONLY

static int fake_insert_event(uint64_t devicetype, uint64_t disctype)
{
	uint64_t param = (uint64_t)(disctype) << 32ULL;
	sys_storage_ext_fake_storage_event(7, 0, devicetype);
	return sys_storage_ext_fake_storage_event(3, param, devicetype);
}

static int fake_eject_event(uint64_t devicetype)
{
	sys_storage_ext_fake_storage_event(4, 0, devicetype);
	return sys_storage_ext_fake_storage_event(8, 0, devicetype);
}

static void reset_usb_ports(char *_path)
{
	u8 usb6 = (u8)val(drives[5] + 8);
	u8 usb7 = (u8)val(drives[6] + 8);

	// eject all usb devices
	for(u8 f0 = 0; f0 < 8; f0++) fake_eject_event(USB_MASS_STORAGE(f0));

	if(usb6 >= 8) fake_eject_event(USB_MASS_STORAGE_2(usb6));
	if(usb7 >= 8) fake_eject_event(USB_MASS_STORAGE_2(usb7));

	sys_ppu_thread_sleep(1); u8 indx = (u8)val(_path + 8);

	// make the current usb device the first
	fake_insert_event(USB_MASS_STORAGE(indx), DEVICE_TYPE_USB);

	sys_ppu_thread_sleep(3);

	// send fake insert event for the other usb devices
	for(u8 f0 = 0; f0 < 8; f0++)
	{
		if(f0 != indx) fake_insert_event(USB_MASS_STORAGE(f0), DEVICE_TYPE_USB);
	}

	if((usb6 >= 8) && (indx != usb6)) fake_insert_event(USB_MASS_STORAGE_2(usb6), DEVICE_TYPE_USB);
	if((usb7 >= 8) && (indx != usb7)) fake_insert_event(USB_MASS_STORAGE_2(usb7), DEVICE_TYPE_USB);
}

/*
static u32 in_cobra(u32 *mode)
{
	system_call_2(SC_COBRA_SYSCALL8, (uint32_t) 0x7000, (uint32_t)mode);
	return_to_user_prog(uint32_t);
}
*/

/*
static uint64_t syscall_837(const char *device, const char *format, const char *point, u32 a, u32 b, u32 c, void *buffer, u32 len)
{
	system_call_8(SC_FS_MOUNT, (u64)device, (u64)format, (u64)point, a, b, c, (u64)buffer, len);
	return_to_user_prog(uint64_t);
}
*/
#endif

static void eject_insert(u8 eject, u8 insert)
{
	u8 atapi_cmnd2[56];
	u8* atapi_cmnd = atapi_cmnd2;
	int dev_id;

	{system_call_4(SC_STORAGE_OPEN, BDVD_DRIVE, 0, (uint64_t)(u32) &dev_id, 0);}

	if(eject)
	{
		memset(atapi_cmnd, 0, 56);
		atapi_cmnd[0x00] = 0x1b;
		atapi_cmnd[0x01] = 0x01;
		atapi_cmnd[0x04] = 0x02;
		atapi_cmnd[0x23] = 0x0c;

		// Eject disc
		{system_call_7(SC_STORAGE_INSERT_EJECT, dev_id, 1, (uint64_t)(u32) atapi_cmnd, 56, NULL, 0, NULL);}

		if(insert) sys_ppu_thread_sleep(2);
	}

	if(insert)
	{
		memset(atapi_cmnd, 0, 56);
		atapi_cmnd[0x00] = 0x1b;
		atapi_cmnd[0x01] = 0x01;
		atapi_cmnd[0x04] = 0x03;
		atapi_cmnd[0x23] = 0x0c;

		// Insert disc
		{system_call_7(SC_STORAGE_INSERT_EJECT, dev_id, 1, (uint64_t)(u32) atapi_cmnd, 56, NULL, 0, NULL);}
	}

	{system_call_1(SC_STORAGE_CLOSE, dev_id);}
}
