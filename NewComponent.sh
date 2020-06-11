#!sh

_className=$1
_file="$(dirname "$0")/project/GameSystems/Components/$1.inl"
touch $_file

./Components.sh

echo -n > $_file
echo -e "#ifndef "$_className"_inl_sh
#define "$_className"_inl_sh

#include \"Component.hpp\"

//TODO documentation
struct "$_className" : public Component
{
    //fields
};
#endif" >> $_file
