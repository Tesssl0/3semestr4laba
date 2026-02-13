#include "benchmark.h"
#include <cmath>  

// ------------------ Функция compute_stats ------------------
// Вычисляет статистические метрики по выборке временных измерений
Stats compute_stats(vector<long>& samples) {
    Stats s;
    if (samples.empty()) return s;  // Защита от пустой выборки

    // Сортировка выборки для вычисления медианы и экстремумов
    sort(samples.begin(), samples.end());
    s.min = samples.front();  // Первый элемент после сортировки - минимальный
    s.max = samples.back();   // Последний элемент - максимальный

    // Вычисление среднего арифметического
    double sum = accumulate(samples.begin(), samples.end(), 0.0);
    s.mean = sum / samples.size();

    // Вычисление медианы
    size_t n = samples.size();
    if (n % 2 == 1) {
        s.median = samples[n / 2];  // Для нечетного количества - средний элемент
    }
    else {
        // Для четного количества - среднее двух центральных элементов
        s.median = (samples[n / 2 - 1] + samples[n / 2]) / 2.0;
    }

    // Вычисление стандартного отклонения (мера разброса данных)
    double sq = 0;
    for (auto v : samples) {
        double diff = v - s.mean;
        sq += diff * diff;  // Сумма квадратов отклонений
    }
    s.stddev = sqrt(sq / samples.size());  // Квадратный корень из дисперсии

    return s;
}

// ------------------ Функция random_char ------------------
// Генерирует случайный печатный ASCII символ (коды 32-126)
// Используется для имитации работы внутри критической секции
char random_char() {
    // thread_local гарантирует, что каждый поток имеет свой собственный генератор
    static thread_local mt19937 gen(random_device{}());
    static uniform_int_distribution<int> dist(32, 126);  // Диапазон печатных ASCII символов
    return static_cast<char>(dist(gen));
}

// ------------------ Тест 1: Мьютекс ------------------
void run_mutex(int threads, int iterations) {
    mutex m;  // Создание мьютекса
    vector<thread> v;
    v.reserve(threads);  // Резервирование памяти для потоков

    for (int i = 0; i < threads; ++i) {
        v.emplace_back([&] {  // Создание потока с лямбда-функцией
            for (int j = 0; j < iterations; ++j) {
                // lock_guard автоматически захватывает мьютекс при создании
                // и освобождает его при выходе из области видимости
                lock_guard<mutex> lg(m);
                volatile char c = random_char();  // Имитация работы
                (void)c;  // Подавление предупреждения о неиспользуемой переменной
            }
            });
    }
    // Ожидание завершения всех потоков
    for (auto& th : v) th.join();
}

// ------------------ Тест 2: Семафор ------------------
void run_semaphore(int threads, int iterations) {
    counting_semaphore<1> sem(1);  // Бинарный семафор (значение 0 или 1)
    vector<thread> v;
    v.reserve(threads);

    for (int i = 0; i < threads; ++i) {
        v.emplace_back([&] {
            for (int j = 0; j < iterations; ++j) {
                sem.acquire();  // Уменьшение значения семафора (ждать если 0)
                volatile char c = random_char();
                (void)c;
                sem.release();  // Увеличение значения семафора
            }
            });
    }
    for (auto& th : v) th.join();
}

// ------------------ Тест 3: Барьер ------------------
void run_barrier(int threads, int iterations) {
    barrier sync_point(threads);  // Барьер на заданное количество потоков
    vector<thread> v;
    v.reserve(threads);

    for (int i = 0; i < threads; ++i) {
        v.emplace_back([&] {
            for (int j = 0; j < iterations; ++j) {
                volatile char c = random_char();
                (void)c;
                // Поток достигает барьера и ждет остальные
                sync_point.arrive_and_wait();
            }
            });
    }
    for (auto& th : v) th.join();
}

// ------------------ Тест 4: Спинлок ------------------
struct SpinLock {
    atomic_flag flag = ATOMIC_FLAG_INIT;  // Атомарный флаг для спинлока

    void lock() {
        // test_and_set атомарно устанавливает флаг и возвращает предыдущее значение
        // Если флаг уже был установлен, поток зацикливается (spin)
        while (flag.test_and_set(memory_order_acquire)) {
            this_thread::yield();  // Уступаем процессор, чтобы снизить нагрузку на CPU
        }
    }

    void unlock() {
        flag.clear(memory_order_release);  // Сброс флага
    }
};

void run_spinlock(int threads, int iterations) {
    SpinLock sl;  // Создание спинлока
    vector<thread> v;
    v.reserve(threads);

    for (int i = 0; i < threads; ++i) {
        v.emplace_back([&] {
            for (int j = 0; j < iterations; ++j) {
                sl.lock();  // Захват спинлока
                volatile char c = random_char();
                (void)c;
                sl.unlock();  // Освобождение спинлока
            }
            });
    }
    for (auto& th : v) th.join();
}

// ------------------ Тест 5: Активное ожидание (Spin-wait) ------------------
void run_spinwait(int threads, int iterations) {
    atomic<int> dummy{ 0 };  // Атомарная переменная для имитации работы
    vector<thread> v;
    v.reserve(threads);

    for (int i = 0; i < threads; ++i) {
        v.emplace_back([&] {
            for (int j = 0; j < iterations; ++j) {
                // Активная работа: 100 атомарных инкрементов
                for (int k = 0; k < 100; ++k) {
                    dummy.fetch_add(1, memory_order_relaxed);  // Релаксированная атомарность
                }
                volatile char c = random_char();
                (void)c;
            }
            });
    }
    for (auto& th : v) th.join();
}

// ------------------ Тест 6: Монитор ------------------
void run_monitor(int threads, int iterations) {
    mutex m;
    condition_variable cv;  // Условная переменная для ожидания
    bool can_enter = true;  // Флаг, разрешающий вход в критическую секцию

    vector<thread> v;
    v.reserve(threads);

    for (int i = 0; i < threads; ++i) {
        v.emplace_back([&] {
            for (int j = 0; j < iterations; ++j) {
                unique_lock<mutex> lk(m);  // unique_lock для работы с condition_variable

                // Ожидание, пока флаг can_enter не станет true
                cv.wait(lk, [&] { return can_enter; });

                // Вход в критическую секцию
                can_enter = false;  // Запрещаем вход другим потокам
                volatile char c = random_char();
                (void)c;

                // Выход из критической секции
                can_enter = true;  // Разрешаем вход следующему потоку
                lk.unlock();       // Освобождаем мьютекс перед уведомлением
                cv.notify_one();   // Будим один ожидающий поток
            }
            });
    }
    for (auto& th : v) th.join();
}

// ------------------ Функция Benchmark ------------------
// Универсальная функция для проведения измерений производительности
void Benchmark(const string& name,
    const function<void()>& action,
    int warmup_runs,
    int measured_runs) {

    // Фаза прогрева: выполнение теста несколько раз без измерений
    // Позволяет "прогреть" кэш процессора и стабилизировать работу планировщика
    for (int i = 0; i < warmup_runs; ++i) action();

    // Вектор для хранения результатов измерений
    vector<long> samples;
    samples.reserve(measured_runs);

    // Основная фаза измерений
    for (int i = 0; i < measured_runs; ++i) {
        Stopwatch sw;
        sw.Start();        // Запуск таймера
        action();          // Выполнение тестируемой функции
        samples.push_back(sw.ElapsedMicro());  // Сохранение времени выполнения

        // Небольшая пауза между прогонами для стабилизации системы
        this_thread::sleep_for(chrono::milliseconds(50));
    }

    // Вычисление статистики по собранным данным
    Stats s = compute_stats(samples);

    // Вывод результатов бенчмарка
    cout << "Бенчмарк: " << name << "\n";
    cout << "  Прогрев: " << warmup_runs << ", Прогонов: " << measured_runs << "\n";
    cout << "  Среднее (мкс):   " << s.mean << "\n";
    cout << "  Медиана (мкс):   " << s.median << "\n";
    cout << "  Стд. отклонение: " << s.stddev << "\n";
    cout << "  Минимум (мкс):   " << s.min << "\n";
    cout << "  Максимум (мкс):  " << s.max << "\n";
    cout << "  Выборка:";
    for (auto v : samples) cout << " " << v;  // Вывод всех измеренных значений
    cout << "\n\n";
}