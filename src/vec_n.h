#ifndef VEC_N_H
#define VEC_N_H

struct VecN {
    int N;
    float *data; // dynamic sized vector

    VecN();
    VecN(int N);
    VecN(const VecN &v);

    ~VecN();

    void zero(); // zero in every position of the vector: v1.zero()
    float dot(const VecN &v) const;

    VecN &operator=(const VecN &v); // v1 = v2
    VecN operator+(const VecN &v) const;
    VecN operator-(const VecN &v) const;
    VecN operator*(const float num) const;

    const VecN &operator+=(const VecN &v);
    const VecN &operator-=(const VecN &v);
    const VecN &operator*=(const float num);

    float operator[](const int index) const; // v4[index]
    float &operator[](const int index);
};

#endif
