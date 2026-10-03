// prakt7_Evsyukov_SP.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

using namespace std;

#define DEBUG_MODE


struct Book {
    string title;     
    string author;    
    int year;     
    int invNumber;  
    bool isAvailable;
};


int stringToInt(const string& str) {
    if (str.empty()) throw invalid_argument("--- Ошибка формата: Обнаружено пустое числовое поле");

    for (int i = 0; i < str.length(); i++) {
        char c = str[i];
        if (!isdigit(c) && c != '-') {
            throw invalid_argument("--- Ошибка формата: Значение '" + str + "' не является числом");
        }
    }
    return stoi(str);
}

int loadLibrary(Book library[], int maxBooks, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("--- Критическая ошибка: Не удалось открыть файл '" + filename + "' для чтения");
    }

    string line;
    int count = 0;

    while (getline(file, line)) {
        if (line.empty()) continue;

        if (count >= maxBooks) {
            file.close();
            throw runtime_error("--- Ошибка: Превышен максимально допустимый размер библиотеки в массиве");
        }

        string parts[5];
        int currentPart = 0;
        string currentWord = "";

        for (int i = 0; i < line.length(); i++) {
            if (line[i] == ';') {
                if (currentPart < 5) {
                    parts[currentPart] = currentWord;
                    currentPart++;
                }
                currentWord = "";
            }
            else {
                currentWord += line[i];
            }
        }

        if (currentPart < 5) {
            parts[currentPart] = currentWord;
        }


        if (currentPart != 4 || parts[0].empty() || parts[1].empty()) {
            file.close();
            throw runtime_error("--- Ошибка структуры файла: Строка содержит недостаточное количество данных");
        }

        int year = stringToInt(parts[2]);
        int inv = stringToInt(parts[3]);
        int av = stringToInt(parts[4]);

        if (year < 0 || year > 2026) throw invalid_argument("--- Ошибка: Некорректный год издания");
        if (inv <= 0) throw invalid_argument("--- Ошибка: Инвентарный номер должен быть больше 0");
        if (av != 0 && av != 1) throw invalid_argument("--- Ошибка: Наличие книги должно быть 0 или 1");

        library[count].title = parts[0];
        library[count].author = parts[1];
        library[count].year = year;
        library[count].invNumber = inv;
        library[count].isAvailable = (av == 1);

        count++;
    }

    file.close();

#ifdef DEBUG_MODE
    cout << "[DEBUG] Успешно загружено записей из файла: " << count << endl;
#endif

    return count;
}

void saveLibrary(const Book library[], int count, const string& filename) {
    fstream file;
    file.open(filename, ios::out | ios::trunc);

    if (!file.is_open()) {
        throw runtime_error("--- Ошибка: Не удалось сохранить изменения. Ошибка открытия файла для записи");
    }

    for (int i = 0; i < count; i++) {
        file << library[i].title << ";"
            << library[i].author << ";"
            << library[i].year << ";"
            << library[i].invNumber << ";"
            << (library[i].isAvailable ? 1 : 0) << "\n";
    }


    file.close();

#ifdef DEBUG_MODE
    cout << "[DEBUG] Успешно сохранено записей в файл: " << count << endl;
#endif
}

