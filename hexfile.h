#ifndef __HEXFILE_H__
#define __HEXFILE_H__
int readhexline(FILE *fd, void **data, size_t *len, long *address);

#define HEX_FILE_ERR_CHECKSUM -1	 		/* Invalid checksum */
#define HEX_FILE_CORRUPT_LINE -2			/* Corrupt line, no data read */
#define HEX_FILE_END_OF_FILE  -3			/* End of line */
#define HEX_FILE_UNKNOWN_TYPE -4			/* Unknown type */

#endif /* __HEXFILE_H__ */
