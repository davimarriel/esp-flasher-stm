#!/usr/bin/env python3
import sys

def bin_to_header(bin_filename, header_filename, source_filename, array_name):
    with open(bin_filename, 'rb') as bin_file, \
         open(header_filename, 'w') as header_file, \
         open(source_filename, 'w') as source_file:

        # Escreve no arquivo de cabeçalho (.h)
        header_file.write("#ifndef {}_H\n".format(array_name.upper()))
        header_file.write("#define {}_H\n\n".format(array_name.upper()))
        header_file.write("#include <stdint.h>\n\n")
        header_file.write("extern const uint8_t {}[];\n".format(array_name))
        header_file.write("extern const uint32_t {}_size;\n\n".format(array_name))
        header_file.write("#endif // {}_H\n\n".format(array_name.upper()))

        # Escreve no arquivo de código-fonte (.c)
        source_file.write("#include \"{}\"\n\n".format(header_filename))
        source_file.write("const uint8_t {}[] = {{\n".format(array_name))

        # Escreve os dados do arquivo binário
        byte = bin_file.read(1)
        count = 0
        while byte:
            source_file.write(hex(ord(byte)).rstrip("L") + ", ")
            byte = bin_file.read(1)
            count += 1
            if count % 16 == 0:
                source_file.write("\n")
        source_file.write("};\n\n")

        # Escreve o tamanho do array
        source_file.write("const uint32_t {}_size = {};\n".format(array_name, count))


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Uso: {} arquivo_binario.bin arquivo_header.h arquivo_fonte.c nome_do_array".format(sys.argv[0]))
        sys.exit(1)

    bin_file = sys.argv[1]
    header_file = sys.argv[2]
    source_file = sys.argv[3]
    array_name = sys.argv[4]

    bin_to_header(bin_file, header_file, source_file, array_name)
