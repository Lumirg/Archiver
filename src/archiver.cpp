#include "archiver.h"

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Использование: " << argv[0] << " [-h]\n";
        return BAD_END;
    }
    std::string option = argv[1];
    std::string archive_name = argv[2];

    if (option == "-c") {
        std::ofstream ofs(archive_name, std::ios::binary);
        for (int i = 3; i < argc; i++) {
            std::cerr << argv[i] << "\n";
            CompressFile(argv[i], archive_name, argc - i - 1, ofs);
        }
        ofs.close();
        std::cout << "Сжатие завершено: " << archive_name << "\n";
    } else if (option == "-d") {
        std::string outputfile;
        std::cout << "Распаковка завершена: " << "\n";
        std::string buffer;
        std::ifstream ifs(archive_name, std::ios::binary);
        while (DecompressFile(ifs, outputfile)) {
            std::cout << outputfile << "\n";
            outputfile.clear();
        }
        std::cout << outputfile << "\n";
        ifs.close();
    } else if (option == "-h") {
        std::cout << "Используйте " << argv[0] << " [-c] " << "<archive_name> " << "file1 file2 ... где filei,"
                  << " это i файл который хотите разархивировать" << "\n";
        std::cout << "Используйте " << argv[0] << " [-d] " << "<archive_name> " << "чтоб разархивировать файл" << "\n";
    } else {
        std::cerr << "Неверный параметр: " << option << ". Используйте -h для справки\n";
        return BAD_END;
    }
    return 0;
}
