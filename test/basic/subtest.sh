# To be sourced

./$s/$BIN64 -p test_suite1_ok
if [ $? -ne 0 ]; then
	exit 1
fi

./$s/$BIN64 -p test_suite1_nok
if [ $? -eq 0 ]; then
	exit 1
fi

./$s/$BIN64 -p test_suite1_skip
if [ $? -ne 0 ]; then
	exit 1
fi
