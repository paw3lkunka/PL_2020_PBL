#!sh

_className=$1
_file="$(dirname "$0")/project/GameSystems/Components/$1.inl"
touch $_file

echo -n > $_file
echo -e "#ifndef "$_className"_inl_sh
#define "$_className"_inl_sh

struct "$_className"
{
    //fields
};
#endif" >> $_file
