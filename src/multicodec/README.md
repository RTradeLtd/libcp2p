# regeneration	

run the following to regenerate the codecs.c file

```shell
$> wget -O codecs.csv https://raw.githubusercontent.com/multiformats/multicodec/master/table.csv
$> awk -F ',' '{print "#define "$1}' codecs.csv | tr '[:lower:]' '[:upper:]' | tr '-' '_' | tee > vars.txt # remove first row as it is the identifier
$> awk -F ',' '{print "\x22"$3"\x22"}' codecs.csv | tr -d " \t\r" | tee > values.txt
$> paste -d ' ' vars.txt values.txt > codecs.c
$> sed -i 's/DEFINE/define/g' codecs.c
```

after you'll want to manually edit the file to remove the final line and `0XCERT_IMPRINT_256`