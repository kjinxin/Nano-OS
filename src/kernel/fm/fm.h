
#define FILE_READ 0
#define FILE_WRITE 1

#define NR_FILE_SIZE (128 * 1024)

void init_fm(void);
void do_read(int file_name, uint8_t *buf, off_t offset, size_t len);
void do_write(int file_name, uint8_t *buf, off_t offset, size_t len); 

