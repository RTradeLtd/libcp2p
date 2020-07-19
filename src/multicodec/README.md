# regeneration	

run the following to regenerate the codecs.c file

```shell
$> wget -O codecs.csv https://raw.githubusercontent.com/multiformats/multicodec/master/table.csv
$> awk -F ',' '{print "#define "$1}' codecs.csv | tr '[:lower:]' '[:upper:]' | tr '-' '_' | tee > vars.txt # remove first row as it is the identifier
$> awk -F ',' '{print "\x22"$3"\x22"}' codecs.csv | tr -d " \t\r" | tee > values.txt
$> paste -d ' ' vars.txt values.txt > codecs.h
$> sed -i 's/DEFINE/define/g' codecs.h
```

after you'll want to manually edit the file to remove the final line and `0XCERT_IMPRINT_256` declaration as it is not a valid variable name. Additionally you'll want to remove the first line of the file.

# notes

* `RAW` is the IPLD raw format
* `0XCERT_IMPRINT_256` is incompatible as a variable name