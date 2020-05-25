#!sh

_out=$(ls -R "$(dirname "$0")/project" | grep "cpp" | wc -l)

if [ "-s" = "$1" ]; then
    _file="$(dirname "$0")/misc/cpp_files_count"
    touch $_file
    echo $_out > $_file
    echo "Number of .cpp files saved in $_file"
else
    echo $_out
fi