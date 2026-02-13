#include "rwlock.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

int shared_data = 0;  // Разделяемые данные для чтения/записи
RWLock* rw;           // Указатель на RWLock (создается после выбора режима)

// ------------------ Функция reader ------------------
// Функция читателя: читает данные из shared_data
// Читатели могут работать параллельно, если нет писателей
void reader(int id) {
    for (int i = 0; i < 5; ++i) {
        rw->lock_read();  // Захват блокировки на чтение
        cout << "Читатель " << id << " читает " << shared_data << "\n";
        rw->unlock_read();  // Освобождение блокировки

        // Имитация сна читателя
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

// ------------------ Функция writer ------------------
// Функция писателя: изменяет данные в shared_data
// Писатель работает в эксклюзивном режиме
void writer(int id) {
    for (int i = 0; i < 5; ++i) {
        rw->lock_write();  // Захват блокировки на запись
        shared_data++;     // Изменение данных
        cout << "Писатель " << id << " пишет " << shared_data << "\n";
        rw->unlock_write(); // Освобождение блокировки

        // Имитация сна писателя 
        this_thread::sleep_for(chrono::milliseconds(150));
    }
}

int main() {
    setlocale(LC_ALL, "Ru");  
    int choice;  

    cout << "Выберите режим работы RWLock:\n";
    cout << "1 — Приоритет писателей\n";
    cout << "2 — Приоритет читателей\n";
    cout << "Ваш выбор: ";
    cin >> choice;

    while (choice != 1 && choice != 2) {
        cout << "Ошибка! Введите 1 или 2: ";
        cin >> choice;
    }

    // Создание RWLock с выбранным приоритетом
    if (choice == 1) {
        rw = new RWLock(Priority::Writers);
        cout << "Выбран режим: приоритет писателей\n\n";
    }
    else {
        rw = new RWLock(Priority::Readers);
        cout << "Выбран режим: приоритет читателей\n\n";
    }

    vector<thread> threads;

    // Запуск 3 читателей
    for (int i = 0; i < 3; ++i)
        threads.emplace_back(reader, i);

    // Запуск 2 писателей
    for (int i = 0; i < 2; ++i)
        threads.emplace_back(writer, i);

    // Ожидание завершения всех потоков
    for (auto& t : threads)
        t.join();

    delete rw;  // Освобождение памяти, выделенной под RWLock
}