#define SUCCESS 0
#define FAILED -1

#define _4KB_    4096

#define USE_64BITS_LSEEK 1

#define SECTOR_FILL    2047
#define SECTOR_SIZE    2048
#define SECTOR_SIZELL  2048LL

#define FS_S_IFMT 0170000
#define FS_S_IFDIR 0040000

#define ISODCL(from, to) (to - from + 1)

static u16 wstring[1024];
static char temp_string[1024];

struct iso_primary_descriptor {
	unsigned char type						[ISODCL	(  1,	1)]; /*	711	*/
	unsigned char id						[ISODCL	(  2,	6)];
	unsigned char version					[ISODCL	(  7,	7)]; /*	711	*/
	unsigned char unused1					[ISODCL	(  8,	8)];
	unsigned char system_id					[ISODCL	(  9,  40)]; /*	aunsigned chars	*/
	unsigned char volume_id					[ISODCL	( 41,  72)]; /*	dunsigned chars	*/
	unsigned char unused2					[ISODCL	( 73,  80)];
	unsigned char volume_space_size			[ISODCL	( 81,  88)]; /*	733	*/
	unsigned char unused3					[ISODCL	( 89, 120)];
	unsigned char volume_set_size			[ISODCL	(121, 124)]; /*	723	*/
	unsigned char volume_sequence_number	[ISODCL	(125, 128)]; /*	723	*/
	unsigned char logical_block_size		[ISODCL	(129, 132)]; /*	723	*/
	unsigned char path_table_size			[ISODCL	(133, 140)]; /*	733	*/
	unsigned char type_l_path_table			[ISODCL	(141, 144)]; /*	731	*/
	unsigned char opt_type_l_path_table		[ISODCL	(145, 148)]; /*	731	*/
	unsigned char type_m_path_table			[ISODCL	(149, 152)]; /*	732	*/
	unsigned char opt_type_m_path_table		[ISODCL	(153, 156)]; /*	732	*/
	unsigned char root_directory_record		[ISODCL	(157, 190)]; /*	9.1	*/
	unsigned char volume_set_id				[ISODCL	(191, 318)]; /*	dunsigned chars	*/
	unsigned char publisher_id				[ISODCL	(319, 446)]; /*	achars */
	unsigned char preparer_id				[ISODCL	(447, 574)]; /*	achars */
	unsigned char application_id			[ISODCL	(575, 702)]; /*	achars */
	unsigned char copyright_file_id			[ISODCL	(703, 739)]; /*	7.5	dchars */
	unsigned char abstract_file_id			[ISODCL	(740, 776)]; /*	7.5	dchars */
	unsigned char bibliographic_file_id		[ISODCL	(777, 813)]; /*	7.5	dchars */
	unsigned char creation_date				[ISODCL	(814, 830)]; /*	8.4.26.1 */
	unsigned char modification_date			[ISODCL	(831, 847)]; /*	8.4.26.1 */
	unsigned char expiration_date			[ISODCL	(848, 864)]; /*	8.4.26.1 */
	unsigned char effective_date			[ISODCL	(865, 881)]; /*	8.4.26.1 */
	unsigned char file_structure_version	[ISODCL	(882, 882)]; /*	711	*/
	unsigned char unused4					[ISODCL	(883, 883)];
	unsigned char application_data			[ISODCL	(884, 1395)];
	unsigned char unused5					[ISODCL	(1396, 2048)];
};

struct iso_directory_record	{
	unsigned char length					[ISODCL	( 1,  1)]; /* 711 */
	unsigned char ext_attr_length			[ISODCL	( 2,  2)]; /* 711 */
	unsigned char extent					[ISODCL	( 3, 10)]; /* 733 */
	unsigned char size						[ISODCL	(11, 18)]; /* 733 */
	unsigned char date						[ISODCL	(19, 25)]; /* 7	by 711 */
	unsigned char flags						[ISODCL	(26, 26)];
	unsigned char file_unit_size			[ISODCL	(27, 27)]; /* 711 */
	unsigned char interleave				[ISODCL	(28, 28)]; /* 711 */
	unsigned char volume_sequence_number	[ISODCL	(29, 32)]; /* 723 */
	unsigned char name_len					[1]; /*	711	*/
	unsigned char name						[1];
};

struct iso_path_table{
	unsigned char  name_len[2];	/* 721 */
	char extent[4];				/* 731 */
	char parent[2];				/* 721 */
	char name[1];
};

static int isonum_731 (unsigned char * p)
{
	return ((p[0] & 0xff)
		 | ((p[1] & 0xff) << 8)
		 | ((p[2] & 0xff) << 16)
		 | ((p[3] & 0xff) << 24));
}

