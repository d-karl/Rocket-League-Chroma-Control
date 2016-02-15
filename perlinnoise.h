#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <QObject>

class PerlinNoise : public QObject
{
    Q_OBJECT
public:
    explicit PerlinNoise(QObject *parent = 0);
    ~PerlinNoise();

    PerlinNoise(QObject * parent, unsigned int seed);
    // Get a noise value, for 2D images z can have any value
    double noise(double x, double y, double z);

private:
    std::vector<int> p;
    double fade(double t);
    double lerp(double t, double a, double b);
    double grad(int hash, double x, double y, double z);
};

#endif // PERLINNOISE_H
