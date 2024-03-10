# To be sourced

./$s/$BIN64 -v &> $s/verbose_output.txt
sed -ri "s/[0-9]+ ms//g" $s/verbose_output.txt
cmp $s/expected_verbose_output.txt $s/verbose_output.txt
if [ $? -ne 0 ]; then
	exit 1
fi