/*
static int isonum_732 (unsigned char * p)
{
	return ((p[3] & 0xff)
		 | ((p[2] & 0xff) << 8)
		 | ((p[1] & 0xff) << 16)
		 | ((p[0] & 0xff) << 24));
}
*/

static int isonum_733 (unsigned char * p)
{
	return (isonum_731 (p));
}


static int isonum_721 (unsigned char * p)
{
	return ((p[0] & 0xff) | ((p[1] & 0xff) << 8));
}

void UTF16_to_UTF8(u16 *stw, u8 *stb)
{
	while(stw[0])
	{
		if((stw[0] & 0xFF80) == 0)
		{
			*(stb++) = stw[0] & 0xFF;   // utf16 00000000 0xxxxxxx utf8 0xxxxxxx
		}
		else if((stw[0] & 0xF800) == 0)
		{
			// utf16 00000yyy yyxxxxxx utf8 110yyyyy 10xxxxxx
			*(stb++) = ((stw[0]>>6) & 0xFF) | 0xC0; *(stb++) = (stw[0] & 0x3F) | 0x80;
		}
		else if((stw[0] & 0xFC00) == 0xD800 && (stw[1] & 0xFC00) == 0xDC00)
		{
			// utf16 110110ww wwzzzzyy 110111yy yyxxxxxx (wwww = uuuuu - 1)
			// utf8  1111000uu 10uuzzzz 10yyyyyy 10xxxxxx
			*(stb++)= (((stw[0] + 64)>>8) & 0x3) | 0xF0; *(stb++)= (((stw[0]>>2) + 16) & 0x3F) | 0x80;
			*(stb++)= ((stw[0]>>4) & 0x30) | 0x80 | ((stw[1]<<2) & 0xF); *(stb++)= (stw[1] & 0x3F) | 0x80;
			stw++;
		}
		else
		{
			// utf16 zzzzyyyy yyxxxxxx utf8 1110zzzz 10yyyyyy 10xxxxxx
			*(stb++)= ((stw[0]>>12) & 0xF) | 0xE0; *(stb++)= ((stw[0]>>6) & 0x3F) | 0x80; *(stb++)= (stw[0] & 0x3F) | 0x80;
		}

		stw++;
	}

	*stb = 0;
}

