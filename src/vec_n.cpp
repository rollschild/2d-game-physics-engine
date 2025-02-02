#include "vec_n.h"
#include <stddef.h>

VecN::VecN() : N(0), data(nullptr) {}

VecN::VecN(int N) : N(N) { data = new float[N]; }

VecN::VecN(const VecN &v) {
    N = v.N;
    data = new float[N];
    for (int i = 0; i < N; i++) {
        data[i] = v.data[i];
    }
}

VecN::~VecN() { delete[] data; }

void VecN::zero() {
    for (int i = 0; i < N; i++) {
        data[i] = 0.0f;
    }
}

float VecN::dot(const VecN &v) const {
    float sum = 0.0f;
    for (int i = 0; i < N; i++) {
        sum += data[i] * v.data[i];
    }
    return sum;
}

VecN &VecN::operator=(const VecN &v) {
    delete[] data;
    N = v.N;
    data = new float[N];
    for (int i = 0; i < N; i++) {
        data[i] = v.data[i];
    }

    return *this;
}

VecN VecN::operator*(const float num) const {
    VecN result = *this;
    result *= num;
    return result;
}

const VecN &VecN::operator*=(const float num) {
    for (int i = 0; i < N; i++) {
        data[i] *= num;
    }
    return *this;
}

const VecN &VecN::operator+=(const VecN &v) {
    for (int i = 0; i < N; i++) {
        data[i] += v.data[i];
    }
    return *this;
}

const VecN &VecN::operator-=(const VecN &v) {
    for (int i = 0; i < N; i++) {
        data[i] -= v.data[i];
    }
    return *this;
}

VecN VecN::operator+(const VecN &v) const {
    VecN result = *this;

    for (int i = 0; i < N; i++) {
        result.data[i] += v.data[i];
    }

    return result;
}

VecN VecN::operator-(const VecN &v) const {
    VecN result = *this;

    for (int i = 0; i < N; i++) {
        result.data[i] -= v.data[i];
    }

    return result;
}

float &VecN::operator[](const int index) { return data[index]; }
float VecN::operator[](const int index) const { return data[index]; }