int main() {
    setlocale(LC_ALL, "Russian");

    const string filename = "library_list.txt";
    const int MAX_BOOKS = 100;
    Book library[MAX_BOOKS];
    int bookCount = 0;

    try {
        bookCount = loadLibrary(library, MAX_BOOKS, filename);
    }
    catch (const exception& e) {
        cout << e.what() << endl;
        cout << "Программа завершает работу из за ошибки файла" << endl;
        return 1;
    }

    int choice = -1;
    while (choice != 0) {
        cout << "\n\t--- МЕНЮ УЧЁТА КНИГ --- " << endl;
        cout << "1. Поиск книги по названию" << endl;
        cout << "2. Поиск книг определённого автора" << endl;
        cout << "3. Вывод всех доступных книг" << endl;
        cout << "4. Выдать книгу (изменить статус)" << endl;
        cout << "5. Вернуть книгу" << endl;
        cout << "6. Сохранить состояние библиотеки" << endl;
        cout << "0. Выход" << endl;
        cout << "\tВыберите действие: ";
        cin >> choice;

        try {
            //1
            if (choice == 1) {
                string searchTitle;
                cout << "Введите название книги для поиска: ";
                cin.ignore();
                getline(cin, searchTitle);

                bool found = false;
                for (int i = 0; i < bookCount; i++) {
                    if (library[i].title == searchTitle) {
                        cout << "Найдено: " << library[i].title << " | Автор: " << library[i].author << " | Год: " << library[i].year << " | Инв. №: " << library[i].invNumber
                            << " | Доступна: " << (library[i].isAvailable ? "Да" : "Нет") << endl;
                        found = true;
                    }
                }
                if (!found) cout << "Книга с таким названием не найдена" << endl;
            }
            //2
            else if (choice == 2) {
                string searchAuthor;
                cout << "Введите автора: ";
                cin.ignore();
                getline(cin, searchAuthor);

                bool found = false;
                for (int i = 0; i < bookCount; i++) {
                    if (library[i].author == searchAuthor) {
                        cout << "- " << library[i].title << " (" << library[i].year << " г.) [Инв. №: " << library[i].invNumber << "]" << endl;
                        found = true;
                    }
                }
                if (!found) cout << "--- Книг данного автора ("<< searchAuthor << ") не найдено" << endl;
            }
            //3
            else if (choice == 3) {
                cout << "\nДоступные книги в наличии:\n";
                bool anyAvailable = false;
                for (int i = 0; i < bookCount; i++) {
                    if (library[i].isAvailable) {
                        cout << "- " << library[i].title << " (Автор: " << library[i].author << ", Инв. №: " << library[i].invNumber << ")" << endl;
                        anyAvailable = true;
                    }
                }
                if (!anyAvailable) cout << "В данный момент все книги на руках" << endl;
            }
            //4
            else if (choice == 4) {
                int inv;
                cout << "Введите инвентарный номер книги для выдачи: ";
                cin >> inv;

                int foundIndex = -1;
                for (int i = 0; i < bookCount; i++) {
                    if (library[i].invNumber == inv) {
                        foundIndex = i;
                        break;
                    }
                }

                if (foundIndex == -1) {
                    throw invalid_argument("--- Ошибка: Книга с таким инвентарным номером не существует");
                }

                // уже выданная книга
                if (!library[foundIndex].isAvailable) {
                    throw runtime_error("--- Недопустимая операция: Попытка выдать уже выданную книгу");
                }

                library[foundIndex].isAvailable = false;
                cout << "Успешно! Книга '" << library[foundIndex].title << "' выдана читателю" << endl;
            }
            //5
            else if (choice == 5) {
                int inv;
                cout << "Введите инвентарный номер возвращаемой книги: ";
                cin >> inv;

                int foundIndex = -1;
                for (int i = 0; i < bookCount; i++) {
                    if (library[i].invNumber == inv) {
                        foundIndex = i;
                        break;
                    }
                }

                if (foundIndex == -1) {
                    throw invalid_argument("--- Ошибка: Книга с таким номером не числится в базе");
                }

                if (library[foundIndex].isAvailable) {
                    cout << "Эта книга уже находится в библиотеке" << endl;
                }
                else {
                    library[foundIndex].isAvailable = true;
                    cout << "Книга '" << library[foundIndex].title << "' возвращена на полку" << endl;
                }
            }
            //6
            else if (choice == 6) {
                saveLibrary(library, bookCount, filename);
                cout << "Изменения успешно зафиксированы в файле" << endl;
            }
        }
        catch (const exception& e) {
            cout << "\n[ПЕРЕХВАЧЕНО ИСКЛЮЧЕНИЕ]: " << e.what() << endl;
        }
    }
    return 0;
}
