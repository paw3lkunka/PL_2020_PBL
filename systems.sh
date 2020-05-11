#!/bin/sh

_path="$(dirname "$0")/project/GameSystems/Systems/include"

_file1="$(dirname "$0")/project/Autogen/include/Systems.inc"
_guardian1="SYSTEMS_INC_AUTOGEN"

_file2="$(dirname "$0")/project/Autogen/include/SystemsPreDeclarations.hxx"
_guardian2="SYSTEMS_PRE_DECL__INC_AUTOGEN"

touch $_file1 $_file2
> $_file1
> $_file2

echo "// ? ╔════════════════════════════╗" | tee -a $_file1 >> $_file2
echo "// ? ║ Headers of all systems     ║" >> $_file1
echo "// ? ║ Pre-decl of all components.║" >> $_file2
echo "// ? ║ Generated by systems.sh    ║" | tee -a $_file1 >> $_file2
echo "// ? ╚════════════════════════════╝" | tee -a $_file1 >> $_file2
echo "#ifndef $_guardian1" >> $_file1
echo "#define $_guardian1" >> $_file1
echo "#ifndef $_guardian2" >> $_file2
echo "#define $_guardian2" >> $_file2
echo >> $_file1
echo "#include \"System.hpp\"" >> $_file1
echo | tee -a $_file1 >> $_file2

for _header in *
do
    echo "    #include \"$_header\"" >> $_file1
    _class=$(echo $_header | cut -f1 -d'.')
    echo "class $_class;" >> $_file2
done

echo | tee -a $_file1 >> $_file2
echo "#endif /* !$_guardian1 */" >> $_file1
echo "#endif /* !$_guardian2 */" >> $_file2
echo "File: $_path/Components.inc generated."