#ifdef USE_64BITS_LSEEK
int get_iso_file_pos(int fd, char *path, u32 *flba, u64 *size)
#else
int get_iso_file_pos(FILE *fp, unsigned char *path, u32 *flba, u64 *size)
#endif
{
	static struct iso_primary_descriptor sect_descriptor;
	struct iso_directory_record * idr;
	static int folder_split[64][3];
	int nfolder_split = 0;

	u32 file_lba = 0xffffffff;

	u8 *sectors = NULL;

	#ifdef USE_64BITS_LSEEK
	if(fd <= 0 || !size || !flba) return FAILED;
	#else
	if(!fp || !size || !flba) return FAILED;
	#endif

	*size = 0;

	folder_split[nfolder_split][0] = 0;
	folder_split[nfolder_split][1] = 0;
	folder_split[nfolder_split][2] = 0;
	int i = 0;

	while(path[i] != 0 && i < strlen(path))
	{
		if(path[i] == '/')
		{
			folder_split[nfolder_split][2] = i - folder_split[nfolder_split][1];
			while(path[i] =='/' && i < strlen(path)) i++;
			if(folder_split[nfolder_split][2] == 0) {folder_split[nfolder_split][1] = i; continue;}
			folder_split[nfolder_split][0] = 1;
			nfolder_split++;
			folder_split[nfolder_split][0] = 0;
			folder_split[nfolder_split][1] = i;
			folder_split[nfolder_split][2] = 0;

		} else i++;
	}

	folder_split[nfolder_split][0] = 0;
	folder_split[nfolder_split][2] = i - folder_split[nfolder_split][1];
	nfolder_split++;

	#ifdef USE_64BITS_LSEEK
	if(ps3ntfs_seek64(fd, 0x8800LL, SEEK_SET) != 0x8800LL) goto err;
	if(ps3ntfs_read(fd, (void *) &sect_descriptor, sizeof(struct iso_primary_descriptor)) != sizeof(struct iso_primary_descriptor)) goto err;
	#else
	if(fseek(fp, 0x8800, SEEK_SET) != 0) goto err;
	if(fread((void *) &sect_descriptor, 1, sizeof(struct iso_primary_descriptor), fp) != sizeof(struct iso_primary_descriptor)) goto err;
	#endif

	if((sect_descriptor.type[0] != 2 && sect_descriptor.type[0] != 0xFF) || strncmp((void *) sect_descriptor.id, "CD001", 5)) goto err;

	// PSP ISO
	if(sect_descriptor.type[0] == 0xFF)
	{
		unsigned char dir_entry[0x38];
		char entry_name[0x20];

		#ifdef USE_64BITS_LSEEK
		if(ps3ntfs_seek64(fd, 0xB860LL, SEEK_SET) != 0xB860LL) goto err;
		#else
		if(fseek(fp, 0xB860, SEEK_SET) != 0) goto err;
		#endif

		int c;

		for(int i = 0; i < 8; i++)
		{
			#ifdef USE_64BITS_LSEEK
			if(ps3ntfs_read(fd, (void *) &dir_entry, 0x38) != 0x38) goto err;
			#else
			if(fread((void *) &dir_entry, 1, 0x38, fp) != 0x38) goto err;
			#endif

			for(c = 0; c < dir_entry[0x20]; c++) entry_name[c] = dir_entry[0x21 + c];
			for( ; c < 0x20; c++) entry_name[c] = 0;

			if(strstr(path, entry_name))
			{
				file_lba = isonum_731(&dir_entry[2]);
				*flba = file_lba;

				*size = isonum_731(&dir_entry[10]);

				#ifdef USE_64BITS_LSEEK
				if(ps3ntfs_seek64(fd, ((s64) file_lba) * SECTOR_SIZELL, SEEK_SET) != ((s64) file_lba) * SECTOR_SIZELL) goto err;
				#else
				if(fseek(fp, file_lba * SECTOR_SIZE, SEEK_SET) != 0) goto err;
				#endif

				return SUCCESS;
			}
		}

		return -4;
	}

	u32 lba0 = isonum_731(&sect_descriptor.type_l_path_table[0]); // lba
	if(!lba0) return -3;

	u32 size0 = isonum_733(&sect_descriptor.path_table_size[0]); // size
	if(!size0) return -3;

	int size1 = ((size0 + SECTOR_FILL) / SECTOR_SIZE) * SECTOR_SIZE;
	if(size1 < 0 || size1 > 0x400000) return -3; // size larger than 4MB

	sectors = malloc(size1 + SECTOR_SIZE);
	if(!sectors) return -3;

	memset(sectors, 0, size1 + SECTOR_SIZE);

	#ifdef USE_64BITS_LSEEK
	if(ps3ntfs_seek64(fd, ((s64) lba0) * SECTOR_SIZELL, SEEK_SET) != ((s64) lba0) * SECTOR_SIZELL) goto err;
	if(ps3ntfs_read(fd, (void *) sectors, size1) != size1) goto err;
	#else
	if(fseek(fp, lba0 * SECTOR_SIZE, SEEK_SET) != 0) goto err;
	if(fread((void *) sectors, 1, size1, fp) != size1) goto err;
	#endif

	u32 p = 0;

	u32 lba_folder = 0xffffffff;
	u32 lba  = 0xffffffff;

	int nsplit = 0;
	int last_parent = 1;
	int cur_parent = 1;

	while(p < size0)
	{
		if(nsplit >= nfolder_split) break;
		if(folder_split[nsplit][0] == 0 && nsplit != 0) {lba_folder = lba; break;}
		if(folder_split[nsplit][2] == 0) continue;

		u32 snamelen = isonum_721(&sectors[p]);
		if(snamelen == 0) p = ((p / SECTOR_SIZE) * SECTOR_SIZE) + SECTOR_SIZE; //break;
		p += 2;
		lba = isonum_731(&sectors[p]);
		p += 4;
		u32 parent_name = isonum_721(&sectors[p]);
		p += 2;

		memset(wstring, 0, 512 * 2);
		memcpy(wstring, &sectors[p], snamelen);

		UTF16_to_UTF8(wstring, (u8 *) temp_string);

		if(cur_parent == 1 && folder_split[nsplit][0] == 0 && nsplit == 0) {lba_folder = lba; break;}

		if(last_parent == parent_name && strlen(temp_string) == folder_split[nsplit][2] &&
			!strncmp((void *) temp_string, &path[folder_split[nsplit][1]], folder_split[nsplit][2]))
		{

			//DPrintf("p: %s %u %u\n", &path[folder_split[nsplit][1]], folder_split[nsplit][2], snamelen);
			last_parent = cur_parent;

			nsplit++;
			if(folder_split[nsplit][0] == 0) {lba_folder = lba; break;}
		}

		p += snamelen;
		cur_parent++;
		if(snamelen & 1) {p++;}
	}

	if(lba_folder == 0xffffffff) goto err;

	memset(sectors, 0, _4KB_);

	#ifdef USE_64BITS_LSEEK
	if(ps3ntfs_seek64(fd, ((s64) lba_folder) * SECTOR_SIZELL, SEEK_SET) != ((s64) lba_folder) * SECTOR_SIZELL) goto err;
	if(ps3ntfs_read(fd, (void *) sectors, SECTOR_SIZE) != SECTOR_SIZE) goto err;
	#else
	if(fseek(fp, lba_folder * SECTOR_SIZE, SEEK_SET) != 0) goto err;
	if(fread((void *) sectors, 1, SECTOR_SIZE, fp) != SECTOR_SIZE) goto err;
	#endif

	int size_directory = -1;
	int p2 = 0;

	p = 0;
	while(true)
	{
		if(nsplit >= nfolder_split) break;
		idr = (struct iso_directory_record *) &sectors[p];

		if(size_directory == -1)
		{
			if((int) idr->name_len[0] == 1 && idr->name[0] == 0 && lba == isonum_731((void *) idr->extent) && idr->flags[0] == 0x2)
			{
				size_directory = isonum_733((void *) idr->size);
			}
		}

		if(idr->length[0] == 0 && sizeof(struct iso_directory_record) + p > SECTOR_SIZE)
		{
			lba_folder++;

			#ifdef USE_64BITS_LSEEK
			if(ps3ntfs_seek64(fd, ((s64) lba_folder) * SECTOR_SIZELL, SEEK_SET) != ((s64) lba_folder) * SECTOR_SIZELL) goto err;
			if(ps3ntfs_read(fd, (void *) sectors, SECTOR_SIZE) != SECTOR_SIZE) goto err;
			#else
			if(fseek(fp, lba_folder * SECTOR_SIZE, SEEK_SET) != 0) goto err;
			if(fread((void *) sectors, 1, SECTOR_SIZE, fp) != SECTOR_SIZE) goto err;
			#endif

			p = 0; p2 = (p2 & ~SECTOR_FILL) + SECTOR_SIZE;

			idr = (struct iso_directory_record *) &sectors[p];
			if((int) idr->length[0] == 0) break;
			if((size_directory == -1 && idr->length[0] == 0) || (size_directory != -1 && p2 >= size_directory)) break;
			continue;
		}

		if((size_directory == -1 && idr->length[0] == 0) || (size_directory != -1 && p2 >= size_directory)) break;

		if((int) idr->length[0] == 0) break;

		memset(wstring, 0, 512 * 2);
		memcpy(wstring, (char *) idr->name, idr->name_len[0]);

		UTF16_to_UTF8(wstring, (u8 *) temp_string);

		if( strlen(temp_string) == folder_split[nsplit][2] &&
			!strncmp((char *) temp_string, &path[folder_split[nsplit][1]], (int) folder_split[nsplit][2]) )
		{
			if(file_lba == 0xffffffff) file_lba = isonum_733(&idr->extent[0]);

			*size+= (u64) (u32) isonum_733(&idr->size[0]);

		} else if(file_lba != 0xffffffff) break;

		p += idr->length[0]; p2 += idr->length[0];
	}

	*flba = file_lba;

	if(file_lba == 0xffffffff) goto err;

	#ifdef USE_64BITS_LSEEK
	if(ps3ntfs_seek64(fd, ((s64) file_lba) * SECTOR_SIZELL, SEEK_SET) != ((s64) file_lba) * SECTOR_SIZELL) goto err;
	#else
	if(fseek(fp, file_lba * SECTOR_SIZE, SEEK_SET) != 0) goto err;
	#endif

	if(sectors) free(sectors);

	return SUCCESS;

err:
	if(sectors) free(sectors);

	return -4;
}

int SaveFile(char *path, char *mem, int file_size);

int ExtractFileFromISO(char *iso_file, char *file, char *outfile)
{
	int fd = ps3ntfs_open(iso_file, O_RDONLY, 0);
	if(fd >= 0)
	{
		u32 flba;
		u64 size;
		char *mem = NULL;

		int re = get_iso_file_pos(fd, file, &flba, &size);

		if(!re && (mem = malloc(size)) != NULL)
		{
			re = ps3ntfs_read(fd, (void *) mem, size);
			ps3ntfs_close(fd);
			if(re == size)
				SaveFile(outfile, mem, size);

			free(mem);

			return (re == size) ? SUCCESS : FAILED;
		}
		else
			ps3ntfs_close(fd);
	}

	return FAILED;
}
