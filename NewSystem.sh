#!sh

_folderName=$1
_className=$2

_folder="$(dirname "$0")/project/GameSystems/Systems/$_folderName"

mkdir $_folder
mkdir $_folder/src
mkdir $_folder/include

_fileCXX="$_folder/src/$_className.cpp"
_fileHXX="$_folder/include/$_className.hpp"

touch $_fileCXX
touch $_fileHXX

./Systems.sh

echo -n > $_fileHXX
echo -e "#ifndef "$_className"_hpp_sh
#define "$_className"_hpp_sh

#include \"System.hpp\"

//TODO documentation
class "$_className" : public System
{
public:
\t"$_className"() = default;

protected:
\tvirtual ~"$_className"() = default;

\tvirtual bool assertEntity(Entity* entity);
\tvirtual void start();
\tvirtual void fixedUpdate();
\tvirtual void frameUpdate();

private:

};

#endif" >> $_fileHXX

echo -n > $_fileCXX
echo -e "#include \"ECS.inc\"

bool "$_className"::assertEntity(Entity* entity)
{
\treturn false;
}

void "$_className"::start()
{
\t
}
void "$_className"::fixedUpdate()
{
\t
}

void "$_className"::frameUpdate()
{
\t
}" >> $_fileCXX


