#ifndef VEC2_H
#define VEC2_H

#include <cmath>
#include <iostream>

struct Vec2 {
    // Coordonnées du vecteur
    double x;
    double z;

    // Constructeur par défaut
    Vec2() : x(0.0), z(0.0) {}

    // Constructeur avec paramètres
    Vec2(double x, double z) : x(x), z(z) {}

    // Surcharge de l'opérateur +
    Vec2 operator+(const Vec2& v) const {
        return Vec2(x + v.x, z + v.z);
    }

    // Surcharge de l'opérateur -
    Vec2 operator-(const Vec2& v) const {
        return Vec2(x - v.x, z - v.z);
    }

    // Surcharge de l'opérateur * (multiplication par un scalaire)
    Vec2 operator*(double scalar) const {
        return Vec2(x * scalar, z * scalar);
    }

    // Surcharge de l'opérateur +=
    Vec2& operator+=(const Vec2& v) {
        x += v.x;
        z += v.z;
        return *this;
    }

    // Surcharge de l'opérateur -=
    Vec2& operator-=(const Vec2& v) {
        x -= v.x;
        z -= v.z;
        return *this;
    }

    // Méthode pour calculer la norme du vecteur
    double norme() const {
        return std::sqrt(x * x + z * z);
    }

    // Méthode pour afficher le vecteur (pour le débogage)
    void afficher() const {
        std::cout << "(" << x << ", " << z << ")" << std::endl;
    }
};

#endif // VEC2_H
