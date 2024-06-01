import sys

def convert_to_header(file_path, output_path):
    with open(file_path, 'rb') as f:
        byte_array = f.read()
    
    with open(output_path, 'w') as f:
        f.write('#include <stdint.h>\n\n')
        f.write('const uint8_t mp3_data[] = {\n')

        for i, byte in enumerate(byte_array):
            if i % 12 == 0:
                f.write('\n')
            f.write(f'0x{byte:02X}, ')

        f.write('\n};\n')
        f.write(f'const size_t mp3_data_len = {len(byte_array)};\n')

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Usage: python convert_to_header.py <input_mp3_file> <output_header_file>')
    else:
        convert_to_header(sys.argv[1], sys.argv[2])
