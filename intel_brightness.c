const char *fn_current = "/sys/class/backlight/intel_backlight/brightness";
const char *fn_max = "/sys/class/backlight/intel_backlight/max_brightness";

void* syscall(
    void* syscall_number,
    void* param1,
    void* param2,
    void* param3,
    void* param4,
    void* param5
);

typedef unsigned long int uintptr; /* size_t */
typedef long int intptr; /* ssize_t */
typedef unsigned short umode_t;
#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR		00000002

const int minval = 0;

static
intptr open(void const* filename, uintptr flags, uintptr mode)
{
    return (intptr)
        syscall(
            (void*)2, /* SYS_open */
            (void*)filename,
            (void*)flags,
            (void*)mode,
            0, /* ignored */
            0  /* ignored */
        );
}

static
intptr close(unsigned int fd)
{
    return (intptr)
        syscall(
            (void*)3,
            (void*)(intptr)fd,
            0,
            0,
            0, /* ignored */
            0  /* ignored */
        );
}

static
intptr read(int fd, void const* buf, uintptr nbytes)
{
    return (intptr)
        syscall(
            (void*)0,
            (void*)(intptr)fd,
            (void*)buf,
            (void*)nbytes,
            0, /* ignored */
            0  /* ignored */
        );
}

static
intptr write(int fd, void const* data, uintptr nbytes)
{
    return (intptr)
        syscall(
            (void*)1, /* SYS_write */
            (void*)(intptr)fd,
            (void*)data,
            (void*)nbytes,
            0, /* ignored */
            0  /* ignored */
        );
}

#define BUFLEN 16

char buf[BUFLEN];

int atoi(char * buf)
{
	int i = 0;
	int pow = 1;
	int result = 0;
	for (; i < BUFLEN; ++i) {
		if (!(buf[i] >= '0' && buf[i] <= '9')) {
			break;
		}
	}
	--i;
	while (i >= 0) {
		//write(1, buf+i, 1);
		result += (buf[i] - '0')*pow;
		pow *= 10;
		--i;
	}
	return result;
}

void reverse(char * buf, int right) {
	int left = -1;
	while (++left < --right) {
		buf[left]  ^= buf[right];
		buf[right] ^= buf[left];
		buf[left]  ^= buf[right];
	}
}

int itoa(char * buf, unsigned int n) {
	int digit;
	int i = 0;
	for (; i < BUFLEN; ++i) {
		digit = n % 10;
		buf[i] = '0' + digit;
		n /= 10;
		if (n == 0)
			break;
	}

	++i;
	buf[i] = 0;

	reverse(buf, i);

	return i;
}

int read_file(const char *filename) {
	int fd = open(filename, O_RDONLY, 0);
	read(fd, buf, BUFLEN);
	close(fd);
	return atoi(buf);
}

void write_file(const char *filename, const char *buf, int len) {
	int fd = open(filename, O_RDWR, 0);
	write(fd, buf, BUFLEN);
	close(fd);
}

int check_bounds(int val, int min, int max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		return 1;
	}
	int cmd = argv[1][0];
	if (cmd != '+' && cmd != '-' && cmd != '=') {
		return 2;
	}
	int maxval = read_file(fn_max);
	int current = read_file(fn_current);
	int arg = atoi(argv[1]+1);
	int result;
	switch (cmd) {
		case '+':
			result = check_bounds(current + arg, minval, maxval);
			break;

		case '-':
			result = check_bounds(current - arg, minval, maxval);
			break;

		case '=':
			result = check_bounds(arg, minval, maxval);
			break;

		default:
			return 2;
	}
	int len = itoa(buf, result);
	if (len < BUFLEN) {
		// append newline
		buf[len++] = '\n';
	}
	write(1, buf, len);
	write_file(fn_current, buf, len);
	return len;
}
