#include "worker_processing.h"

int main() {
    size_t n;           // Размер массива работников
    int thread_count;   // Количество потоков для многопоточной обработки

    // Ввод параметров от пользователя
    cout << "Array size: ";
    cin >> n;
    cout << "Threads: ";
    cin >> thread_count;

    // Генерация тестовых данных
    auto data = generate_data(n);
    string D = "D";  // Искомая должность для анализа

    // Однопоточное выполнение
    auto t1_start = Clock::now();  // Засекаем время начала
    auto [avg1, max1] = process_single(data, D);  // Выполняем обработку
    auto t1_end = Clock::now();    // Засекаем время окончания

    // Многопоточное выполнение
    auto t2_start = Clock::now();
    auto [avg2, max2] = process_multi(data, D, thread_count);
    auto t2_end = Clock::now();

    // Расчет времени выполнения в миллисекундах
    auto ms1 = chrono::duration_cast<chrono::milliseconds>(t1_end - t1_start).count();
    auto ms2 = chrono::duration_cast<chrono::milliseconds>(t2_end - t2_start).count();

    // Вывод результатов сравнения
    cout << "Single-thread: " << ms1 << " ms\n";
    cout << "Multi-thread:  " << ms2 << " ms\n";
    cout << "Avg age (single): " << avg1 << ", max salary: " << max1 << "\n";
    cout << "Avg age (multi):  " << avg2 << ", max salary: " << max2 << "\n";
}