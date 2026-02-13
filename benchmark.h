#ifndef BENCHMARK_H 
#define BENCHMARK_H

#include <algorithm>           
#include <atomic>              
#include <barrier>             
#include <chrono>              
#include <condition_variable>  
#include <functional>          
#include <iostream>           
#include <mutex>              
#include <numeric>             
#include <random>             
#include <semaphore>          
#include <string>              
#include <thread>              
#include <vector>             

using namespace std;  

// ------------------ Stopwatch для измерения времени ------------------
// Обертка над std::chrono для удобного измерения интервалов времени
struct Stopwatch {
    using clock = chrono::high_resolution_clock;  // Высокоточные часы
    clock::time_point start;                      // Точка начала измерения

    void Start() { start = clock::now(); }        // Запуск таймера

    // Возвращает прошедшее время в микросекундах
    long ElapsedMicro() const {
        return chrono::duration_cast<chrono::microseconds>(clock::now() - start).count();
    }

    // Возвращает прошедшее время в миллисекундах
    long ElapsedMilli() const {
        return chrono::duration_cast<chrono::milliseconds>(clock::now() - start).count();
    }
};

// ------------------ Структура Stats для статистических данных ------------------
struct Stats {
    double mean = 0;    // Среднее арифметическое
    double median = 0;  // Медиана (среднее значение)
    double stddev = 0;  // Стандартное отклонение
    long min = 0;       // Минимальное значение
    long max = 0;       // Максимальное значение
};

// Объявления функций (реализация в benchmark.cpp)
Stats compute_stats(vector<long>& samples);  // Вычисление статистики по выборке
char random_char();                          // Генерация случайного символа

// Объявления функций тестирования механизмов синхронизации
void run_mutex(int threads, int iterations);      
void run_semaphore(int threads, int iterations);  
void run_barrier(int threads, int iterations);    
void run_spinlock(int threads, int iterations);   
void run_spinwait(int threads, int iterations); 
void run_monitor(int threads, int iterations);   

// ------------------ Функция Benchmark для запуска тестов ------------------
// Универсальная функция для проведения бенчмарков с прогревочными прогонами
void Benchmark(const string& name,                  // Название теста
    const function<void()>& action,                 // Тестируемая функция
    int warmup_runs = 3,                           // Количество прогонов прогрева
    int measured_runs = 10);                       // Количество измерительных прогонов

#endif  