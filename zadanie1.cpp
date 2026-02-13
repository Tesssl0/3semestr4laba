#include "benchmark.h"

int main(int argc, char** argv) {
    setlocale(LC_ALL, "Ru");

    // Значения по умолчанию для параметров тестирования
    int threads = 8;           // Количество потоков по умолчанию
    int iterations = 100000;   // Количество итераций в каждом потоке
    int warmup = 3;           // Количество прогонов для прогрева
    int runs = 10;            // Количество измеряемых прогонов

    if (argc >= 2) threads = stoi(argv[1]);        
    if (argc >= 3) iterations = stoi(argv[2]);     
    if (argc >= 4) warmup = stoi(argv[3]);         
    if (argc >= 5) runs = stoi(argv[4]);         

    // Вывод параметров тестирования
    cout << "Потоков: " << threads
        << ", Итераций: " << iterations
        << ", Прогрев: " << warmup
        << ", Прогонов: " << runs << "\n\n";

    // Запуск бенчмарков для различных механизмов синхронизации
    // Каждый бенчмарк принимает лямбда-функцию с тестируемым кодом
    Benchmark("mutex", [&] { run_mutex(threads, iterations); }, warmup, runs);
    Benchmark("semaphore", [&] { run_semaphore(threads, iterations); }, warmup, runs);
    Benchmark("barrier", [&] { run_barrier(threads, iterations); }, warmup, runs);
    Benchmark("spinlock", [&] { run_spinlock(threads, iterations); }, warmup, runs);
    Benchmark("spinwait", [&] { run_spinwait(threads, iterations); }, warmup, runs);
    Benchmark("monitor", [&] { run_monitor(threads, iterations); }, warmup, runs);

    return 0;  // Успешное завершение программы
}
