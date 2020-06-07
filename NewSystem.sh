#!sh

_className=$1
_fileCXX="$(dirname "$0")/project/GameSystems/Systems/src/$1.cpp"
_fileHXX="$(dirname "$0")/project/GameSystems/Systems/include/$1.hpp"
touch $_fileCXX
touch $_fileHXX

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
#include \"Components.inc\"

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


