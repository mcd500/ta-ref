static int buf_flag = 0;
static char *buf;

static void tee_init() {
    if(buf_flag) {
        buf = malloc(BUF_SIZE);
        if(!buf) TEE_Panic(0);
        buf_flag = 0;
    }
}
