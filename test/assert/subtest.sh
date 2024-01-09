# To be sourced

./$s/$BIN64 -p test_pass
if [ $? -ne 0 ]; then
	exit 1
fi

./$s/$BIN64 -p test_fail
if [ $? -eq 0 ]; then
	exit 1
fi

./$s/$BIN64 -p test_skip
if [ $? -ne 0 ]; then
	exit 1
fi
