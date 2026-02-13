#ifndef RWLOCK_H 
#define RWLOCK_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

// Определяет приоритет в работе с разделяемыми данными
enum class Priority {
    Writers,  // Приоритет писателей (писатели не ждут читателей)
    Readers   // Приоритет читателей (читатели не ждут писателей)
};

class RWLock {
    mutex m;                    // Основной мьютекс для защиты внутренних переменных
    condition_variable cv;      // Условная переменная для ожидания

    // Состояние блокировки
    int readers = 0;            // Количество активных читателей
    int writers = 0;            // Количество активных писателей (0 или 1)
    int waiting_readers = 0;    // Количество читателей, ожидающих доступа
    int waiting_writers = 0;    // Количество писателей, ожидающих доступа

    Priority priority;          // Выбранный режим приоритета

public:
    RWLock(Priority p);         // Конструктор с выбором приоритета
    void lock_read();           // Блокировка для чтения
    void unlock_read();         // Разблокировка чтения
    void lock_write();          // Блокировка для записи
    void unlock_write();        // Разблокировка записи
};

#endif 