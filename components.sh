#!/bin/sh

_path="$(dirname "$0")/project/GameSystems/Components"

_file1="$(dirname "$0")/project/Autogen/include/Components.inc"
_guardian1="COMPONENTS_INC_AUTOGEN"

_file2="$(dirname "$0")/project/Autogen/include/ComponentsPreDeclarations.hxx"
_guardian2="COMPONENTS_PRE_DECL_INC_AUTOGEN"

touch $_file1 $_file2
cd $_path
> $_file1
> $_file2

echo "// ? ╔════════════════════════════╗" | tee -a $_file1 >> $_file2
echo "// ? ║ Headers of all components. ║" >> $_file1
echo "// ? ║ Pre-decl of all components.║" >> $_file2
echo "// ? ║ Generated by components.sh ║" | tee -a $_file1 >> $_file2
echo "// ? ╚════════════════════════════╝" | tee -a $_file1 >> $_file2
echo "#ifndef $_guardian1" >> $_file1
echo "#define $_guardian1" >> $_file1
echo "#ifndef $_guardian2" >> $_file2
echo "#define $_guardian2" >> $_file2
echo >> $_file1
echo "#include \"Component.inl\"" >> $_file1
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