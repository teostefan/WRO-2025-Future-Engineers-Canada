CC = gcc
CFLAGS = -Wall -Wextra -O2 -Isoftware
SRC = software/main.c software/cv/cv.c software/pid/pid.c software/i2cmux/i2cmux.c software/gyro/gyro.c software/esc/esc.c software/tof/tof.c
OUT = builds/main
OUT_STOP = builds/stop
TESTS = builds/test_cv builds/test_tof builds/test_i2cmux builds/test_gyro builds/test_esc builds/test_kf

# Ensure builds directory exists before building.
$(shell mkdir -p builds)

all: $(OUT) $(OUT_STOP)

all-tests: $(OUT) $(OUT_STOP) $(TESTS)

$(OUT_STOP): software/stop.c
	@echo -ne "\033[1;34m[2] - Compiling: \033[0m"
	$(CC) $(CFLAGS) -o $(OUT_STOP) software/stop.c -lm -lpigpio -lrt
	@echo -e "\033[1;32mBuild successful!\033[0m"

$(OUT): $(SRC)
	@echo -ne "\033[1;34m[2] - Compiling: \033[0m"
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) -lm -lpigpio -lrt
	@echo -e "\033[1;32mBuild successful!\033[0m"

builds/test_cv: software/cv/test_cv.c software/cv/cv.c
	@echo -ne "\033[1;34m[3] - Compiling test_cv: \033[0m"
	$(CC) $(CFLAGS) -o builds/test_cv software/cv/test_cv.c software/cv/cv.c
	@echo -e "\033[1;32mtest_cv build successful!\033[0m"

builds/test_esc: software/esc/test_esc.c software/esc/esc.c
	@echo -ne "\033[1;34m[3] - Compiling test_esc: \033[0m"
	$(CC) $(CFLAGS) -o builds/test_esc software/esc/test_esc.c software/esc/esc.c -lpigpio -lrt
	@echo -e "\033[1;32mtest_esc build successful!\033[0m"

builds/test_tof: software/tof/test_tof.c software/tof/tof.c
	@echo -ne "\033[1;34m[3] - Compiling test_tof: \033[0m"
	$(CC) $(CFLAGS) -o builds/test_tof software/tof/test_tof.c software/tof/tof.c
	@echo -e "\033[1;32mtest_tof build successful!\033[0m"

builds/test_gyro: software/gyro/test_gyro.c software/gyro/gyro.c
	@echo -ne "\033[1;34m[3] - Compiling test_gyro: \033[0m"
	$(CC) $(CFLAGS) -o builds/test_gyro software/gyro/test_gyro.c software/gyro/gyro.c -lm
	@echo -e "\033[1;32mtest_gyro build successful!\033[0m"

builds/test_i2cmux: software/i2cmux/test_i2cmux.c software/i2cmux/i2cmux.c
	@echo -ne "\033[1;34m[3] - Compiling test_i2cmux: \033[0m"
	$(CC) $(CFLAGS) -o builds/test_i2cmux software/i2cmux/test_i2cmux.c software/i2cmux/i2cmux.c
	@echo -e "\033[1;32mtest_i2cmux build successful!\033[0m"

clean:
	rm -f $(OUT) $(TESTS)

.FORCE: