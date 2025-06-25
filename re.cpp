#include <iostream>
#include <bitset>
#include <string>
#include <cstdint>
#include <termios.h>
#include <unistd.h>
#include <algorithm>

void enable_raw_mode(termios &orig_term) {
    tcgetattr(STDIN_FILENO, &orig_term);
    termios raw = orig_term;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void disable_raw_mode(const termios &orig_term) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
}

std::string get_bits_linux_live(int max_len = 64) {
    std::string bits;
    std::cout << "Введите битовую строку (только 0 и 1, максимум " << max_len << " бит):\n";
    std::cout << "Backspace — удалить символ, Enter — завершить ввод.\n";
    std::cout.flush();

    termios orig_term;
    enable_raw_mode(orig_term);

    while (true) {
        char ch = 0;
        ssize_t res = read(STDIN_FILENO, &ch, 1);
        if (res <= 0) continue;
        if (ch == '\n' || ch == '\r') {
            if (!bits.empty()) {
                std::cout << std::endl;
                break;
            }
        } else if ((ch == 127 || ch == 8) && !bits.empty()) { // Backspace
            bits.pop_back();
        } else if ((ch == '0' || ch == '1') && (int)bits.size() < max_len) {
            bits += ch;
        }
        // Счётчик и отображение
        std::cout << "\r" << bits;
        std::cout << std::string(max_len - bits.size(), ' ') << "  [" << bits.size() << "/" << max_len << "]";
        std::cout.flush();
    }
    disable_raw_mode(orig_term);
    std::cout << std::endl;
    return bits;
}

std::bitset<64> input_bitset64_linux_live() {
    while (true) {
        std::string bits = get_bits_linux_live(64);
        if (bits.empty()) {
            std::cout << "Пустой ввод! Попробуйте снова.\n";
            continue;
        }
        if (bits.size() > 64) {
            std::cout << "Введено больше 64 бит! Повторите ввод.\n";
            continue;
        }
        if (bits.size() < 64) {
            std::cout << "Введено " << bits.size() << " бит(а/ов). Как поступить?\n";
            std::cout << "1) Дополнить нулями до 64 (в конец)\n";
            std::cout << "2) Ввести недостающие биты вручную\n";
            std::cout << "Выберите 1 или 2: ";
            int choice = 0;
            std::cin >> choice;
            std::cin.ignore(1000, '\n');
            if (choice == 1) {
                bits.append(64 - bits.size(), '0');
                std::cout << "Вектор дополнен нулями до 64 бит: " << bits << "\n";
            } else {
                while (bits.size() < 64) {
                    std::cout << "Осталось " << (64 - bits.size()) << " бит(а/ов). Введите ещё: ";
                    std::string extra = get_bits_linux_live(64 - bits.size());
                    bits += extra;
                    std::cout << "Вы ввели всего " << bits.size() << " бит(а/ов).\n";
                }
                std::cout << "Итоговый вектор: " << bits << "\n";
            }
        }
        return std::bitset<64>(bits);
    }
}

uint64_t comb(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;
    uint64_t res = 1;
    for (int i = 1; i <= k; ++i) {
        res = res * (n - i + 1) / i;
    }
    return res;
}

uint64_t count_vectors_in_sphere(const std::bitset<64>& x, int r, int w, bool verbose = true) {
    int n = 64;
    int s = static_cast<int>(x.count());
    uint64_t total = 0;
    if (verbose) {
        std::cout << "\nАнализ диапазонов для перебора d (расстояния):\n";
    }
    for (int d = 0; d <= r; ++d) {
        int k_numer = s + d - w;
        if (verbose) {
            std::cout << "d = " << d << ": ";
        }
        if (k_numer % 2 != 0) {
            if (verbose) std::cout << "k не целое\n";
            continue;
        }
        int k = k_numer / 2;
        int min_k = std::max(0, d - (n - s));
        int max_k = std::min(d, s);
        if (verbose) {
            std::cout << "k = (s + d - w)/2 = (" << s << " + " << d << " - " << w << ")/2 = " << k;
            std::cout << " | Диапазон: min_k = " << min_k << ", max_k = " << max_k << ". ";
        }
        if (k < min_k || k > max_k) {
            if (verbose) std::cout << "k вне диапазона\n";
            continue;
        }
        uint64_t ways = comb(s, k) * comb(n - s, d - k);
        total += ways;
        if (verbose) std::cout << "Добавлено " << ways << " вариантов\n";
    }
    return total;
}

void print_vector(const std::bitset<64>& v, const std::string& label = "Вектор") {
    std::cout << label << " (битовая строка): ";
    for (int i = 63; i >= 0; --i) {
        std::cout << v[i];
        if (i % 8 == 0 && i != 0) std::cout << ' ';
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "==== Количество наборов веса w в сфере радиуса r вокруг x ====\n";
    std::bitset<64> x = input_bitset64_linux_live();
    print_vector(x, "Исходный вектор");
    int s = static_cast<int>(x.count());
    std::cout << "Вес исходного вектора (число единиц): " << s << "\n";

    int r, w;
    do {
        std::cout << "Введите радиус r (1 <= r <= 64): ";
        std::cin >> r;
        std::cin.ignore(1000, '\n');
    } while (r < 1 || r > 64);

    do {
        std::cout << "Введите желаемый вес w (0 <= w <= " << s << "): ";
        std::cin >> w;
        std::cin.ignore(1000, '\n');
    } while (w < 0 || w > s);

    uint64_t ans = count_vectors_in_sphere(x, r, w, true);
    std::cout << "\nОтвет: S_F(x) = " << ans << " наборов веса " << w << " в сфере радиуса " << r << " вокруг x\n";
    return 0;
}