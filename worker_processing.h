#ifndef WORKER_PROCESSING_H  // Защита от повторного включения
#define WORKER_PROCESSING_H

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <random>
#include <chrono>
#include <mutex>
#include <cmath>  

using namespace std;

// ------------------ Структура Worker ------------------
// Представляет данные о работнике для обработки
struct Worker {
    string fio;        // Фамилия Имя Отчество (в данном случае генерируется)
    string position;   // Должность (A, B, C, D)
    int age;           // Возраст работника
    double salary;     // Зарплата работника
};

// Псевдоним для удобного использования высокоточных часов
using Clock = chrono::high_resolution_clock;

// Объявления функций
vector<Worker> generate_data(size_t n);  // Генерация тестовых данных
pair<double, double> process_single(const vector<Worker>& v, const string& D);  // Однопоточная обработка
pair<double, double> process_multi(const vector<Worker>& v, const string& D, int thread_count);  // Многопоточная обработка

#endif  // Конец защиты от повторного включения