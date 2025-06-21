#include <iostream>
#include <bitset>
#include <random>



void print_vector(const std::bitset<64>& v, const std::string& label = "Вектор") {
    std::cout << label << " (битовая строка): ";
    for (int i = 63; i >= 0; --i) {
        std::cout << v[i];
        if (i % 8 == 0 && i != 0) std::cout << ' ';
    }
    std::cout << std::endl;
}

int count_weight_in_sphere(const std::bitset<64>& x, int r, int w) {
    int n = 64;
    int s = static_cast<int>(x.count());
    int result = 0;
    for (int d = 0; d <= r; ++d) {
        for (int k = 0; k <= d; ++k) {
            if (s + d - 2 * k == w && k <= s && (d - k) <= n - s) {
                uint64_t ways = 1;
                for (int i = 0; i < k; ++i) ways = ways * (s - i) / (i + 1);
                for (int i = 0; i < d - k; ++i) ways = ways * (n - s - i) / (i + 1);
                result += static_cast<int>(ways);
            }
        }
    }
    return result;
}

std::bitset<64> random_bitset64() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> distrib(0, ~0ULL);
    uint64_t value = distrib(gen);
    return std::bitset<64>(value);
}

int main() {
 

    std::random_device rd;
    std::mt19937 gen(rd());

    std::bitset<64> x = random_bitset64();
    int n = 64;
    int s = static_cast<int>(x.count());
    const int r_max = 10;

    int r, w, answer = 0, attempts = 0;
    do {
        std::uniform_int_distribution<> distr_r(1, r_max);
        r = distr_r(gen);

        int w_min = std::max(0, s - r);
        int w_max = std::min(n, s + r);
        std::uniform_int_distribution<> distr_w(w_min, w_max);
        w = distr_w(gen);

        answer = count_weight_in_sphere(x, r, w);
        ++attempts;
    } while (answer == 0 && attempts < 100);

    std::cout << "Случайный 64-битный вектор:\n";
    print_vector(x, "x");

    std::cout << "\nАвтоматически подобраны параметры:\n";
    std::cout << "Вес исходного вектора (число единиц): " << s << std::endl;
    std::cout << "Радиус r: " << r << std::endl;
    std::cout << "Вес w: " << w << std::endl;

    std::cout << "\nРезультат:\n";
    std::cout << "Количество наборов веса " << w << " в сфере радиуса " << r << " вокруг x: " << answer << std::endl;

    return 0;
}